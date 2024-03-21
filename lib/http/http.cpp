#include "http.h"

static const char *TAG = "http";

#ifdef ARDUINO

#include "root_ca.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

class WifiClientIterator {
private:
    WiFiClient *client;
    char chr;

public:
    using difference_type = std::size_t;
    using value_type = char;
    using pointer = const char *;
    using reference = const char &;
    using iterator_category = std::input_iterator_tag;

    explicit WifiClientIterator(WiFiClient *_client, bool end) : client(_client) {
        if (end) {
            chr = -1;
        } else {
            chr = (char) client->read();
        }
    }

    WifiClientIterator &operator++() {
        chr = (char) client->read();
        return *this;
    }

    reference operator*() const {
        return chr;
    }

    bool operator!=(const WifiClientIterator &rhs) const {
        return chr != *rhs;
    }
};


json fill_json_resp_by_get(const std::string &url) {
    WiFiClientSecure client;
    client.setCACert(ROOT_CA);

    HTTPClient https;
    https.useHTTP10(true);
    bool success = https.begin(client, url.c_str());
    if (!success) {
        ESP_LOGE(TAG, "Failed to connect to server");
        return json{};
    }

    int code = https.GET();
    if (code != 200) {
        ESP_LOGE(TAG, "Failed to get response with error code %d", code);
        return json{};
    }


    WifiClientIterator begin{&client, false};
    WifiClientIterator end{&client, true};
    return json::parse(begin, end);
}

#else

json fill_json_resp_by_get(const std::string &url)
{
    printf("url: %s\n", url.c_str());
    json data = {
        {"credit_card_balance", 1000.0},
        {"cash_balance", 2000.0},
        {"xirr", 0.1},
    };

    return data;
}
#endif