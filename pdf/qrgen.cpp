// qr_demo.cpp - QR code generation with PNG in memory using qrcodegen.cpp
#include <png.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "qrcodegen.hpp"  // qrcodegen.cpp header

typedef struct {
    uint8_t *data;
    int width;
    int height;
} GrayImage;

// Block style enum
typedef enum {
    BLOCK_STYLE_SQUARE = 0,
    BLOCK_STYLE_ROUNDED = 1
} BlockStyle;

// Render QR code to grayscale buffer using qrcodegen.cpp
static bool render_qrcode_to_gray_buffer(const char *text, int scale, int border, BlockStyle block_style, GrayImage *out_img) { 
    using namespace qrcodegen;
    
    // Create QR code
    auto qr = qrcodegen::QrCode::encodeText(text, qrcodegen::QrCode::Ecc::LOW);
    
    int size = qr.getSize();
    int img_w = (size + border * 2) * scale;
    int img_h = img_w;

    uint8_t *buf = (uint8_t *)malloc((size_t)img_w * img_h);
    if (!buf) return false;
    memset(buf, 255, (size_t)img_w * img_h); // white background

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (!qr.getModule(x, y)) continue; // white module
            int px0 = (border + x) * scale;
            int py0 = (border + y) * scale;
            
            if (block_style == BLOCK_STYLE_SQUARE) {
                // Draw square blocks (original behavior)
                for (int dy = 0; dy < scale; dy++) {
                    uint8_t *row = buf + (size_t)(py0 + dy) * img_w + px0;
                    memset(row, 0, (size_t)scale); // black block
                }
            } else {
                // Draw rounded/circular dots
                int center_x = px0 + scale / 2;
                int center_y = py0 + scale / 2;
                int radius = scale / 2;
                
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        int pixel_x = px0 + dx;
                        int pixel_y = py0 + dy;
                        
                        // Calculate distance from center
                        int dist_sq = (pixel_x - center_x) * (pixel_x - center_x) + 
                                     (pixel_y - center_y) * (pixel_y - center_y);
                        
                        // If pixel is within circle radius, make it black
                        if (dist_sq <= radius * radius) {
                            uint8_t *pixel = buf + (size_t)pixel_y * img_w + pixel_x;
                            *pixel = 0; // black pixel
                        }
                    }
                }
            }
        }
    }

    out_img->data = buf;
    out_img->width = img_w;
    out_img->height = img_h;
    return true;
}

// PNG memory buffer
typedef struct {
    unsigned char *data;
    size_t size;
    size_t capacity;
} MemBuffer;

static void mem_write_fn(png_structp png_ptr, png_bytep data, png_size_t len) {
    MemBuffer *m = (MemBuffer *)png_get_io_ptr(png_ptr);
    if (m->size + len > m->capacity) {
        size_t new_cap = m->capacity ? m->capacity * 2 : 4096;
        while (new_cap < m->size + len) new_cap *= 2;
        unsigned char *new_data = (unsigned char *)realloc(m->data, new_cap);
        if (!new_data) {
            png_error(png_ptr, "out of memory");
            return;
        }
        m->data = new_data;
        m->capacity = new_cap;
    }
    memcpy(m->data + m->size, data, len);
    m->size += len;
}

static void mem_flush_fn(png_structp png_ptr) {
    (void)png_ptr;
}

// Encode grayscale to PNG in memory
static bool png_encode_grayscale_to_memory(const uint8_t *gray, int width, int height, unsigned char **out_png, size_t *out_size) {
    *out_png = NULL;
    *out_size = 0;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return false;
    png_infop info = png_create_info_struct(png);
    if (!info) { png_destroy_write_struct(&png, NULL); return false; }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        free(*out_png);
        *out_png = NULL;
        *out_size = 0;
        return false;
    }

    MemBuffer mem = {0};
    png_set_write_fn(png, &mem, mem_write_fn, mem_flush_fn);

    png_set_IHDR(png, info, (png_uint_32)width, (png_uint_32)height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    png_bytep *rows = (png_bytep *)malloc((size_t)height * sizeof(png_bytep));
    if (!rows) { png_error(png, "out of memory"); }
    for (int y = 0; y < height; y++) {
        rows[y] = (png_bytep)(gray + (size_t)y * width);
    }

    png_write_image(png, rows);
    png_write_end(png, info);

    free(rows);
    png_destroy_write_struct(&png, &info);

    if (mem.size == 0) {
        free(mem.data);
        return false;
    }

    *out_png = mem.data;
    *out_size = mem.size;
    return true;
}

// Main function: text -> PNG bytes in memory
bool qrcode_text_to_png_mem(const char *text, int scale, int border, unsigned char **pngBuf, size_t *pngSize, BlockStyle block_style) {
    GrayImage img = {0};
    if (!render_qrcode_to_gray_buffer(text, scale, border, block_style, &img)) return false;

    bool ok = png_encode_grayscale_to_memory(img.data, img.width, img.height, pngBuf, pngSize);

    free(img.data);
    return ok;
}