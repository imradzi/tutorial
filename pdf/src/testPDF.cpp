#include <iostream>
#include <hpdf.h>
#include <csetjmp>
#include <fmt/format.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "../include/PDFWriter.h"
#include "../include/PDFError.h"
#include "../include/PDFCell.h"
#include "../include/PDFColor.h"
#include <exception>

void TestPDF() {
    if (setjmp(PDF::g_env)) {
        std::cout << "Jump: " << PDF::g_error.error_message << std::endl;
        return;
    }

    try {
        PDF::Writer pdf("hello.pdf");
        pdf.enableCompression();  // after setjmp is active
        auto page = pdf.addPage();

        PDF::ClientRect rect {
            .rect = {.topLeft = {0, 0},.bottomRight = {500, 400}},
            .margins = {{10, PDF::RED}, {50, PDF::RED}, {10, PDF::RED}, {50, PDF::RED}},
            .borders = {{2, PDF::BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}},
            .paddings = {{50, PDF::GREEN}, {10, PDF::BLUE}, {20, PDF::BLUE}, {20, PDF::BLUE}},
            .backgroundColor = PDF::YELLOW};

        HPDF_REAL x=10, y=10;
        auto font = page.getFont("Courier-Bold");
        auto [w, h] = page.addCell(PDF::Cell {
            .rect = rect, 
            .text = "Hello, World!", 
            .properties = {
                .font = font, 
                .fontSize = 30.0, 
                .color = PDF::BLUE, 
                .horizontalAlignment = PDF::Alignment::alignCenter, 
                .verticalAlignment = PDF::Alignment::alignCenter}
            }, x,y);

        page.addCell(PDF::Cell {
            .rect = rect,
            .text = "Second one",
            .properties = {
                .font = font,
                .fontSize = 30.0,
                .color = PDF::BLUE,
                .horizontalAlignment = PDF::Alignment::alignCenter,
                .verticalAlignment = PDF::Alignment::alignCenter}},
            x, y+h+2);

    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Exception: " << std::endl;
    }
}