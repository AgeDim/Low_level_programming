#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#ifndef IMAGE_STRUCTS
#define IMAGE_STRUCTS
struct pixel { uint8_t b, g, r; };

struct image {
	uint64_t width, height;
	struct pixel* data;
};
#endif

struct image create_img(size_t width, size_t height);

void delete_image(struct image image);
