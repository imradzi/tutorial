#pragma once

#include <string>

struct PdfConfig {
    std::string pageSize = "A4";
    std::string marginTop = "20mm";
    std::string marginBottom = "20mm";
    std::string marginLeft = "15mm";
    std::string marginRight = "15mm";
    bool enableLocalFileAccess = true;  // needed for local images
    std::string wkhtmltopdfPath = "wkhtmltopdf";  // path to executable
};

// Thread-safe PDF generator using wkhtmltopdf process spawning
class PdfGenerator {
public:
    PdfGenerator();
    explicit PdfGenerator(const PdfConfig& config);
    ~PdfGenerator() = default;
    
    // No-op for backward compatibility (process-based approach needs no init)
    static bool initLibrary() { return true; }
    static void deinitLibrary() {}
    
    // Generate PDF from HTML content (string)
    bool generate(const std::string& htmlContent, const std::string& outputPath);
    
    // Generate PDF from HTML file
    bool generateFromFile(const std::string& htmlPath, const std::string& outputPath);
    
    // Generate PDF to memory buffer
    bool generateToBuffer(const std::string& htmlContent, std::string& outputBuffer);
    
private:
    PdfConfig config_;
    
    // Create temporary HTML file and return its path
    std::string createTempHtmlFile(const std::string& htmlContent);
    
    // Build command line arguments
    std::string buildCommand(const std::string& inputPath, const std::string& outputPath);
    
    // Execute wkhtmltopdf process
    bool executeProcess(const std::string& command);
};
