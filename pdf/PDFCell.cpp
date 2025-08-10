#include "PDFCell.h"

namespace PDF {
    Rect ClientRect::getBorderRect() const {  // with margins - where print border rect is.
        Rect borderRect;
        auto n = rect.topLeft.x + margins[left].size;
        if (n < rect.bottomRight.x) borderRect.topLeft.x = n;

        n = rect.topLeft.y + margins[top].size;
        if (n < rect.bottomRight.y) borderRect.topLeft.y = n;

        n = rect.bottomRight.x - margins[right].size;
        if (n > rect.topLeft.x) borderRect.bottomRight.x = n;

        n = rect.bottomRight.y - margins[bottom].size;
        if (n > rect.topLeft.y) borderRect.bottomRight.y = n;

        return borderRect;
    }
    Rect ClientRect::getInnerRect() const {
        auto checkRect = getBorderRect();
        Rect innerRect;
        auto n = rect.topLeft.x + paddings[left].size + borders[left].size + margins[left].size;
        if (n < checkRect.bottomRight.x) innerRect.topLeft.x = n;

        n = rect.topLeft.y + paddings[top].size + borders[top].size + margins[top].size;
        if (n < checkRect.bottomRight.y) innerRect.topLeft.y = n;

        n = rect.bottomRight.x - paddings[right].size - borders[right].size - margins[right].size;
        if (n > checkRect.topLeft.x) innerRect.bottomRight.x = n;

        n = rect.bottomRight.y - paddings[bottom].size - borders[bottom].size - margins[bottom].size;
        if (n > checkRect.topLeft.y) innerRect.bottomRight.y = n;
        return innerRect;
    }
}