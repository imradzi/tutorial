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
#include "ReportPDF.h"
#include <exception>

int main() {
    if (setjmp(PDF::g_env)) {
        std::cout << "Jump: " << PDF::g_error.error_message << std::endl;
        return 0;
    }

    try {
        PDF::Writer pdf("hello.pdf");
        pdf.enableCompression();  // after setjmp is active
        auto page = pdf.addPage();
        const auto fontSize = 3;
        HPDF_Page_SetFontAndSize(page(), page.getFont("Courier"), fontSize);
        auto rect = page.getInnerRect();
        auto height = page.getTextHeight() + 10;
        HPDF_REAL x = 10;
        for (HPDF_REAL y = rect.topLeft.y; y < rect.bottomRight.y; y += height+5) {
            page.writeText({ .x = x, .y = y}, fmt::format("({}, {}), h={}", x, y, height));

            PDF::ClientRect innerRect = PDF::ClientRect::init({
                .rect {.topLeft {x + 50, y - height}, .bottomRight {x + 100, y}}, 
                .borders {{.size = 0.5, .color = PDF::BLACK}, {.size = 0.5, .color = PDF::BLACK}, {.size = 0.5, .color = PDF::BLACK}, {.size = 0.5, .color = PDF::BLACK}}
            });

            page.addText(innerRect, "Hello, World!", PDF::TextProperties{ .font = page.getFont("Courier"), .fontSize = fontSize, .color = PDF::BLACK, .horizontalAlignment = PDF::Alignment::alignCenter, .verticalAlignment = PDF::Alignment::alignCenter });
        }

        // PDF::Coord qrPoint {80, 40};
        // const auto size = 80;
        // PDF::ClientRect crect = PDF::ClientRect::init({
        //     .rect = {.topLeft = qrPoint, .bottomRight = qrPoint.moveBy(size,size)},
        //     //.margins = {{10, PDF::RED}, {10, PDF::RED}, {10, PDF::RED}, {10, PDF::RED}},
        //     //.borders = {{2, PDF::BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}, {2, PDF::LIGHT_BLUE}},
        //     //.paddings = {{10, PDF::GREEN}, {10, PDF::BLUE}, {10, PDF::BLUE}, {10, PDF::BLUE}},
        //     //.backgroundColor = PDF::YELLOW 
        // });

        // page.drawQR(crect, 
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     "okajs jkdfk akjkd akjla dfjk akdjfl jakjdlal jalkjdlajl ladlkfalkj lkajdflakjl kajldjfalkj lajdlkjfalj "
        //     , 20, 4);


        //HPDF_REAL x = 10, y = 10;
        //auto font = page.getFont("Courier-Bold");
        //auto [w, h] = page.addCell(PDF::Cell {
        //                               .rect = rect,
        //                               .text = "Hello, World!",
        //                               .properties = {
        //                                   .font = font,
        //                                   .fontSize = 30.0,
        //                                   .color = PDF::BLUE,
        //                                   .horizontalAlignment = PDF::Alignment::alignCenter,
        //                                   .verticalAlignment = PDF::Alignment::alignCenter}},
        //    x, y);

        //page.addCell(PDF::Cell {
        //                 .rect = rect,
        //                 .text = "Second one",
        //                 .properties = {
        //                     .font = font,
        //                     .fontSize = 30.0,
        //                     .color = PDF::BLUE,
        //                     .horizontalAlignment = PDF::Alignment::alignCenter,
        //                     .verticalAlignment = PDF::Alignment::alignCenter}},
        //    x, y + h + 2);

        //PDF::ReportPDF report("report.pdf", "Report", {
        //                                                  {.flex = 1, .width = 0, .columnTitle = "Name", .cell {}},
        //                                                  {.flex = 3, .width = 0, .columnTitle = "Address", .cell {}},
        //                                                  {.flex = 2, .width = 0, .columnTitle = "Address", .cell {}},
        //                                                  {.flex = 1, .width = 0, .columnTitle = "Phone", .cell {}},
        //                                              });

        //report.setFont("Helvetica", 12);

        //std::vector<std::vector<std::string>> rows = {
        //    {"John Doe", "123 Main St", "555-1234-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", "00000"},
        //    {"Jane Smith", "456 Elm St", "555-5678-yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy", "11111"},
        //    {"Jim Beam", "789 Oak St", "555-9101-zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", "22222"}
        //};

        //auto prop = PDF::TextProperties{ .font = report.getFont("Helvetica"), .fontSize = 12, .color = PDF::BLACK, .horizontalAlignment = PDF::Alignment::alignLeft, .verticalAlignment = PDF::Alignment::alignCenter };
        //report.run(PDF::ReportPDF::Parameters{
        //    .getRow = [rows](size_t r) {
        //        if (r >= rows.size())
        //            return std::vector<std::string>{};
        //        else
        //            return rows[r];
        //    },
        //    .createLineRect = [&prop, &report](PDF::Coord topLeft, HPDF_REAL height) {
        //        PDF::ClientRect rect = PDF::ClientRect::init({
        //            .rect = {.topLeft = topLeft, .bottomRight = topLeft},
        //            .borders {{0.5, PDF::LIGHT_GRAY}, {0.5, PDF::LIGHT_GRAY}, {0.5, PDF::LIGHT_GRAY}, {0.5, PDF::LIGHT_GRAY}},
        //            .paddings {{5}, {5}, {5}, {5}}
        //        });
        //        rect.rect.setHeight(height + rect.getBoundingSize(PDF::top) + rect.getBoundingSize(PDF::bottom));
        //        return PDF::ClientRect::init(std::move(rect)); // recalculate the inner rects;
        //    }, // same point for bottom right so that addText will use fontsize to calculate
        //    .createInnerRect = [](int col, PDF::Rect rect) {
        //        return PDF::ClientRect::init({
        //            .rect = rect,
        //            .borders {{0.5, PDF::BLACK}, {0.5, PDF::BLACK}, {0.5, PDF::BLACK}, {0.5, PDF::BLACK}},
        //            .paddings { {0.0}, {5.0}, {0.0}, {5.0}}}
        //        ); // in need color/margin/etc ... can be defined here.
        //    },
        //    .renderRow = [&](const std::vector<std::string> &row, const PDF::ClientRect &lineRect, std::function<PDF::ClientRect(int col, PDF::Rect rect)> fn) {
        //        return report.drawLine(lineRect, row, prop, fn);
        //    },
        //});
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Exception: " << std::endl;
    }
    return 0;
}
