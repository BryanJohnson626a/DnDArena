//
// Created by bryan on 2/23/20.
//

#pragma once

#include <ostream>
#include "Types.h"

const int OUTPUT_LEVEL = 1;

std::ostream& operator<<(std::ostream & out, const ActorQueue & actors);
std::ostream& operator<<(std::ostream & out, const Arena & arena);
std::ostream& operator<<(std::ostream & out, const Group & group);
std::ostream& operator<<(std::ostream & out, const Groups & groups);
std::ostream& operator<<(std::ostream & out, const Actor & actor);
std::ostream& operator<<(std::ostream & out, const Action & action);
