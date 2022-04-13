#ifndef FILE_OPS
#define FILE_OPS
#include <stdio.h>
#include "image.h"
#include "status.h"
#include "bmp.h"
enum read_a get_image(char* const filename, struct image* const image);
enum write_a save_image(char* const filename, struct image* const image);
#endif
