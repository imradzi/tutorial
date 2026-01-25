#include "pdf_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <array>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

PdfGenerator::PdfGenerator() : config_() {}

PdfGenerator::PdfGenerator(const PdfConfig& config) : config_(config) {}

std::string PdfGenerator::createTempHtmlFile(const std::string& htmlContent) {
    // Generate unique temp filename
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    fs::path tempDir = fs::temp_directory_path();
    std::string tempFile = (tempDir / ("pdf_gen_" + std::to_string(dis(gen)) + ".html")).string();
    
    std::ofstream ofs(tempFile);
    if (!ofs) {
        std::cerr << "Failed to create temp file: " << tempFile << std::endl;
        return "";
    }
    ofs << htmlContent;
    ofs.close();
    
    return tempFile;
}

std::string PdfGenerator::buildCommand(const std::string& inputPath, const std::string& outputPath) {
    std::ostringstream cmd;
    
    cmd << config_.wkhtmltopdfPath
        << " --quiet"
        << " --page-size " << config_.pageSize
        << " --margin-top " << config_.marginTop
        << " --margin-bottom " << config_.marginBottom
        << " --margin-left " << config_.marginLeft
        << " --margin-right " << config_.marginRight;
    
    if (config_.enableLocalFileAccess) {
        cmd << " --enable-local-file-access";
    }
    
    cmd << " \"" << inputPath << "\" \"" << outputPath << "\"";
    
    return cmd.str();
}

bool PdfGenerator::executeProcess(const std::string& command) {
    int result = std::system(command.c_str());
    return (result == 0);
}

bool PdfGenerator::generate(const std::string& htmlContent, const std::string& outputPath) {
    // Create temp HTML file
    std::string tempHtml = createTempHtmlFile(htmlContent);
    if (tempHtml.empty()) {
        return false;
    }
    
    // Build and execute command
    std::string cmd = buildCommand(tempHtml, outputPath);
    bool success = executeProcess(cmd);
    
    // Cleanup temp file
    std::remove(tempHtml.c_str());
    
    if (success) {
        std::cout << "PDF generated: " << outputPath << std::endl;
    } else {
        std::cerr << "PDF conversion failed" << std::endl;
    }
    
    return success;
}

bool PdfGenerator::generateFromFile(const std::string& htmlPath, const std::string& outputPath) {
    // Check if file exists
    if (!fs::exists(htmlPath)) {
        std::cerr << "Failed to open file: " << htmlPath << std::endl;
        return false;
    }
    
    // Build and execute command directly with the HTML file
    std::string cmd = buildCommand(htmlPath, outputPath);
    bool success = executeProcess(cmd);
    
    if (success) {
        std::cout << "PDF generated: " << outputPath << std::endl;
    } else {
        std::cerr << "PDF conversion failed" << std::endl;
    }
    
    return success;
}

bool PdfGenerator::generateToBuffer(const std::string& htmlContent, std::string& outputBuffer) {
    // Create temp HTML file
    std::string tempHtml = createTempHtmlFile(htmlContent);
    if (tempHtml.empty()) {
        return false;
    }
    
    // Create temp output file
    fs::path tempDir = fs::temp_directory_path();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    std::string tempPdf = (tempDir / ("pdf_gen_" + std::to_string(dis(gen)) + ".pdf")).string();
    
    // Build and execute command
    std::string cmd = buildCommand(tempHtml, tempPdf);
    bool success = executeProcess(cmd);
    
    // Read PDF into buffer if successful
    if (success) {
        std::ifstream ifs(tempPdf, std::ios::binary);
        if (ifs) {
            std::ostringstream oss;
            oss << ifs.rdbuf();
            outputBuffer = oss.str();
        } else {
            success = false;
        }
    }
    
    // Cleanup temp files
    std::remove(tempHtml.c_str());
    std::remove(tempPdf.c_str());
    
    if (!success) {
        std::cerr << "PDF conversion failed" << std::endl;
    }
    
    return success;
}
