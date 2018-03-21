#pragma once

#include <cstdint>

#define INTERNAL static
#define GLOBAL   static

using uint = unsigned int;
using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

#pragma pack(push, 1)
struct BinaryInstruction
{
    union
    {
        struct
        {
            u16 word;
        };
        
        struct 
        {
            u8 byte0;
            u8 byte1;
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
    s16 regAC;

    // Memory address register
    unsigned regMAR: 12;

    // Memory buffer register
    u16 regMBR;

    // Program counter
    unsigned regPC: 12;

    // Instruction register
    BinaryInstruction regIR; 

    // Input register
    u16 regIN;

    // Output register
    u16 regOUT;

    // Main memory
    u16 *memory;
};

struct SrcInstruction
{
    char *name;
    unsigned opCode: 4;
};

struct Symbol
{
    char *name;
    u16 addr;
};

struct MatchResult
{
    void *match;
    float weight;
};

