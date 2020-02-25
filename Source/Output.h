//
// Created by bryan on 2/23/20.
//

#pragma once

#include <iostream>
#include "Types.h"

// Output levels. Each entry includes all levels above it.
// 0: No output.
// Errors: Vital errors
// Warnings: Non-vital errors.
// Results: Final results after all trials.
// TrialResults: Results of each trial.
// AllActions: Detailed breakdown of each trial.
enum OL
{
    None = 0,
    Errors = 1,
    Warnings = 2,
    Results = 3,
    TrialResults = 4,
    AllActions = 5
};

// Change this to have compiler automatically remove some or all debug code.
const enum OL MAX_OUTPUT_LEVEL = AllActions;

class Output
{
public:

    explicit Output(std::ostream & out_stream = std::cout, enum OL level = None);

    //void operator()(enum OutputLevel level, const char * message, ...);
    bool operator()(enum OL level) const;

    std::ostream & O();

    OL Level;
    std::ostream * OutStream;
};

extern Output Out;

std::ostream & operator<<(std::ostream & out, const ActorQueue & actors);

std::ostream & operator<<(std::ostream & out, const Arena & arena);

std::ostream & operator<<(std::ostream & out, const Group & group);

std::ostream & operator<<(std::ostream & out, const Groups & groups);

std::ostream & operator<<(std::ostream & out, const Actor & actor);

std::ostream & operator<<(std::ostream & out, const Action & action);
