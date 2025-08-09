#include <iostream>
#include <hpdf.h>
#include <csetjmp>
#include <fmt/format.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct HPDF_STATUS_MESSAGE {
    HPDF_STATUS status;
    std::string message;
};


std::string GetErrorMessage(HPDF_STATUS status) {
    extern std::unordered_map<HPDF_STATUS, HPDF_STATUS_MESSAGE> HPDF_STATUS_MESSAGES;
    return HPDF_STATUS_MESSAGES[status].message;
}

struct PDFError {
    HPDF_STATUS error_no;
    HPDF_STATUS detail_no;
    std::string error_message;
    std::string current_function;
    PDFError() : error_no(0), detail_no(0), error_message("") {}
};

// globals
std::jmp_buf g_env;
PDFError g_error;

static void error_handler(HPDF_STATUS e, HPDF_STATUS d, void* user_data) {
  auto err = static_cast<PDFError*>(user_data);
  err->error_no = e;
  err->detail_no = d;
  err->error_message = fmt::format("{} -> ERROR: error_no={:04X}, detail_no={:04X}, message={}", err->current_function, (int64_t)e, (int64_t)d, GetErrorMessage(e) + " " + GetErrorMessage(d));
  longjmp(g_env, 1); // jump to a live frame
}

class PDFFont {
    HPDF_Doc pdf;
    HPDF_Font font;
public:
    PDFFont(HPDF_Doc pdf, const std::string& name, const char* encoding = nullptr) : pdf(pdf), font(HPDF_GetFont(pdf, name.c_str(), encoding)) {
        font = HPDF_GetFont(pdf, name.c_str(), encoding);
        if (!font) {
            throw std::runtime_error(fmt::format("Error getting font: {}", name));
        }
    }
    PDFFont(HPDF_Doc pdf, HPDF_Font font) : pdf(pdf), font(font) {}
    auto getFontName() { return HPDF_Font_GetFontName(font); }
    auto getEncodingName() { return HPDF_Font_GetEncodingName(font); }
    auto getUnicodeWidth(wchar_t w) { return HPDF_Font_GetUnicodeWidth(font, w); }
    auto getBBox() { return HPDF_Font_GetBBox(font); }
    auto getAscent() { return HPDF_Font_GetAscent(font); }
    auto getDescent() { return HPDF_Font_GetDescent(font); }
    auto getXHeight() { return HPDF_Font_GetXHeight(font); }
    auto getCapHeight() { return HPDF_Font_GetCapHeight(font); }
    auto textWidth(const std::string& text) { return HPDF_Font_TextWidth(font, reinterpret_cast<const HPDF_BYTE*>(text.c_str()), text.size()); }
    std::tuple<HPDF_REAL, HPDF_REAL> measureText(const std::string& text, HPDF_REAL width, HPDF_REAL font_size, HPDF_BOOL wordwrap=false, HPDF_REAL char_space=1, HPDF_REAL word_space=1) {
        HPDF_REAL real_width;
        auto w = HPDF_Font_MeasureText(font, reinterpret_cast<const HPDF_BYTE*>(text.c_str()), text.size(), width, font_size, char_space, word_space, wordwrap, &real_width);
        return {w, real_width};
    }
};

struct PDFColor {
    HPDF_REAL r;
    HPDF_REAL g;
    HPDF_REAL b;
};

auto constexpr PDF_BLACK = PDFColor{.r = 0, .g = 0, .b =  0};
auto constexpr PDF_WHITE = PDFColor{.r = 1, .g = 1, .b =  1};
auto constexpr PDF_RED = PDFColor{.r = 1, .g = 0, .b =  0};
auto constexpr PDF_GREEN = PDFColor{.r = 0, .g = 1, .b =  0};
auto constexpr PDF_BLUE = PDFColor{.r = 0, .g = 0, .b =  1};
auto constexpr PDF_YELLOW = PDFColor{.r = 1, .g = 1, .b =  0};
auto constexpr PDF_CYAN = PDFColor{.r = 0, .g = 1, .b =  1};
auto constexpr PDF_MAGENTA = PDFColor{.r = 1, .g = 0, .b =  1};
auto constexpr PDF_ORANGE = PDFColor{.r = 1, .g = 0.5, .b =  0};
auto constexpr PDF_PINK = PDFColor{.r = 1, .g = 0.75, .b =  0.75};
auto constexpr PDF_PURPLE = PDFColor{.r = 0.5, .g = 0, .b =  0.5};
auto constexpr PDF_GRAY = PDFColor{.r = 0.5, .g = 0.5, .b =  0.5};
auto constexpr PDF_BROWN = PDFColor{.r = 0.5, .g = 0.25, .b =  0};
auto constexpr PDF_LIME = PDFColor{.r = 0.75, .g = 1, .b =  0};
auto constexpr PDF_TEAL = PDFColor{.r = 0, .g = 0.5, .b =  0.5};
auto constexpr PDF_INDIGO = PDFColor{.r = 0.5, .g = 0, .b =  0.5};
auto constexpr PDF_VIOLET = PDFColor{.r = 0.5, .g = 0, .b =  1};

auto constexpr PDF_LIGHT_GRAY = PDFColor{.r = 0.95, .g = 0.95, .b =  0.95};
auto constexpr PDF_LIGHT_BLUE = PDFColor{.r = 0.75, .g = 0.75, .b =  1};
auto constexpr PDF_LIGHT_GREEN = PDFColor{.r = 0.75, .g = 1, .b =  0.75};
auto constexpr PDF_LIGHT_RED = PDFColor{.r = 1, .g = 0.75, .b =  0.75};
auto constexpr PDF_LIGHT_YELLOW = PDFColor{.r = 1, .g = 1, .b =  0.75};
auto constexpr PDF_LIGHT_CYAN = PDFColor{.r = 0.75, .g = 1, .b =  1};
auto constexpr PDF_LIGHT_MAGENTA = PDFColor{.r = 1, .g = 0.75, .b =  1};
auto constexpr PDF_LIGHT_PURPLE = PDFColor{.r = 0.75, .g = 0.75, .b =  1};

auto constexpr PDF_DARK_GRAY = PDFColor{.r = 0.25, .g = 0.25, .b =  0.25};
auto constexpr PDF_DARK_BLUE = PDFColor{.r = 0, .g = 0, .b =  0.5};
auto constexpr PDF_DARK_GREEN = PDFColor{.r = 0, .g = 0.5, .b =  0};
auto constexpr PDF_DARK_RED = PDFColor{.r = 0.5, .g = 0, .b =  0};
auto constexpr PDF_DARK_YELLOW = PDFColor{.r = 0.5, .g = 0.5, .b =  0};
auto constexpr PDF_DARK_CYAN = PDFColor{.r = 0, .g = 0.5, .b =  0.5};
auto constexpr PDF_DARK_MAGENTA = PDFColor{.r = 0.5, .g = 0, .b =  0.5};

enum PDFJustification {
    left,
    right,
    center,
    top,
    bottom
};

struct PDFCell {
    HPDF_REAL border_thickness = 1.0;
    HPDF_REAL padding = 5.0;
    HPDF_REAL margin=0.0;
    HPDF_REAL width=0;
    HPDF_REAL height=0;
    PDFColor border_color = PDF_BLACK;
    std::string text;
    HPDF_Font font;
    HPDF_REAL fontSize;
    PDFColor background_color = PDF_LIGHT_GRAY;
    PDFColor text_color = PDF_BLACK;
    PDFJustification horizontal_justify = PDFJustification::left;
    PDFJustification vertical_justify = PDFJustification::center;
};

class PDFPage {
    static std::unordered_set<std::string> fonts;
    HPDF_Doc pdf;
    HPDF_Page page;
    HPDF_REAL h;
    HPDF_REAL w;
    HPDF_REAL border_width = 1.0;
    HPDF_REAL border_padding = 5.0;
    PDFColor border_color = PDFColor(0, 0, 0);
public:
    PDFPage(HPDF_Doc pdf) : pdf(pdf), page(HPDF_AddPage(pdf)) {
        g_error.current_function = "PDFPage::PDFPage";
        if (!page) {
            throw std::runtime_error("Error adding page");
        }
        h = HPDF_Page_GetHeight(page);
        w = HPDF_Page_GetWidth(page);
        HPDF_Page_Concat(page, 1, 0, 0, -1, 0, h); // 1) Make (0,0) be top-left for all drawing
    }
    HPDF_Page operator()() {return page; }
    HPDF_REAL height() {return h; }
    HPDF_REAL width() {return w; }

    static bool isFontExist(const std::string &fontName) {
        if (fonts.find(fontName) == fonts.end()) false;
        return true;
    }

    void setBorderWidth(HPDF_REAL width) {border_width = width; }
    void setBorderColor(const PDFColor &c) {border_color = c; }
    void setBorderPadding(HPDF_REAL padding) {border_padding = padding; }
    HPDF_REAL getTextWidth(const std::string& text, const std::string& font, int size) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        return HPDF_Page_TextWidth(page, text.c_str());
    }
    HPDF_REAL getTextWidth(const std::string& text, HPDF_Font fontPtr, HPDF_REAL size) const {
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        return HPDF_Page_TextWidth(page, text.c_str());
    }

    HPDF_Font getFont(const std::string& font) const {
        if (isFontExist(font)) {
            return HPDF_GetFont(pdf, font.c_str(), nullptr);
        } 
        return HPDF_Page_GetCurrentFont(page);
    }

    HPDF_REAL getTextHeight(const std::string& font, int size) const {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }

    HPDF_REAL getTextHeight() const {
        auto fontPtr = HPDF_Page_GetCurrentFont(page);
        auto size = HPDF_Page_GetCurrentFontSize(page);
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }
    HPDF_REAL getTextHeight(HPDF_Font fontPtr, HPDF_REAL size) const {
        HPDF_REAL ascent = HPDF_Font_GetAscent(fontPtr);
        HPDF_REAL descent = HPDF_Font_GetDescent(fontPtr); // Note: This is negative!
        return (ascent - descent) * size / 1000.0;
    }

    HPDF_REAL getTextWidth(const std::string& text) const {return HPDF_Page_TextWidth(page, text.c_str());}

    HPDF_REAL getCellHeight(PDFCell cell) {
        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto height = cell.height;
        if (height <= 0) {
            height = textHeight;
        }
        return height + 2*cell.padding + 2*cell.border_thickness;
    }
    HPDF_REAL getCellWidth(PDFCell cell) {
        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);
        auto width = cell.width;
        if (width <= 0) {
            width = textWidth;
        }
        return width + 2*cell.padding + 2*cell.border_thickness;
    }

    std::tuple<HPDF_REAL, HPDF_REAL> addCell(PDFCell cell, HPDF_REAL x, HPDF_REAL y) {

        auto fontPtr = cell.font == nullptr ? HPDF_Page_GetCurrentFont(page) : cell.font;
        auto fontSize = cell.fontSize <= 0 ? HPDF_Page_GetCurrentFontSize(page) : cell.fontSize;
        HPDF_Page_SetFontAndSize(page, fontPtr, fontSize);
        auto textHeight = getTextHeight(fontPtr, fontSize);
        auto textWidth = getTextWidth(cell.text, fontPtr, fontSize);

        auto width = cell.width;
        HPDF_REAL balanceWidth = 0;
        HPDF_REAL deltaWidth = 0;
        if (width <= 0) {
            width = getTextWidth(cell.text);
        } else {
            balanceWidth = width - textWidth;
            if (cell.horizontal_justify == PDFJustification::center) {
                deltaWidth = balanceWidth / 2;
            } else if (cell.horizontal_justify == PDFJustification::right) {
                deltaWidth = balanceWidth;
            }
            if (deltaWidth < 0) deltaWidth = 0;
        }

        width += 2*cell.padding + 2*cell.margin;
        
        auto height = cell.height;
        HPDF_REAL balanceHeight = 0;
        HPDF_REAL deltaHeight = 0;
        if (height <= 0) {
            height = textHeight;
        } else {
            balanceHeight = height - textHeight;
            if (cell.vertical_justify == PDFJustification::center) {
                deltaHeight = balanceHeight / 2;
            } else if (cell.vertical_justify == PDFJustification::top) {
                deltaHeight = balanceHeight;
            }
            if (deltaHeight < 0) deltaHeight = 0;
        }
        height += 2*cell.padding + 2*cell.margin;

        if (cell.border_thickness > 0) {
            HPDF_Page_SetLineWidth(page, cell.border_thickness);
            HPDF_Page_SetRGBStroke(page, cell.border_color.r, cell.border_color.g, cell.border_color.b);
            HPDF_Page_Rectangle(page, x+cell.margin, y-height+cell.margin, width-cell.margin, height-cell.margin-cell.margin);
            HPDF_Page_Stroke(page);
        }

        HPDF_Page_SetRGBFill(page, cell.background_color.r, cell.background_color.g, cell.background_color.b);
        HPDF_Page_Rectangle(page, x+cell.margin, y-height+cell.margin, width-cell.margin, height-cell.margin-cell.margin);
        HPDF_Page_Fill(page);
        
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, cell.text_color.r, cell.text_color.g, cell.text_color.b);
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);
        
       
        PDFFont f(pdf, fontPtr);
        auto [measuredWidth, actualWidth] = f.measureText(cell.text, width, fontSize);
        auto text = cell.text;
        if (measuredWidth < actualWidth) {
            text = cell.text.substr(0, measuredWidth-3);
            text += "...";
        }
        HPDF_Page_TextOut(page, x+cell.padding+cell.margin+deltaWidth, y-cell.padding-cell.margin-deltaHeight, text.c_str());
        HPDF_Page_EndText(page);

        return {width + cell.border_thickness, height + cell.border_thickness};
        
    }

    HPDF_REAL addText(const std::string& text, const std::string& font, int size, int x, int y, bool isBordered = false, int height = 0) {
        if (!isFontExist(font)) {
            throw std::runtime_error(fmt::format("Error getting font: {}", font));
        }
        HPDF_Page_BeginText(page);
        auto fontPtr = HPDF_GetFont(pdf, font.c_str(), nullptr);
        HPDF_Page_SetFontAndSize(page, fontPtr, size);
        auto textHeight = getTextHeight(fontPtr, size);
        if (height == 0) {
            height = textHeight;
        }
        HPDF_Page_SetTextMatrix(page, 1, 0, 0, -1, 0, h);
        HPDF_Page_TextOut(page, x, y, text.c_str());
        HPDF_Page_EndText(page);
        if (isBordered) {
            HPDF_Page_SetLineWidth(page, border_width);
            HPDF_Page_SetRGBStroke(page, border_color.r, border_color.g, border_color.b);
            HPDF_Page_Rectangle(page, x-border_padding, y-height-border_padding, getTextWidth(text, font, size)+2*border_padding, height+2*border_padding);
            HPDF_Page_Stroke(page);
        }
        return height;
    }
};

class PDFWriter {
    HPDF_Doc pdf;
    std::string filename;
    std::string error_message;
public:
    PDFWriter(const std::string& filename) : filename(filename) {
        g_error.current_function = "PDFWriter::PDFWriter";
        pdf = HPDF_New(error_handler, &g_error);
        if (!pdf) {
            throw std::runtime_error("Error creating PDF");
        }
    }

    ~PDFWriter() {
        if (pdf) HPDF_SaveToFile(pdf, filename.c_str());
        HPDF_Free(pdf);
    }
    void enableCompression() {
        g_error.current_function = "PDFWriter::enableCompression";
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
    }
    HPDF_Font getFont(const std::string &fontName) {
        if (PDFPage::isFontExist(fontName)) return HPDF_GetFont(pdf, fontName.c_str(), nullptr);
        return nullptr;
    }
    void write();
    auto addPage() { return PDFPage(pdf); }
    void addImage(const std::string& imagePath, int x, int y, int width, int height);
    void addLine(int x1, int y1, int x2, int y2);
    void addRectangle(int x, int y, int width, int height);
    void addCircle(int x, int y, int radius);
};


int main() {
    if (setjmp(g_env)) {
        std::cout << "Jump: " << g_error.error_message << std::endl;
        return 1;
    }

    try {
        PDFWriter pdf("hello.pdf");
        pdf.enableCompression();             // after setjmp is active
        auto page = pdf.addPage();
        page.setBorderWidth(1.5);
        page.setBorderColor(PDF_LIGHT_GRAY);
        int x=50, y=50;
        auto constexpr spacing = 20;
        y += page.addText("Hello, World!", "Helvetica", 40, x, y, true) + spacing;
        y += page.addText("Wow! This is a Courier test!", "Courier", 18, x, y, true) + spacing;
        auto yy= y - page.getTextHeight("Times-Roman", 30);
        if (yy < y) y = yy+spacing; // move down if overlapped
        y += page.addText("Wow! This is a Times-Roman test!", "Times-Roman", 30, x, y, true) + spacing;
        page.addText("Wow! This is a Symbol test!", "Symbol", 18, x, y,  true, 20);
        y+= 20 +spacing;
        page.addText("Wow! This is a ZapfDingbats test!", "ZapfDingbats", 18, x, y, true, 20);
        y+= 20 +spacing;

        auto font = page.getFont("Courier");
        
        std::vector<PDFCell> cells = {
            {.width=200,.text="Line 000", .font=font, .fontSize=20},
            {.width=200,.text="Line 001", .font=font, .fontSize=20},
            {.width=200,.text="Line 002, fontsize=10", .font=font, .fontSize=10},
            {.padding=10, .width=0,.text="Line 003, fontsize=40", .font=font, .fontSize=40},
            {.width=200,.text="Line 004", .font=font, .fontSize=20}
        };

        std::vector<PDFCell> hcells = {
            {.border_thickness=0.5, .width=0, .height=3, .text="Horz Line 000 font=5", .font=font, .fontSize=5, .background_color=PDF_LIGHT_BLUE, .text_color=PDF_BLACK, .horizontal_justify=PDFJustification::center},
            {.border_thickness=0.8, .width=30, .height=200, .text="Horz Line 001", .font=font, .fontSize=10, .background_color=PDF_LIGHT_GREEN, .text_color=PDF_BLACK, .vertical_justify=PDFJustification::center},
            {.border_thickness=0.5, .text="Horz Line 002, fontsize=5", .font=font, .fontSize=5, .background_color=PDF_LIGHT_RED, .text_color=PDF_BLACK},
            {.border_thickness=0.5, .padding=10, .text="Horiz Line 003, fontsize=10", .font=font, .fontSize=10, .background_color=PDF_LIGHT_YELLOW, .text_color=PDF_BLACK},
            {.border_thickness=0.5, .padding=20, .text="Horiz Line 004", .font=font, .fontSize=10, .background_color=PDF_LIGHT_MAGENTA, .text_color=PDF_BLACK}
        };


        for (const auto &c: cells) {
            auto thisCellHeight = page.getCellHeight(c);
            auto [w, h] = page.addCell(c, x, y+thisCellHeight);
            y += h;
        }

        x=10;
        y += 50;
        for (const auto &c: hcells) {
            auto thisCellWidth = page.getCellWidth(c);
            auto [w, h] = page.addCell(c, x, y);
            x += w-c.border_thickness;

        }

    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

std::unordered_set<std::string> PDFPage::fonts = {
    "Courier",
    "Courier-Bold",
    "Courier-Oblique",
    "Courier-BoldOblique",
    "Helvetica",
    "Helvetica-Bold",
    "Helvetica-Oblique",
    "Helvetica-BoldOblique",
    "Times-Roman",
    "Times-Bold",
    "Times-Italic",
    "Times-BoldItalic",
    "Symbol",
    "ZapfDingbats"
};

std::unordered_map<HPDF_STATUS, HPDF_STATUS_MESSAGE> HPDF_STATUS_MESSAGES = {   
    {HPDF_ARRAY_COUNT_ERR, {0x1001, "Internal error. Data consistency was lost."}},
    {HPDF_ARRAY_ITEM_NOT_FOUND, {0x1002, "Internal error. Data consistency was lost."}},
    {HPDF_ARRAY_ITEM_UNEXPECTED_TYPE, {0x1003, "Internal error. Data consistency was lost."}},
    {HPDF_BINARY_LENGTH_ERR, {0x1004, "Data length > HPDF_LIMIT_MAX_STRING_LEN."}},
    {HPDF_CANNOT_GET_PALLET, {0x1005, "Cannot get pallet data from PNG image."}},
    {0x1006, {0x1006, "Not defined"}},
    {HPDF_DICT_COUNT_ERR, {0x1007, "Dictionary elements > HPDF_LIMIT_MAX_DICT_ELEMENT"}},
    {HPDF_DICT_ITEM_NOT_FOUND, {0x1008, "Internal error. Data consistency was lost."}},
    {HPDF_DICT_ITEM_UNEXPECTED_TYPE, {0x1009, "Internal error. Data consistency was lost."}},
    {HPDF_DICT_STREAM_LENGTH_NOT_FOUND, {0x100A, "Internal error. Data consistency was lost."}},
    {HPDF_DOC_ENCRYPTDICT_NOT_FOUND, {0x100B, "HPDF_SetEncryptMode() or HPDF_SetPermission() called before password set."}},
    {HPDF_DOC_INVALID_OBJECT, {0x100C, "Internal error. Data consistency was lost."}},
    {0x100D, {0x100D, "Not defined"}},
    {HPDF_DUPLICATE_REGISTRATION, {0x100E, "Tried to re-register a registered font."}},
    {HPDF_EXCEED_JWW_CODE_NUM_LIMIT, {0x100F, "Cannot register a character to the Japanese word wrap characters list."}},
    {0x1010, {0x1010, "Not defined"}},
    {HPDF_ENCRYPT_INVALID_PASSWORD, {0x1011, "1. Tried to set the owner password to NULL. 2. Owner and user password are the same."}},
    {0x1012, {0x1012, "Not defined"}},  
    {HPDF_ERR_UNKNOWN_CLASS, {0x1013, "Internal error. Data consistency was lost."}},
    {HPDF_EXCEED_GSTATE_LIMIT, {0x1014, "Stack depth > HPDF_LIMIT_MAX_GSTATE."}},
    {HPDF_FAILD_TO_ALLOC_MEM, {0x1015, "Memory allocation failed."}},
    {HPDF_FILE_IO_ERROR, {0x1016, "File processing failed. (Detailed code is set.)"}},
    {HPDF_FILE_OPEN_ERROR, {0x1017, "Cannot open a file. (Detailed code is set.)"}},
    {0x1018, {0x1018, "Not defined"}},
    {HPDF_FONT_EXISTS, {0x1019, "Tried to load a font that has been registered."}},
    {HPDF_FONT_INVALID_WIDTHS_TABLE, {0x101A, "1. Font-file format is invalid. 2. Internal error. Data consistency was lost."}},
    {HPDF_INVALID_AFM_HEADER, {0x101B, "Cannot recognize header of afm file."}},
    {HPDF_INVALID_ANNOTATION, {0x101C, "Specified annotation handle is invalid."}},
    {0x101D, {0x101D, "Not defined"}},
    {HPDF_INVALID_BIT_PER_COMPONENT, {0x101E, "Bit-per-component of a image which was set as mask-image is invalid."}},
    {HPDF_INVALID_CHAR_MATRICS_DATA, {0x101F, "Cannot recognize char-matrics-data of afm file."}},
    {HPDF_INVALID_COLOR_SPACE, {0x1020, "1. Invalid color_space parameter of HPDF_LoadRawImage. 2. Color-space of a image which was set as mask-image is invalid. 3. Invoked function invalid in present color-space."}},
    {HPDF_INVALID_COMPRESSION_MODE, {0x1021, "Invalid value set when invoking HPDF_SetCommpressionMode()."}},
    {HPDF_INVALID_DATE_TIME, {0x1022, "An invalid date-time value was set."}},
    {HPDF_INVALID_DESTINATION, {0x1023, "An invalid destination handle was set."}},
    {0x1024, {0x1024, "Not defined"}},
    {HPDF_INVALID_DOCUMENT, {0x1025, "An invalid document handle was set."}},
    {HPDF_INVALID_DOCUMENT_STATE, {0x1026, "Function invalid in the present state was invoked."}},
    {HPDF_INVALID_ENCODER, {0x1027, "An invalid encoder handle was set."}},
    {HPDF_INVALID_ENCODER_TYPE, {0x1028, "Combination between font and encoder is wrong."}},
    {0x1029, {0x1029, "Not defined"}},
    {0x102A, {0x102A, "Not defined"}},
    {HPDF_INVALID_ENCODING_NAME, {0x102B, "An Invalid encoding name is specified."}},
    {HPDF_INVALID_ENCRYPT_KEY_LEN, {0x102C, "Encryption key length is invalid."}},
    {HPDF_INVALID_FONTDEF_DATA, {0x102D, "1. An invalid font handle was set. 2. Unsupported font format."}},
    {HPDF_INVALID_FONTDEF_TYPE, {0x102E, "Internal error. Data consistency was lost."}},
    {HPDF_INVALID_FONT_NAME, {0x102F, "Font with the specified name is not found."}},
    {HPDF_INVALID_IMAGE, {0x1030, "Unsupported image format."}},
    {HPDF_INVALID_JPEG_DATA, {0x1031, "Unsupported image format."}},
    {HPDF_INVALID_N_DATA, {0x1032, "Cannot read a postscript-name from an afm file."}},
    {HPDF_INVALID_OBJECT, {0x1033, "1. An invalid object is set. 2. Internal error. Data consistency was lost."}},
    {HPDF_INVALID_OBJ_ID, {0x1034, "Internal error. Data consistency was lost."}},
    {HPDF_INVALID_OPERATION, {0x1035, "Invoked HPDF_Image_SetColorMask() against the image-object which was set a mask-image."}},
    {HPDF_INVALID_OUTLINE, {0x1036, "An invalid outline-handle was specified."}},
    {HPDF_INVALID_PAGE, {0x1037, "An invalid page-handle was specified."}},
    {HPDF_INVALID_PAGES, {0x1038, "An invalid pages-handle was specified. (internal error)"}},
    {HPDF_INVALID_PARAMETER, {0x1039, "An invalid value is set."}},
    {0x103A, {0x103A, "Not defined"}},
    {HPDF_INVALID_PNG_IMAGE, {0x103B, "Invalid PNG image format."}},
    {HPDF_INVALID_STREAM, {0x103C, "Internal error. Data consistency was lost."}},
    {HPDF_MISSING_FILE_NAME_ENTRY, {0x103D, "Internal error. \"_FILE_NAME\" entry for delayed loading is missing."}},
    {0x103E, {0x103E, "Not defined"}},
    {HPDF_INVALID_TTC_FILE, {0x103F, "Invalid .TTC file format."}},
    {HPDF_INVALID_TTC_INDEX, {0x1040, "Index parameter > number of included fonts."}},
    {HPDF_INVALID_WX_DATA, {0x1041, "Cannot read a width-data from an afm file."}},
    {HPDF_ITEM_NOT_FOUND, {0x1042, "Internal error. Data consistency was lost."}},
    {HPDF_LIBPNG_ERROR, {0x1043, "Error returned from PNGLIB while loading image."}},
    {HPDF_NAME_INVALID_VALUE, {0x1044, "Internal error. Data consistency was lost."}},
    {HPDF_NAME_OUT_OF_RANGE, {0x1045, "Internal error. Data consistency was lost."}},
    {0x1046, {0x1046, "Not defined"}},  
    {0x1047, {0x1047, "Not defined"}},
    {0x1048, {0x1048, "Not defined"}},
    {HPDF_PAGES_MISSING_KIDS_ENTRY, {0x1049, "Internal error. Data consistency was lost."}},
    {HPDF_PAGE_CANNOT_FIND_OBJECT, {0x104A, "Internal error. Data consistency was lost."}},
    {HPDF_PAGE_CANNOT_GET_ROOT_PAGES, {0x104B, "Internal error. Data consistency was lost."}},
    {HPDF_PAGE_CANNOT_RESTORE_GSTATE, {0x104C, "There are no graphics-states to be restored."}},
    {HPDF_PAGE_CANNOT_SET_PARENT, {0x104D, "Internal error. Data consistency was lost."}},
    {HPDF_PAGE_FONT_NOT_FOUND, {0x104E, "The current font is not set."}},
    {HPDF_PAGE_INVALID_FONT, {0x104F, "An invalid font-handle was specified."}},
    {HPDF_PAGE_INVALID_FONT_SIZE, {0x1050, "An invalid font-size was set."}},
    {HPDF_PAGE_INVALID_GMODE, {0x1051, "See Graphics mode."}},
    {HPDF_PAGE_INVALID_INDEX, {0x1052, "Internal error. Data consistency was lost."}},
    {HPDF_PAGE_INVALID_ROTATE_VALUE, {0x1053, "Specified value is not multiple of 90."}},
    {HPDF_PAGE_INVALID_SIZE, {0x1054, "An invalid page-size was set."}},
    {HPDF_PAGE_INVALID_XOBJECT, {0x1055, "An invalid image-handle was set."}},
    {HPDF_PAGE_OUT_OF_RANGE, {0x1056, "The specified value is out of range."}},
    {HPDF_REAL_OUT_OF_RANGE, {0x1057, "The specified value is out of range."}},
    {HPDF_STREAM_EOF, {0x1058, "Unexpected EOF marker was detected."}},
    {HPDF_STREAM_READLN_CONTINUE, {0x1059, "Internal error. Data consistency was lost."}},
    {0x105A, {0x105A, "Not defined"}},
    {HPDF_STRING_OUT_OF_RANGE, {0x105B, "The length of the text is too long."}},
    {HPDF_THIS_FUNC_WAS_SKIPPED, {0x105C, "Function not executed because of other errors."}},
    {HPDF_TTF_CANNOT_EMBEDDING_FONT, {0x105D, "Font cannot be embedded. (license restriction)"}},
    {HPDF_TTF_INVALID_CMAP, {0x105E, "Unsupported ttf format. (cannot find unicode cmap)"}},
    {HPDF_TTF_INVALID_FOMAT, {0x105F, "Unsupported ttf format."}},
    {HPDF_TTF_MISSING_TABLE, {0x1060, "Unsupported ttf format. (cannot find a necessary table)"}},
    {HPDF_UNSUPPORTED_FONT_TYPE, {0x1061, "Internal error. Data consistency was lost."}},
    {HPDF_UNSUPPORTED_FUNC, {0x1062, "1. Library not configured to use PNGLIB."}},
    {HPDF_UNSUPPORTED_JPEG_FORMAT, {0x1063, "Unsupported JPEG format."}},
    {HPDF_UNSUPPORTED_TYPE1_FONT, {0x1064, "Failed to parse .PFB file."}},
    {HPDF_XREF_COUNT_ERR, {0x1065, "Internal error. Data consistency was lost."}},
    {HPDF_ZLIB_ERROR, {0x1066, "Error while executing ZLIB function."}},
    {HPDF_INVALID_PAGE_INDEX, {0x1067, "An invalid page index was passed."}},
    {HPDF_INVALID_URI, {0x1068, "An invalid URI was set."}},
    {0x1069, {0x1069, "An invalid page-layout was set."}},
    {0x106A, {0x106A, "Not defined"}},
    {0x106B, {0x106B, "Not defined"}},
    {0x106C, {0x106C, "Not defined"}},
    {0x106D, {0x106D, "Not defined"}},
    {0x106E, {0x106E, "Not defined"}},
    {0x106F, {0x106F, "Not defined"}},
    {0x1070, {0x1070, "An invalid page-mode was set."}},
    {0x1071, {0x1071, "An invalid page-num-style was set."}},
    {HPDF_ANNOT_INVALID_ICON, {0x1072, "An invalid icon was set."}},
    {HPDF_ANNOT_INVALID_BORDER_STYLE, {0x1073, "An invalid border-style was set."}},
    {HPDF_PAGE_INVALID_DIRECTION, {0x1074, "An invalid page-direction was set."}},
    {HPDF_INVALID_FONT, {0x1075, "An i  nvalid font-handle was specified."}}
};