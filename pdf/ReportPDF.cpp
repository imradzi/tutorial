#include "ReportPDF.h"
#include <tuple>
namespace PDF {
    ReportPDF::ReportPDF(const std::string &fileName, const std::string &title, const std::vector<Column> &columns, PDF::PageOrientation orientation) : Document(fileName, orientation), fileName(fileName), title(title), columns(columns) {
        computeColumnWeightage();
    }
    ReportPDF::~ReportPDF() {}

    void ReportPDF::computeColumnWeightage() {
        if (currentPage == nullptr) return;
        auto totalFlex = 0.0;
        for (auto &column : columns) {
            totalFlex += column.flex;
        }
        for (auto &column : columns) {
            column.width = column.flex / totalFlex * currentPage->getClientSize().width;
        }
    }

    std::tuple<HPDF_REAL, HPDF_REAL> ReportPDF::writeList(ClientRect rect, const Cell &title, std::vector<Cell> &points, PointType pointType) {
        return { HPDF_REAL(0.0), HPDF_REAL(0.0)};
    }  // return height;
    std::tuple<HPDF_REAL, HPDF_REAL> ReportPDF::writeLetterHead(ClientRect rect, const Cell &name2, const Cell &address, const Cell &regNo, const std::string &imageFileName, const std::string &eInvoiceQRstring) {
        return { HPDF_REAL(0.0), HPDF_REAL(0.0) };
    }  // return lineNo where the below letterhead

    HPDF_REAL ReportPDF::drawLine(ClientRect crect, const std::vector<std::string> &row, TextProperties prop) const {
        HPDF_REAL height = 0;
        auto rc = currentPage->drawWidget(crect, crect.backgroundColor, [this, row, prop, &height](Rect innerRect) {
            auto x = innerRect.topLeft.x;
            int colNo = 0;
            for (auto &str : row) {
                innerRect.moveTo(Coord(x, innerRect.topLeft.y));
                innerRect.setWidth(columns[colNo].width);
                auto [w, h] = currentPage->addText(ClientRect {.rect = innerRect}, str, prop); // lost the margin, borders and padding.
                x += columns[colNo].width;
                height = std::max({ h, innerRect.getHeight(), height });
                colNo++;
            }
            return HPDF_OK;
        });
        return height;
    }

    void ReportPDF::run(ReportPDF::Parameters fn) {
        int pageNo = 1;
        auto page = addPage();
        auto pageRect = page.getInnerRect();
        auto y = pageRect.topLeft.y;
        auto x = pageRect.topLeft.x;
        auto h = page.getTextHeight();
        if (fn.beginOfPage) fn.beginOfPage(page, pageNo);
        size_t rowNo = 0;
        while (true) {
            if (!fn.getRow) break;
            auto row = fn.getRow(rowNo);
            if (row.empty()) break;
            if (y > pageRect.bottomRight.y) {
                if (fn.endOfPage) fn.endOfPage(page, pageNo);
                y = pageRect.topLeft.y;
                pageNo++;
                page = addPage();
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
