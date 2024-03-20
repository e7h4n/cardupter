#include "fava.h"
#include "http.h"
#include "string"

static const char *TAG = "fava";
const std::string LEDGER_STAT_API = "LEDGER_API";

FavaLedgerStat getFavaLedgerStat()
{
    const auto &json = fill_json_resp_by_get(LEDGER_STAT_API);

    FavaLedgerStat stat{};
    stat.creditCardBalance = json["credit_card_balance"];
    stat.cashBalance = json["cash_balance"];
    stat.xirr = json["xirr"];

    return stat;
}
