#include "file.h"

enum read_a get_image(char* const filename, struct image* const image) {
	FILE* file;
	file = fopen(filename, "rb");
	if (file == NULL){return READ_ERROR;}
	enum read_a read_bmp_a = from_bmp(file, image);
	if (read_bmp_a != READ_OK){return read_bmp_a;}
	if(fclose(file) != 0 ){ return READ_ERROR;}
	return READ_OK;
}

enum write_a save_image(char* const filename, struct image* const image) {
	FILE* file;
	file = fopen(filename, "wb");
	if (file == NULL){return WRITE_ERROR;}
	enum write_a write_bmp_a = to_bmp(file, image);
	if (write_bmp_a != WRITE_OK){return write_bmp_a;}
	if(fclose(file) != 0){return WRITE_ERROR;}
	return WRITE_OK;
}
