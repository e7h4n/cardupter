static const char *TAG = "esp_http";

#include "http.h"
#include "root_ca.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

void fill_json_resp_by_get(const std::string &url)
{
    WiFiClientSecure client;
    client.setCACert(ROOT_CA);

    HTTPClient https;
    https.useHTTP10(true);
    bool success = https.begin(client, url);
    if (!success)
    {
        ESP_LOGE(TAG, "Failed to connect to Fava server");
        return FavaLedgerStat{};
    }

    int code = https.GET();
    if (code != 200)
    {
        ESP_LOGE(TAG, "Failed to get Fava ledger stat: %d", code);
        return FavaLedgerStat{};
    }

    JsonDocument doc;
    deserializeJson(doc, https.getStream());
    FavaLedgerStat stat{};
    stat.creditCardBalance = doc["credit_card_balance"].as<float>();
    stat.cashBalance = doc["cash_balance"].as<float>();
    stat.xirr = doc["xirr"].as<float>();

    return stat;
}
