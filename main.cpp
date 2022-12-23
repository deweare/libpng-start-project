#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <png.h>


struct pixel_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};


struct bitmap_t {
    pixel_t* pixels;
    size_t width;
    size_t height;
};


pixel_t* pixel_at(bitmap_t* bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

int save_png_to_file(bitmap_t* bitmap, const char* path)
{
    FILE* fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte** row_pointers = NULL;
    
    int pixel_size = 4;
    int depth = 8;

    fp = fopen(path, "wb");
    if (!fp) {
        return -1;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return - 1;
    }

    png_set_IHDR(png_ptr,
        info_ptr,
        bitmap->width,
        bitmap->height,
        depth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);


    row_pointers = static_cast<png_byte**>(png_malloc(png_ptr, bitmap->height * sizeof(png_byte*)));

    for (size_t y = 0; y < bitmap->height; ++y) {

        png_byte* row = static_cast<png_byte*>(
            png_malloc(png_ptr, sizeof(uint8_t) * bitmap->width * pixel_size));

        row_pointers[y] = row;

        for (size_t x = 0; x < bitmap->width; ++x) {

            pixel_t* pixel = pixel_at(bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
            *row++ = pixel->alpha;

        }

    }


    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for (size_t y = 0; y < bitmap->height; y++)
        png_free(png_ptr, row_pointers[y]);
   
    png_free(png_ptr, row_pointers);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}


int main()
{
    bitmap_t png_img;

    png_img.width = 400;
    png_img.height = 400;

    png_img.pixels = static_cast<pixel_t*>(calloc(sizeof(pixel_t), png_img.width * png_img.height));
    
    for (int x = 0; x < png_img.width; x++) {
        for (int y = 0; y < png_img.height; y++) {

            pixel_t* pixel = pixel_at(&png_img, x, y);

            pixel->red = (y + 80) % 170;
            pixel->green = (y + 11) % 120;
            pixel->blue = (x + 30) % 255;
            pixel->alpha = (x * 4) % 255;

        }
    }

    save_png_to_file(&png_img, "..\\image.png");

    free(png_img.pixels);

    return 0;
}