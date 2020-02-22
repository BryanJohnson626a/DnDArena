//
// Created by Bryan on 2/21/2020.
//

#include <string>
#include "Types.h"

struct Action
{
    std::string Name;
};

struct Attack : public Action
{
    bool Ranged;
    bool Reach;
    Stat KeyAttribute;
};
