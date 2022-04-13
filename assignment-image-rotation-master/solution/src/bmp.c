#include "bmp.h"
#include "image.h"

#define TYPE 19778
#define PLANES 1
#define DIB_SIZE 40
#define BPP 24
#define COMP 0
#define X_PPM 0
#define Y_PPM 0
#define IMP_COLORS 0
#define NUM_COLORS 0
#define HEADER_SIZE 54

static enum read_a read_header(FILE* in,struct bmp_header* header) {

	if (fread(header, sizeof(struct bmp_header), 1, in) == 1)
		return READ_OK;
	else return READ_ERROR;
}

static uint32_t define_padding(uint32_t width) { 
    const uint8_t padding = (width * sizeof(struct pixel)) % 4; 
    return padding ? 4 - padding : 0; 
 
}

static enum read_a read_data(FILE* in,const struct image img) {
	uint32_t padding = define_padding(img.width);
	struct pixel* pixels = img.data;
	size_t width = img.width;
	const size_t height = img.height;
	for (size_t i = 0; i < height; i++) {
		if (fread(pixels + width * i, sizeof(struct pixel), width, in) != width) {
			free(pixels);
			return READ_ERROR;
		}
		if (fseek(in, padding, SEEK_CUR) != 0) {
			free(pixels);
			return READ_ERROR;
		}
	}
	return READ_OK;
}

static enum write_a write_header(FILE* out, struct image const* img) {
	uint32_t padding = define_padding(img->width);
	struct bmp_header header = (struct bmp_header){ 0 };
	size_t image_size = sizeof(struct pixel) * img->width * img->height + padding * img->height;
	header.bfType = TYPE;
	header.bFileSize = HEADER_SIZE + image_size;
	header.bOffBits = HEADER_SIZE;
	header.biSize = DIB_SIZE;
	header.biWidth = img->width;
	header.biHeight = img->height;
	header.biPlanes = PLANES;
	header.biBitCount = BPP;
	header.biCompression = COMP;
	header.biSizeImage = image_size;
	header.biXPelsPerMeter = X_PPM;
	header.biYPelsPerMeter = Y_PPM;
	header.biClrUsed = NUM_COLORS;
	header.biClrImportant = IMP_COLORS;
	if (fwrite(&header, sizeof(struct bmp_header), 1, out) != 1)
		return WRITE_ERROR;
	else return WRITE_OK;
}

static enum write_a write_content(FILE* out, struct image const img) {
	uint32_t padding = define_padding(img.width);
	const size_t width = img.width;
	const size_t height = img.height;
	struct pixel* current_pixel = img.data;
	for (size_t i = 0; i < height; i++) {
		if (fwrite(current_pixel, sizeof(struct pixel), width, out) != width)
			return WRITE_ERROR;
		if (fwrite(current_pixel, 1, padding, out) != padding)
			return WRITE_ERROR;
		current_pixel = current_pixel + (size_t)width;
	}
	return WRITE_OK;
}

enum write_a to_bmp(FILE* out, struct image const* img) {
	if (write_header(out, img) != WRITE_OK)
		return WRITE_ERROR;
	return write_content(out, *img);
}

enum read_a from_bmp(FILE* in, struct image* img) { 
    struct bmp_header header = { 0 }; 
 enum read_a header_check = read_header(in, &header); 
 if (header_check != READ_OK) 
  return header_check; 
    *img = create_img(header.biWidth,header.biHeight); 
 enum read_a read_check = read_data(in, *img); 
 return read_check; 
}
