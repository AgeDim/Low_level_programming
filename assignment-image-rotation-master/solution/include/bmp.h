#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "image.h"
#include "status.h"

#ifndef BMP
#define BMP
struct __attribute__((__packed__)) bmp_header {
    uint16_t bfType;
    uint32_t bFileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#endif

enum read_a from_bmp(FILE* in, struct image* img);

enum write_a to_bmp(FILE* out, struct image const* img);
