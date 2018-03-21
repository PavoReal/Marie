#pragma once

#include "marie.h"

struct AssemblerState
{
    SrcInstruction *srcInstructions;
    u32 srcInstructionCount;
};

struct LineResult
{
    enum Type
    {
        VALID,
        ERROR,
        SKIP
    };

    Type type;
};
