#pragma once

#include "json.hpp"
#include <string>

using json = nlohmann::json;

json fill_json_resp_by_get(const std::string &url);
