//
// Created by bryan on 2/23/20.
//

#pragma once

#include <iostream>
#include <thread>
#include "Types.h"

// Output levels. Each entry includes all levels above it.
// 0: No output.
// Errors: Vital errors
// Warnings: Non-vital errors.
// Info: System status messages.
// Results: Final results after all trials.
// TrialResults: Results of each trial.
// AllActions: Detailed breakdown of each trial.
enum MessageLevel
{
    NoOutput,
    Errors,
    Warnings,
    Info,
    Results,
    TrialResults,
    AllActions,
    AllHPChanges,
    AllOutput
};

// Change this to have compiler automatically remove some or all debug code.
const enum MessageLevel MAX_OUTPUT_LEVEL = AllOutput;

class Output
{
public:
    static Output & Out();

    explicit Output(std::ostream & out_stream = std::cout, enum MessageLevel level = NoOutput);

    [[nodiscard]] bool CheckLevel(enum MessageLevel level) const;
    [[nodiscard]] std::ostream & GetStream();
    bool SetLevel(MessageLevel new_level);
    void SetDelay(int milliseconds);

    std::chrono::milliseconds MessageDelay{0};
    MessageLevel Level;
    std::ostream * MessageStream;
    int errors{0};
    int warnings{0};
};

#define OUT_ERROR {++Output::Out().errors; if(Output::Out().CheckLevel(Errors)) Output::Out().GetStream() << "\x1B[31mError: " << __FILE__ << " : " << __LINE__ << " : "
#define ERROR_END "\033[0m\t\t" << std::endl;}

#define OUT_WARNING {++Output::Out().warnings; if(Output::Out().CheckLevel(Warnings)) Output::Out().GetStream() << "\x1B[33mWarning: " << __FILE__ << " : " << __LINE__ << " : "
#define WARNING_END "\033[0m\t\t" << std::endl;}

#define OUT_INFO {if(Output::Out().CheckLevel(Info)) Output::Out().GetStream() << "\x1B[32mInfo: "
#define INFO_END "\033[0m\t\t" << std::endl;}

#define OUT_RESULTS {if(Output::Out().CheckLevel(Results)) Output::Out().GetStream()
#define OUT_TRIALS {if(Output::Out().CheckLevel(TrialResults)) Output::Out().GetStream()
#define OUT_ALL {if(Output::Out().CheckLevel(AllActions)) Output::Out().GetStream()
#define OUT_HP {if (Output::Out().CheckLevel(AllHPChanges)) Output::Out().GetStream()
#define ALL_CONT ;}
#define ALL_ENDL << std::endl; Output::Out().GetStream().flush(); std::this_thread::sleep_for(Output::Out().MessageDelay);}

std::ostream & operator<<(std::ostream & out, const Arena & arena);
std::ostream & operator<<(std::ostream & out, const Group & group);
std::ostream & operator<<(std::ostream & out, const Groups & groups);
std::ostream & operator<<(std::ostream & out, const Actor & actor);
std::ostream & operator<<(std::ostream & out, const Action & action);
std::ostream & operator<<(std::ostream & out, enum DamageType damage_type);
