#include "bmp.h"
#include "file.h"
#include "image.h"
#include "transform.h"
#include <stdio.h>

const char* write_a_output[] = {
    [WRITE_OK] = "Content written succesfully\n",
    [WRITE_ERROR] = "An error occurred while writing into the file\n"
};

const char* read_a_output[] = {
    [READ_OK] = "Content successfully read\n",
    [READ_ERROR] = "Error occured while reading the file\n"
};

void std_output(const char* a) {
    fprintf(stdout, "%s", a);
}

void std_error_output(const char* a) {
    fprintf(stderr, "%s", a);
}


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "%s", "Incorrect format, please input images!");
    }
    else {
        struct image source = { 0 };
        struct image result = { 0 };
        enum read_a read_image_source = get_image(argv[1], &source);
        if (read_image_source == 1) {
            std_error_output(read_a_output[read_image_source]);
            return 0;
        }
        else std_output(read_a_output[read_image_source]);
        result = rotate(&source);
        enum write_a set_image_source = save_image(argv[2], &result);
        if (set_image_source == 1) {
            std_error_output(write_a_output[set_image_source]);
            return 0;
        }
        else std_output(write_a_output[set_image_source]);
        delete_image(result);
        delete_image(source);
    }
    return 0;
}
