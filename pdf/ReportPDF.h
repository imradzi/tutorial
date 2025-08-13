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
        Page *currentPage = nullptr;
        struct Parameters {
            std::function<std::vector<std::string>(size_t rowNo)> getRow;
            std::function<ClientRect(Coord)> createLineRect;
            std::function<HPDF_REAL(const std::vector<std::string> &, const ClientRect &)> renderRow;
            std::function<void(Page, int)> beginOfPage;
            std::function<void(Page, int)> endOfPage;
            std::function<void(Page, int)> endOfDocument;
        };

        ReportPDF(const std::string &fileName, const std::string &title, const std::vector<Column> &columns, PDF::PageOrientation orientation = PDF::PageOrientation::Portrait);
        ~ReportPDF();
        void computeColumnWeightage();
        virtual std::tuple<HPDF_REAL, HPDF_REAL> writeList(ClientRect rect, const Cell &title, std::vector<Cell> &points, PointType pointType = PointType::dot);                                                      // return height;
        virtual std::tuple<HPDF_REAL, HPDF_REAL> writeLetterHead(ClientRect rect, const Cell &name2, const Cell &address, const Cell &regNo, const std::string &imageFileName, const std::string &eInvoiceQRstring);  // return lineNo where the below letterhead
        HPDF_STATUS drawLine(ClientRect outerRect, const std::vector<std::string> &row) const;
        void run(Parameters fn);
    };
}