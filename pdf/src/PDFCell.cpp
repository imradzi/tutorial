#include "../include/PDFCell.h"
#include "../include/PDFPage.h"

namespace PDF {
    HPDF_REAL TextProperties::getTextHeight(PDF::Page* page) const { return page ? page->getTextHeight(font, fontSize) : 0.0; }

    Rect& Rect::shrinkAndMove(HPDF_REAL top, HPDF_REAL right, HPDF_REAL bottom, HPDF_REAL left) {
        if (topLeft.y + top < bottomRight.y) topLeft.y += top;
        if (topLeft.x + left < bottomRight.x) topLeft.x += left;
        if (bottomRight.y - bottom > topLeft.y) bottomRight.y -= bottom;
        if (bottomRight.x - right > topLeft.x) bottomRight.x -= right;
        return *this;
    }


    ClientRect &&ClientRect::init(ClientRect &&rect) {
        auto &borderRect = rect.borderRect;
        borderRect.shrinkAndMove(rect.margins[top].size, rect.margins[right].size, rect.margins[bottom].size, rect.margins[left].size);
        auto &paddingRect = rect.paddingRect;
        paddingRect = borderRect;
        paddingRect.shrinkAndMove(rect.borders[top].size, rect.borders[right].size, rect.borders[bottom].size, rect.borders[left].size);
        auto& innerRect = rect.innerRect;
        innerRect = paddingRect;
        innerRect.shrinkAndMove(rect.paddings[top].size, rect.paddings[right].size, rect.paddings[bottom].size, rect.paddings[left].size);
        return std::move(rect);
    }

    ClientRect& ClientRect::moveBy(Coord coord) {
        rect.moveBy(coord);
        borderRect.moveBy(coord);
        paddingRect.moveBy(coord);
        innerRect.moveBy(coord);
        return *this;
    }

    //Rect ClientRect::getBorderRect() const {  // with margins - where print border rect is.
    //    Rect borderRect {};
    //    auto n = rect.topLeft.x + margins[left].size;
    //    if (n < rect.bottomRight.x) borderRect.topLeft.x = n;

    //    n = rect.topLeft.y + margins[top].size;
    //    if (n < rect.bottomRight.y) borderRect.topLeft.y = n;

    //    n = rect.bottomRight.x - margins[right].size;
    //    if (n > rect.topLeft.x) borderRect.bottomRight.x = n;

    //    n = rect.bottomRight.y - margins[bottom].size;
    //    if (n > rect.topLeft.y) borderRect.bottomRight.y = n;

    //    return borderRect;
    //}
    //Rect ClientRect::getPaddingRect() const {  // with margins - where print padding rect is.
    //    auto checkRect = getBorderRect();
    //    Rect innerRect {};
    //    auto n = rect.topLeft.x + borders[left].size + margins[left].size;
    //    if (n < checkRect.bottomRight.x) innerRect.topLeft.x = n;

    //    n = rect.topLeft.y + borders[top].size + margins[top].size;
    //    if (n < checkRect.bottomRight.y) innerRect.topLeft.y = n;

    //    n = rect.bottomRight.x - borders[right].size - margins[right].size;
    //    if (n > checkRect.topLeft.x) innerRect.bottomRight.x = n;

    //    n = rect.bottomRight.y - borders[bottom].size - margins[bottom].size;
    //    if (n > checkRect.topLeft.y) innerRect.bottomRight.y = n;
    //    return innerRect;
    //}
    //Rect ClientRect::getInnerRect() const {
    //    auto checkRect = getBorderRect();
    //    Rect innerRect {};
    //    auto n = rect.topLeft.x + paddings[left].size + borders[left].size + margins[left].size;
    //    if (n < checkRect.bottomRight.x) innerRect.topLeft.x = n;

    //    n = rect.topLeft.y + paddings[top].size + borders[top].size + margins[top].size;
    //    if (n < checkRect.bottomRight.y) innerRect.topLeft.y = n;

    //    n = rect.bottomRight.x - paddings[right].size - borders[right].size - margins[right].size;
    //    if (n > checkRect.topLeft.x) innerRect.bottomRight.x = n;

    //    n = rect.bottomRight.y - paddings[bottom].size - borders[bottom].size - margins[bottom].size;
    //    if (n > checkRect.topLeft.y) innerRect.bottomRight.y = n;
    //    return innerRect;
    //}

    Rect& Rect::moveTo(Coord coord) {
        Size size = getSize();
        topLeft = coord;
        bottomRight = Coord {topLeft.x + size.width, topLeft.y + size.height};
        return *this;
    }
    Rect& Rect::moveBy(Coord coord) {
        topLeft.x += coord.x;
        topLeft.y += coord.y;
        bottomRight.x += coord.x;
        bottomRight.y += coord.y;
        return *this;
    }
}