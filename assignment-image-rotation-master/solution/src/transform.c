#include "transform.h"
#include "image.h"

static struct pixel get_pixel(const struct image* const img, const size_t row, const size_t col) {
    return img->data[row * img->width + col];
}

static void set_pixel(struct image* const img, const size_t row, const size_t col, const struct pixel pixel) {
    img->data[row * img->width + col] = pixel;
}

struct image rotate(struct image* const img) {
	const uint32_t width = img->width;
    const uint32_t height = img->height;
    struct image result = create_img(height, width);
		for (size_t i = 0; i < height; ++i) {
        for(size_t j = 0; j < width; ++j) {
            const struct pixel pixel = get_pixel(img, i, j);
            set_pixel(&result, j, height - i - 1, pixel);
        }
    }


	return result;	

}
