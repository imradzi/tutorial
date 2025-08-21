// cc qr_png.c qrcodegen.c -o qr_png -lpng -lz
#pragma once

// Block style enum
typedef enum {
    BLOCK_STYLE_SQUARE = 0,
    BLOCK_STYLE_ROUNDED = 1
} BlockStyle;

std::tuple<int, int> qrcode_text_to_png_mem(const char *text, int scale, int border, unsigned char **pngBuf, size_t *pngSize, BlockStyle block_style=BLOCK_STYLE_SQUARE);
