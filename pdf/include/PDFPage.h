#pragma once
#include <hpdf.h>
#include <string>
#include <unordered_set>
#include <tuple>
#include "PDFCell.h"
#include "PDFColor.h"
#include "PDFFont.h"
#include <functional>

namespace PDF {

    enum PageSizes {
        Letter = HPDF_PAGE_SIZE_LETTER, //      8.5 x 11 (inches)         612 x 792
        Legal = HPDF_PAGE_SIZE_LEGAL, //       8.5 x 14 (inches)         612 x 1008
        A3 = HPDF_PAGE_SIZE_A3, //          297 x 420 (mm)         841.89 x 1199.551
        A4 = HPDF_PAGE_SIZE_A4, //          210 x 297 (mm)        595.276 x 841.89
        A5 = HPDF_PAGE_SIZE_A5, //          148 x 210 (mm)        419.528 x 595.276
        B4 = HPDF_PAGE_SIZE_B4, //          250 x 353 (mm)        708.661 x 1000.63
        B5 = HPDF_PAGE_SIZE_B5, //          176 x 250 (mm)        498.898 x 708.661
        Executive = HPDF_PAGE_SIZE_EXECUTIVE, //  7.25 x 10.5 (inches)       522 x 756
        US4x6 = HPDF_PAGE_SIZE_US4x6, //         4 x 6 (inches)          288 x 432
        US4x8 = HPDF_PAGE_SIZE_US4x8, //         4 x 8 (inches)          288 x 576
        US5x7 = HPDF_PAGE_SIZE_US5x7, //         5 x 7 (inches)          360 x 504
        Comm10 = HPDF_PAGE_SIZE_COMM10 //    4.125 x 9.5 (inches)        297 x 684
    };

    enum PageOrientation  {
        Portrait = HPDF_PAGE_PORTRAIT, // Set the longer value to vertical.
        Landscape = HPDF_PAGE_LANDSCAPE // Set the longer value to horizontal.
    };

    enum CoordinateSystem {
        BOTTOM_LEFT,  // Default PDF coordinates
        TOP_LEFT      // Custom top-left coordinates
    };

    
    class Page {
        static std::unordered_set<std::string> fonts;
        HPDF_Doc pdf;
        HPDF_Page page;
        ClientRect pageRect;
    private:
        void fillWithColor(Rect rect, Color color) const;
        HPDF_STATUS drawText(Rect rect, HPDF_REAL deltaWidth, HPDF_REAL deltaHeight, const std::string& text, HPDF_Font font, HPDF_REAL fontSize, Color color = PDF::BLACK) const;
        HPDF_STATUS drawRectangle(Rect rect, HPDF_REAL size = 1.0, Color color = PDF::BLACK) const;
        HPDF_STATUS drawImage(Rect rect, const unsigned char* data, size_t size) const;
        HPDF_STATUS drawQR(Rect rect, const std::string& qrText, int scale, int qrBlockSize) const;

    public:
        Page(HPDF_Doc pdf, PageSizes size = PageSizes::A4, PageOrientation orientation=PageOrientation::Portrait, CoordinateSystem coordinateSystem=CoordinateSystem::TOP_LEFT);
        HPDF_Page operator()() {return page; }
        Rect getWriteableRect() const {return pageRect.getInnerRect(); }
        static bool isFontExist(const std::string &fontName) {
            return (fonts.find(fontName) != fonts.end());
        }

        HPDF_STATUS setTransform(HPDF_REAL a, HPDF_REAL b, HPDF_REAL c, HPDF_REAL d, HPDF_REAL x, HPDF_REAL y) {
            return HPDF_Page_Concat(page, a, b, c, d, x, y);
        }

        HPDF_STATUS setBottomLeftAsOrigin() {
            // Reset to default PDF coordinates (0,0 at bottom-left)
            return HPDF_Page_Concat(page, 1, 0, 0, 1, 0, 0);
        }

        HPDF_STATUS setTopLeftAsOrigin() {
            return HPDF_Page_Concat(page, 1, 0, 0, -1, 0, HPDF_Page_GetHeight(page));
        }


        HPDF_Font getFont(const std::string& font) const;

        HPDF_REAL getTextHeight() const;
        HPDF_REAL getTextHeight(const std::string& font, HPDF_REAL size) const;
        HPDF_REAL getTextHeight(HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text, const std::string& font, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text, HPDF_Font fontPtr, HPDF_REAL size) const;
        HPDF_REAL getTextWidth(const std::string& text) const {return HPDF_Page_TextWidth(page, text.c_str());}

        HPDF_REAL computeCellHeight(PDF::Cell cell) const;
        HPDF_REAL computeCellWidth(PDF::Cell cell) const;

        std::tuple<HPDF_REAL, HPDF_REAL> addCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y) const;      
        std::tuple<HPDF_REAL, HPDF_REAL> addConstrainedCell(PDF::Cell cell, HPDF_REAL x, HPDF_REAL y, HPDF_REAL boxWidth=0, HPDF_REAL boxHeight=0) const;
        std::tuple<HPDF_REAL, HPDF_REAL> addText(ClientRect rect, const std::string& text, const std::string& font, HPDF_REAL fontSize, Color textColor = PDF::BLACK, Color backgroundColor = PDF::WHITE) const;
        std::tuple<HPDF_REAL, HPDF_REAL> addText(ClientRect rect, const std::string& text, TextProperties properties, Color backgroundColor = PDF::WHITE) const;
        std::tuple<HPDF_REAL, HPDF_REAL> addText(ClientRect rect, const std::string& text, Color textColor = PDF::BLACK, Color backgroundColor = PDF::WHITE) const;

        
        // ----- function below uses drawWidget to draw with margin, border and padding.
        // 
        // these draw functions on clientRect will paint the margin, border and padding.
        HPDF_STATUS drawWidget(ClientRect outerRect, Color backgroundColor, std::function<HPDF_STATUS(Rect)> draw) const;

        // these draw text on clientRect with margin, border and padding.
        HPDF_STATUS drawText(ClientRect outerRect, const std::string& text, TextProperties prop, Color backgroundColor = PDF::WHITE) const ;
        // these draw image on clientRect with margin, border and padding.
        HPDF_STATUS drawImage(ClientRect outerRect, const unsigned char* data, size_t size, Color backgroundColor = PDF::WHITE) const {
            return drawWidget(outerRect, backgroundColor, [this, data, size](Rect innerRect) {
                return drawImage(innerRect, data, size);
            });
        }

        // these draw rectangle on clientRect with margin, border and padding.
        HPDF_STATUS drawRectangle(ClientRect outerRect, HPDF_REAL size = 1.0, Color color = PDF::BLACK, Color backgroundColor = PDF::WHITE) const {
            return drawWidget(outerRect, backgroundColor, [this, size, color](Rect innerRect) {
                return drawRectangle(innerRect, size, color);
            });
        }

        // these draw QR on clientRect with margin, border and padding.
        HPDF_STATUS drawQR(ClientRect outerRect, const std::string& text, int scale, int qrBlockSize) {
            return drawWidget(outerRect, PDF::WHITE, [this, text, scale, qrBlockSize](Rect innerRect) {
                return drawQR(innerRect, text, scale, qrBlockSize);
            });
        }
    };
}