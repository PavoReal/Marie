#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "marie.h"
#include "strings.h"

#define OUTPUT_FILE_DEFAULT "a.marie"

INTERNAL SrcInstruction
MakeSrcInstruction(uint8 opCode, const char *name)
{
    SrcInstruction instr = {};

    instr.name = static_cast<char*>(malloc(strlen(name) + 1));
    strcpy(instr.name, name);

    instr.opCode = opCode;

    return instr;
}

const SrcInstruction SRC_INSTRUCTIONS[] =
{
    MakeSrcInstruction({0x1}, "Load"),
    MakeSrcInstruction({0x2}, "Store"),
    MakeSrcInstruction({0x3}, "Add"),
    MakeSrcInstruction({0x4}, "Subt"),
    MakeSrcInstruction({0x5}, "Input"),
    MakeSrcInstruction({0x6}, "Output"),
    MakeSrcInstruction({0x7}, "Halt"),
    MakeSrcInstruction({0x8}, "Skipcond"),
    MakeSrcInstruction({0x9}, "Jump"),
};

INTERNAL bool
GetInstructionFromString(BinaryInstruction *result, char *src)
{
    bool valid = false;

    uint8 opCode = {};
    uint16 addr  = {};

    while (*src == ' ')
    {
        ++src;
    }

    size_t srcLength = strlen(src);
    size_t lengthToSpace = StringLengthTo(src, ' ');

    char *addrString = nullptr;

    if (srcLength != lengthToSpace)
    {
        addrString = src + lengthToSpace + 1;
        src[lengthToSpace] = '\0';

        addr = CharsToNum<uint16>(addrString, strlen(addrString));
    }

    for (const SrcInstruction &instr : SRC_INSTRUCTIONS)
    {
        if (stricmp(src, instr.name) == 0)
        {
            opCode = instr.opCode;

            valid = true;

            break;
        }
    }

    result->opCode = opCode;
    result->addr = addr;

    return valid;
}

int
main(int argc, char **argv)
{
    FILE *inputFile  = nullptr;
    FILE *outputFile = nullptr;

    if (argc >= 2)
    {
        inputFile  = fopen(argv[1], "r");

        if (argc >= 3)
        {
            outputFile = fopen(argv[2], "w");
        }

        if (!inputFile)
        {
            fprintf(stderr, "Error: Could not read input file: %s...\n", argv[1]);

            return 1;
        }

        if (!outputFile)
        {
            printf("Invalid or no input file provided, defaulting to %s...\n", OUTPUT_FILE_DEFAULT);

            outputFile = fopen(OUTPUT_FILE_DEFAULT, "w");

            if (!outputFile)
            {
                fprintf(stderr, "Error: Could not open output file %s...\n", OUTPUT_FILE_DEFAULT);

                fclose(inputFile);

                return 1;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: No input file provided...\n");

        return 1;
    }

    uint16 *marieMem = static_cast<uint16*>(calloc(sizeof(uint16), 4096));

    const size_t INPUT_BUFFER_SIZE = 64;
    char *inputBuffer = static_cast<char*>(calloc(1, INPUT_BUFFER_SIZE));

    fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);

    size_t memoryIndex = 0;
    uint32 line = 1;
    while (!feof(inputFile) && memoryIndex < 4096)
    {
        StripCharFromString(inputBuffer, '\n');
        StripCharFromString(inputBuffer, '\t');

        BinaryInstruction instr = {};
        bool validInstr = GetInstructionFromString(&instr, inputBuffer);

        if (validInstr)
        {
            // NOTE(Peacock): The bit order on disk does not repersent the bit order when loaded into memory...
            marieMem[memoryIndex++] = instr.word; //((instr.byte0 << 8) | (instr.byte1));
        }
        else
        {
            fprintf(stderr, "Error: Invalid instruction on line %d...\n<< %s >>\nAborting...\n", line, inputBuffer);

            break;
        }

        ++line;
        fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);
    }

    free(inputBuffer);
    fclose(inputFile);

    fwrite(marieMem, sizeof(uint16), 4096, outputFile);

    free(marieMem);
    fclose(outputFile);

    return 0;
}

