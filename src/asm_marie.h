#pragma once

#include "marie.h"

struct AssemblerState
{
    SrcInstruction *srcInstructions;
    uint32 srcInstructionCount;
};

// TODO(Peacock): Make this a struct with more info like closest instr match
enum class LineResult
{
    VALID,
    ERROR,
    SKIP
};

