//
// Created by bryan on 2/24/20.
//

#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include "Types.h"

StatBlock * ParseStatBlock(std::string_view name);

const Action * ParseAction(std::string_view name);
