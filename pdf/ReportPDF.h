#pragma once
#include "PDFWriter.h"
#include "PDFCell.h"
#include "PDFColor.h"
#include "PDFPage.h"
#include "PDFError.h"

namespace PDF {
    enum PointType {
        none,
        dot,
        diamond,
        plus,
        minus,
        x,
        checked,
        unchecked
    };

    struct Column {
        double flex;
        double width;
        std::string columnTitle;
        PDF::Cell cells;
    };

    class ReportPDF : public PDF::Document {
        std::string fileName;
        std::string title;
        std::vector<Column> columns;
        int x0, y0, xR, w0, h0;

    public:
        ReportPDF(const std::string &fileName, const std::string &title, std::vector<Column> &columns, PDF::PageOrientation orientation = PDF::PageOrientation::Portrait) : Document(fileName, orientation), fileName(fileName), title(title), columns(columns) {
            computeColumnWeightage(columns);
        }
        virtual ~ReportPDF();

        void computeColumnWeightage() {
            auto totalFlex = 0.0;
            for (auto &column : columns) {
                totalFlex += column.flex;
            }
            for (auto &column : columns) {
                column.width = column.flex / totalFlex;
            }
        }

        virtual std::tuple<HPDF_REAL, HPDF_REAL> writeList(ClientRect rect, const Cell &title, std::vector<Cell> &points, PointType pointType = PointType::dot);                                                      // return height;
        virtual std::tuple<HPDF_REAL, HPDF_REAL> writeLetterHead(ClientRect rect, const Cell &name2, const Cell &address, const Cell &regNo, const std::string &imageFileName, const std::string &eInvoiceQRstring);  // return lineNo where the below letterhead

        void run(std::function<std::vector<std::string> getRow()> fnGetRow) {
            int pageNo = 1;
            PDF::Page page = addPage();
            auto pageRect = page.getInnerRect();
            HPDF_REAL y = pageRect.topLeft.y;
            HPDF_REAL x = pageRect.topLeft.x;
            while (true) {
                auto row = fnGetRow();
                if (y > pageRect.bottomRight.y) {
                    //breakPage;
                }
                auto lineRect = ClientRect(x, y, pageRect.bottomRight.x, y + lineHeight);
                auto [w, h] = renderRow(pageNo, row, lineRect);
            }
        }
    };
}