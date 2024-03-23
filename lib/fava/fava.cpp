#include "fava.h"
#include "http.h"
#include "cardputer_conf.h"

static const char *TAG = "fava";

FavaLedgerStat getFavaLedgerStat() {
    const auto &json = fill_json_resp_by_get(FAVA_DAILY_STAT_URL);

    FavaLedgerStat stat{};
    stat.creditCardBalance = json["credit_card_balance"];
    stat.cashBalance = json["cash_balance"];
    stat.xirr = json["xirr"];

    return stat;
}
