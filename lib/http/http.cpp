#include "http.h"

static const char *TAG = "native_http";

json fill_json_resp_by_get(const std::string &url)
{
    json data = {
        {"credit_card_balance", 1000.0},
        {"cash_balance", 2000.0},
        {"xirr", 0.1},
    };

    return data;
}
