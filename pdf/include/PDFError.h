#pragma once
#include <hpdf.h>
#include <string>
#include <unordered_map>
#include <csetjmp>

namespace PDF {
    struct HPDF_STATUS_MESSAGE {
        HPDF_STATUS status;
        std::string message;
    };


    extern std::unordered_map<HPDF_STATUS, HPDF_STATUS_MESSAGE> HPDF_STATUS_MESSAGES;

    struct Error {
        HPDF_STATUS error_no;
        HPDF_STATUS detail_no;
        std::string error_message;
        std::string current_function;
        Error() : error_no(0), detail_no(0), error_message("") {}
    };

    // globals
    extern std::jmp_buf g_env;
    extern Error g_error;

    std::string GetErrorMessage(HPDF_STATUS status);
    void error_handler(HPDF_STATUS e, HPDF_STATUS d, void* user_data);
}