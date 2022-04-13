#include "image.h"

struct image create_img(size_t width, size_t height) { 
 struct image image = { 0 }; 
 image.width = width; 
 image.height = height; 
    image.data = malloc(width * height * sizeof(struct pixel)); 
 return image; 
}

void delete_image(struct image image) {
	free(image.data);
}
