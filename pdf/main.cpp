#include <iostream>
#include <hpdf.h>
#include <csetjmp>
#include <fmt/format.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "PDFWriter.h"
#include "PDFError.h"
#include "PDFCell.h"
#include "PDFColor.h"
#include <exception>

int main() {
    if (setjmp(PDF::g_env)) {
        std::cout << "Jump: " << PDF::g_error.error_message << std::endl;
        return 1;
    }

    try {
        PDF::Writer pdf("hello.pdf");
        pdf.enableCompression();             // after setjmp is active
        auto page = pdf.addPage();

        PDF::ClientRect rect {
            .rect={
                .topLeft={0, 0}, 
                .bottomRight={400, 300}}, 
                .margins={{10, PDF::RED}, {50, PDF::RED}, {10, PDF::RED}, {50, PDF::RED}},
                .borders={{2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}},
                .paddings={{50, PDF::BLUE}, {10, PDF::BLUE}, {20, PDF::BLUE}, {20, PDF::BLUE}},
                //.paddings={{10, 10, 10, 10}},
                //.margins={{10, 10, 10, 10}},
            .backgroundColor=PDF::LIGHT_RED
        };

        auto font = page.getFont("Courier-Bold");
        page.addCell(PDF::Cell{.rect = rect, .text = "Hello, World!", .properties = {.font = font, .fontSize = 30.0, .color = PDF::BLACK, .horizontalJustify = PDF::Justification::justifyCenter, .verticalJustify = PDF::Justification::justifyCenter}}, 50, 100);
        //page.addText(rect, "Hello, World!", "Helvetica", 20.0, PDF::BLACK);
        //page.drawRectangle(rect);


        

    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Exception: " << std::endl;
        return 1;
    }
    return 0;
}
