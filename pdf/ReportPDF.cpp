#include "ReportPDF.h"
#include <tuple>
namespace PDF {
    ReportPDF::ReportPDF(const std::string &fileName, const std::string &title, const std::vector<Column> &columns, PDF::PageOrientation orientation) : Document(fileName, orientation), fileName(fileName), title(title), columns(columns) {
        computeColumnWeightage();
    }
    ReportPDF::~ReportPDF() {}

    void ReportPDF::computeColumnWeightage() {
        auto totalFlex = 0.0;
        for (auto &column : columns) {
            totalFlex += column.flex;
        }
        for (auto &column : columns) {
            column.width = column.flex / totalFlex;
        }
    }

    std::tuple<HPDF_REAL, HPDF_REAL> ReportPDF::writeList(ClientRect rect, const Cell &title, std::vector<Cell> &points, PointType pointType) {
        return {0, 0};
    }  // return height;
    std::tuple<HPDF_REAL, HPDF_REAL> ReportPDF::writeLetterHead(ClientRect rect, const Cell &name2, const Cell &address, const Cell &regNo, const std::string &imageFileName, const std::string &eInvoiceQRstring) {
        return {0, 0};
    }  // return lineNo where the below letterhead

    HPDF_STATUS ReportPDF::drawLine(ClientRect outerRect, const std::vector<std::string> &row) const {
        return currentPage->drawWidget(outerRect, outerRect.backgroundColor, [this, row](Rect innerRect) {
            auto x = innerRect.topLeft.x;
            int colNo = 0;
            for (auto &str : row) {
                innerRect.moveTo(Coord(x, innerRect.topLeft.y));
                innerRect.setWidth(columns[colNo].width);
                currentPage->addText(ClientRect {.rect = innerRect}, str, TextProperties {.font = currentPage->getFont("Helvetica"), .fontSize = 12, .color = PDF::BLACK, .horizontalAlignment = PDF::Alignment::alignLeft, .verticalAlignment = PDF::Alignment::alignCenter});
                x += columns[colNo].width;
                colNo++;
            }
            return HPDF_OK;
        });
    }

    void ReportPDF::run(ReportPDF::Parameters fn) {
        int pageNo = 1;
        auto page = addPage();
        auto font = page.getFont("Helvetica");
        currentPage = &page;
        auto pageRect = page.getInnerRect();
        auto y = pageRect.topLeft.y;
        auto x = pageRect.topLeft.x;
        auto h = page.getTextHeight();
        if (fn.beginOfPage) fn.beginOfPage(page, pageNo);
        size_t rowNo = 0;
        while (true) {
            if (!fn.getRow) break;
            auto row = fn.getRow(rowNo);
            if (y > pageRect.bottomRight.y) {
                if (fn.endOfPage) fn.endOfPage(page, pageNo);
                y = pageRect.topLeft.y;
                pageNo++;
                page = addPage();
                currentPage = &page;
                pageRect = page.getInnerRect();
                if (fn.beginOfPage) fn.beginOfPage(page, pageNo);
            }
            if (!fn.renderRow) break;
            auto h = fn.renderRow(row, fn.createLineRect(Coord(x, y)));
            y += h;
            rowNo++;
        }
        if (fn.endOfPage) fn.endOfPage(page, pageNo);
        if (fn.endOfDocument) fn.endOfDocument(page, pageNo);
    }
}
