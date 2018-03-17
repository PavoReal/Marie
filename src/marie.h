#pragma once

#include <cstdint>

#define INTERNAL static
#define GLOBAL   static

using uint   = unsigned int;
using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8  = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

#pragma pack(push, 1)
struct BinaryInstruction
{
    union
    {
        struct
        {
            uint16 word;
        };
        
        struct 
        {
            uint8 byte0;
            uint8 byte1;
        };

        struct 
        {
            unsigned addr: 12;
            unsigned opCode: 4;
        };
    };
};
#pragma pack(pop)

struct Marie
{
    // Accumulator register
    int16 regAC;

    // Memory address register
    unsigned regMAR: 12;

    // Memory buffer register
    uint16 regMBR;

    // Program counter
    unsigned regPC: 12;

    // Instruction register
    BinaryInstruction regIR; 

    // Input register
    uint16 regIN;

    // Output register
    uint16 regOUT;

    // Main memory
    uint16 *memory;
};

struct SrcInstruction
{
    unsigned opCode: 4;

    char *name;
};

struct Symbol
{
    char *name;

    uint16 addr;
};

struct MatchResult
{
    float weight;

    void *match;
};

