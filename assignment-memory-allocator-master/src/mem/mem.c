#include <stdarg.h>
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mem.h"
#include "mem_internals.h"
#include "./util/util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool block_is_big_enough( size_t query, struct block_header* block ) {
	return block->capacity.bytes >= query; 
	}
	
static size_t pages_count   ( size_t mem ){ return mem / getpagesize() + ((mem % getpagesize()) > 0); }

static size_t round_pages   ( size_t mem ){ return getpagesize() * pages_count( mem ) ; }

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );



static void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , 0, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region  ( void const * addr, size_t query ) {
  struct region reg;
  query = region_actual_size(query);
  void *reg_address = map_pages(addr, query, MAP_FIXED_NOREPLACE);
  if (reg_address == MAP_FAILED) {
    reg_address = map_pages(addr, query, 0);
    if(reg_address == MAP_FAILED){return REGION_INVALID;}
    reg = (struct region){reg_address, query, false};
  } else {
    reg = (struct region){reg_address, query, false};
  }
  block_init(reg_address, (block_size){query}, NULL);
  return reg;
}

static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big( struct block_header* block, size_t query ) {
  if (block_splittable(block, query) == true) {
    void *new_block = (void *)((uint8_t *)block +
                               offsetof(struct block_header, contents) + query);
    block_init(new_block, (block_size){block->capacity.bytes - query}, NULL);
    block->next = new_block;
    block->capacity.bytes = query;
    return true;
  }
  return false;
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block )              {
  return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header* block ) {
  struct block_header *next = block->next;
  if (next != NULL && mergeable(block, next) == true) {
    block->next = next->next;
    block->capacity.bytes += size_from_capacity(next->capacity).bytes;
    return true;
  }
  return false;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};


static struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz )    {
  struct block_header *current = block;
  struct block_header *last = block;
  while (current != NULL) {
    while (current->is_free && (block_is_big_enough(sz, current) ||
                                      try_merge_with_next(current))) {
      if (block_is_big_enough(sz, current)) {
        return (struct block_search_result){.type = BSR_FOUND_GOOD_BLOCK,
                                            .block = current};
      }
    }
    last = current;
    current = current->next;
  }
  return (struct block_search_result){.type = BSR_REACHED_END_NOT_FOUND,
                                      .block = last};
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
  const struct block_search_result search_result = find_good_or_last(block, query);
  if (search_result.type != BSR_FOUND_GOOD_BLOCK) return search_result;
  struct block_header* suitable_block = search_result.block;
  split_if_too_big(suitable_block, query);
  suitable_block->is_free = false;

  return (struct block_search_result) { .type = BSR_FOUND_GOOD_BLOCK, .block = suitable_block };
}



static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  block_size size = size_from_capacity(last->capacity);
  void *new_heap_address = (void *)((uint8_t *)last + size.bytes);
  struct region region = alloc_region(new_heap_address, query);
  if(region_is_invalid(&region)){return NULL;}
  block_init(region.addr, (block_size){region.size}, NULL);
  last->next = (struct block_header *)region.addr;
  if (try_merge_with_next(last)) {
    return last;
  } else {
    return last->next;
  }
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {
  query = size_max(query, BLOCK_MIN_CAPACITY);
   struct block_search_result res = try_memalloc_existing(query, heap_start);
  struct block_header* new_block = grow_heap(res.block, query);
  switch (res.type) {
  case BSR_REACHED_END_NOT_FOUND:
     if (new_block == NULL) return NULL;
    new_block = try_memalloc_existing(query, new_block).block;
    return new_block;
  case BSR_FOUND_GOOD_BLOCK:
    break;
  case BSR_CORRUPTED:
    err("Block working not correct!");
    return NULL;
  }
  res.block->is_free = false;
  return res.block;

}

void* _malloc( size_t query ) {
  struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;
}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  while (try_merge_with_next(header));
}
