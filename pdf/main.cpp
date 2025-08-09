#include <iostream>
#include <hpdf.h>
#include <csetjmp>
#include <fmt/format.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFError.h"
#include "PDFCell.h"
#include "PDFColor.h"
#include "PDFFont.h"

int main() {
    if (setjmp(PDF::g_env)) {
        std::cout << "Jump: " << PDF::g_error.error_message << std::endl;
        return 1;
    }

    try {
        PDF::Writer pdf("hello.pdf");
        pdf.enableCompression();             // after setjmp is active
        auto page = pdf.addPage();
        page.setBorderWidth(1.5);
        page.setBorderColor(PDF::LIGHT_GRAY);
        int x=50, y=50;
        auto constexpr spacing = 20;
        y += page.addText("Hello, World!", "Helvetica", 40, x, y, true) + spacing;
        y += page.addText("Wow! This is a Courier test!", "Courier", 18, x, y, true) + spacing;
        auto yy= y - page.getTextHeight("Times-Roman", 30);
        if (yy < y) y = yy+spacing; // move down if overlapped
        y += page.addText("Wow! This is a Times-Roman test!", "Times-Roman", 30, x, y, true) + spacing;
        page.addText("Wow! This is a Symbol test!", "Symbol", 18, x, y,  true, 20);
        y+= 20 +spacing;
        page.addText("Wow! This is a ZapfDingbats test!", "ZapfDingbats", 18, x, y, true, 20);
        y+= 20 +spacing;

        auto font = page.getFont("Courier");
        
        std::vector<PDF::Cell> cells = {
            {.width=200,.text="Line 000", .font=font, .fontSize=20},
            {.width=200,.text="Line 001", .font=font, .fontSize=20},
            {.width=200,.text="Line 002, fontsize=10", .font=font, .fontSize=10},
            {.padding=10, .width=0,.text="Line 003, fontsize=40", .font=font, .fontSize=40},
            {.width=200,.text="Line 004", .font=font, .fontSize=20}
        };

        std::vector<PDF::Cell> hcells = {
            {.border_thickness=0.5, .width=0, .height=0, .text="Horz Line 000", .font=font, .fontSize=5, .background_color=PDF::LIGHT_BLUE, .horizontal_justify=PDF::Justification::center, .vertical_justify=PDF::Justification::center},
            {.border_thickness=0.5, .width=0, .height=0, .text="Horz Line 001", .font=font, .fontSize=5, .background_color=PDF::LIGHT_GREEN, .horizontal_justify=PDF::Justification::center, .vertical_justify=PDF::Justification::center},
            {.border_thickness=0.5, .width=0, .height=0, .text="Horz Line 002", .font=font, .fontSize=5, .background_color=PDF::LIGHT_RED},
            {.border_thickness=0.5, .width=0, .height=0, .text="Horz Line 003", .font=font, .fontSize=5, .background_color=PDF::LIGHT_YELLOW},
            {.border_thickness=0.5, .width=0, .height=0, .text="Horz Line 004", .font=font, .fontSize=5, .background_color=PDF::LIGHT_MAGENTA}
        };


        for (const auto &c: cells) {
            auto thisCellHeight = page.getCellHeight(c);
            auto [w, h] = page.addCell(c, x, y+thisCellHeight);
            y += h;
        }

        x=10;
        y += 50;
        for (const auto &c: hcells) {
            auto thisCellWidth = page.getCellWidth(c);
            auto [w, h] = page.addCell(c, x, y);
            x += w-c.border_thickness;

        }

        x=10;
        y += 50;
        for (const auto &c: hcells) {
            auto thisCellWidth = page.getCellWidth(c);
            auto [w, h] = page.addConstrainedCell(c, x, y, thisCellWidth, 0);
            x += w-c.border_thickness;

        }


    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
