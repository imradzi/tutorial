#include "../include/PDFFont.h"
#include <fmt/format.h>
namespace PDF {
    Font::Font(HPDF_Doc pdf, const std::string& name, const char* encoding) : pdf(pdf), font(HPDF_GetFont(pdf, name.c_str(), encoding)) {
        font = HPDF_GetFont(pdf, name.c_str(), encoding);
        if (!font) {
            throw std::runtime_error(fmt::format("Error getting font: {}", name));
        }
    }
    Font::Font(HPDF_Doc pdf, HPDF_Font font) : pdf(pdf), font(font) {}
    std::tuple<HPDF_REAL, HPDF_REAL> Font::measureText(const std::string& text, HPDF_REAL width, HPDF_REAL font_size, HPDF_BOOL wordwrap, HPDF_REAL char_space, HPDF_REAL word_space) {
        HPDF_REAL real_width;
        auto w = HPDF_Font_MeasureText(font, reinterpret_cast<const HPDF_BYTE*>(text.c_str()), text.size(), width, font_size, char_space, word_space, wordwrap, &real_width);
        return {w, real_width};
    } 
}