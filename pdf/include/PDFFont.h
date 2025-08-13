#pragma once
#include <hpdf.h>
#include <string>
#include <tuple>

namespace PDF {
    class Font {
        HPDF_Doc pdf;
        HPDF_Font font;
    public:
        Font(HPDF_Doc pdf, const std::string& name, const char* encoding = nullptr);
        Font(HPDF_Doc pdf, HPDF_Font font);
        auto getFontName() { return HPDF_Font_GetFontName(font); }
        auto getEncodingName() { return HPDF_Font_GetEncodingName(font); }
        auto getUnicodeWidth(wchar_t w) { return HPDF_Font_GetUnicodeWidth(font, w); }
        auto getBBox() { return HPDF_Font_GetBBox(font); }
        auto getAscent() { return HPDF_Font_GetAscent(font); }
        auto getDescent() { return HPDF_Font_GetDescent(font); }
        auto getXHeight() { return HPDF_Font_GetXHeight(font); }
        auto getCapHeight() { return HPDF_Font_GetCapHeight(font); }
        auto textWidth(const std::string& text) { return HPDF_Font_TextWidth(font, reinterpret_cast<const HPDF_BYTE*>(text.c_str()), text.size()); }
        std::tuple<HPDF_REAL, HPDF_REAL> measureText(const std::string& text, HPDF_REAL width, HPDF_REAL font_size, HPDF_BOOL wordwrap=false, HPDF_REAL char_space=0, HPDF_REAL word_space=0);
    };   
}