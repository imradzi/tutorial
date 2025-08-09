#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <iostream>
#include <string>
#include <boost/json.hpp>
using namespace boost::json;

struct ToyyipPayTrans {
    std::string userSecretKey;
    std::string categoryCode;
    std::string billName;
    std::string billDescription;
    std::string billPriceSetting = "1";
    std::string billPayorInfo = "1";
    std::string billAmount;
    std::string billTo;
    std::string billEmail;
    std::string billPhone;
    std::string billReturnUrl;
    std::string billCallbackUrl;
    std::string billExternalReferenceNo;
    std::string billPaymentChannel;
    std::string billContentEmail;
    std::string billChargeToCustomer;
    std::string billExpiryDays;

    boost::json::value GetJSON() {
        boost::json::value jv = {
            {"userSecretKey", userSecretKey},
            {"categoryCode", categoryCode},
            {"billName", billName},
            {"billDescription", billDescription},
            {"billPriceSetting", billPriceSetting},
            {"billPayorInfo", billPayorInfo},
            {"billAmount", billAmount},
            {"billTo", billTo},
            {"billEmail", billEmail},
            {"billPhone", billPhone},
            {"billReturnUrl", billReturnUrl},
            {"billCallbackUrl", billCallbackUrl},
            {"billExternalReferenceNo", billExternalReferenceNo},
            {"billPaymentChannel", billPaymentChannel},
            {"billContentEmail", billContentEmail},
            {"billChargeToCustomer", billChargeToCustomer},
            {"billExpiryDays", billExpiryDays}
        };
        return jv;
    }

    std::string serialize() {
        return boost::json::serialize(GetJSON());
    }
};

int main() {
    ToyyipPayTrans testRec;
    std::cout << testRec.serialize();
    std::cout << "\n\n";
}
