//
// Created by bryan on 2/24/20.
//

#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include "Types.h"

StatBlock * ParseStatBlock(std::string_view file_name);

const Action * ParseAction(std::string_view file_name);

void to_json(nlohmann::json & js, const StatBlock & stat_block);

void from_json(const nlohmann::json & js, StatBlock & stat_block);
