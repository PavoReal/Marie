#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>

#include "marie.h"
#include "strings.h"
#include "list.h"

using SymbolList = TList<Symbol>;

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
    MakeSrcInstruction(0x1, "Load"),
    MakeSrcInstruction(0x2, "Store"),
    MakeSrcInstruction(0x3, "Add"),
    MakeSrcInstruction(0x4, "Subt"),
    MakeSrcInstruction(0x5, "Input"),
    MakeSrcInstruction(0x6, "Output"),
    MakeSrcInstruction(0x7, "Halt"),
    MakeSrcInstruction(0x8, "Skipcond"),
    MakeSrcInstruction(0x9, "Jump"),
    MakeSrcInstruction(0xA, "Clear")
};

enum class LineResult
{
    VALID,
    ERROR,
    SKIP
};

INTERNAL LineResult
GetInstructionFromString(BinaryInstruction *instr, SymbolList *symbols, char *src)
{
    LineResult result = LineResult::ERROR;

    uint8 opCode = {};
    uint16 addr  = {};

    size_t srcLength = strlen(src);
    while ((*src == ' ' || *src == '\t') && srcLength)
    {
        ++src;
        --srcLength;
    }

    if (!srcLength)
    {
        result = LineResult::SKIP;
    }
    else
    {
        if (StringContains(src, ','))
        {
            src += StringLengthTo(src, ',') + 1;

            srcLength = strlen(src);
            while ((*src == ' ' || *src == '\t') && srcLength)
            {
                ++src;
                --srcLength;
            }
        }

        srcLength = strlen(src);
        size_t lengthToSpace = StringLengthTo(src, ' ');

        char *addrString = nullptr;

        if (srcLength != lengthToSpace)
        {
            addrString = src + lengthToSpace + 1;
            src[lengthToSpace] = '\0';

            for (uint32 i = 0; (i < symbols->count) && (!addr); ++i)
            {
                Symbol *symbol = PeakAt(symbols, i);

                if (strcmp(symbol->name, addrString) == 0)
                {
                    addr = symbol->addr;
                }
            }

            if (!addr)
            {
                if (tolower(*addrString) == 'x')
                {
                    addr = HexCharsToNum<uint16>(addrString + 1);
                }
                else
                {
                    addr = DecCharsToNum<uint16>(addrString, strlen(addrString));
                }
            }
        }

        for (const SrcInstruction &str : SRC_INSTRUCTIONS)
        {
            if (stricmp(src, str.name) == 0)
            {
                opCode = str.opCode;

                result = LineResult::VALID;

                break;
            }
        }

        if (result != LineResult::VALID)
        {
            if (stricmp(src, "DEC") == 0)
            {
                instr->word = addr;
                result = LineResult::VALID;
            }
            else if (stricmp(src, "HEX") == 0)
            {
                instr->word = HexCharsToNum<uint16>(addrString);
                result = LineResult::VALID;
            }
        }
        else
        {
            instr->opCode = opCode;
            instr->addr = addr;
        }
    }

    return result;
}

INTERNAL LineResult
GetSymbolFromLine(Symbol *symbol, char *line, uint32 lineNum)
{
    LineResult result = LineResult::ERROR;

    size_t srcLength = strlen(line);
    while ((*line == ' ' || *line == '\t') && srcLength)
    {
        ++line;
        --srcLength;
    }

    if (!srcLength)
    {
        result = LineResult::SKIP;
    }
    else
    {
        char *read = line;
        while (*read && *read != ',')
        {
            ++read;
        }

        size_t lineLength = strlen(line);
        size_t delta = read - line;

        if (delta != lineLength)
        {
            *read = '\0';
            symbol->name = static_cast<char*>(malloc(strlen(line) + 1));
            strcpy(symbol->name, line);
            symbol->addr = lineNum;

            result = LineResult::VALID;
        }
    }

    return result;
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
            outputFile = fopen(argv[2], "wb");
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

    fpos_t inputFileStartPos = {};
    fgetpos(inputFile, &inputFileStartPos);

    const size_t INPUT_BUFFER_SIZE = 64;
    char inputBuffer[INPUT_BUFFER_SIZE];

    //
    fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);
    // Pass 1 - Symbol table

    uint32 line = 0;
    SymbolList symbols = {};
    while (!feof(inputFile))
    {
        StripCharFromString(inputBuffer, '\n');

        Symbol symbol = {};
        LineResult valid = GetSymbolFromLine(&symbol, inputBuffer, line);

        if (valid == LineResult::VALID)
        {
            Push(&symbols, symbol);
        }

        fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);

        if (valid != LineResult::SKIP)
        {
            ++line;
        }
    }

    //
    fsetpos(inputFile, &inputFileStartPos);
    fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);
    // Pass 2 

    line = 1;
    size_t memoryIndex = 0;
    while (!feof(inputFile) && memoryIndex < 4096)
    {
        StripCharFromString(inputBuffer, '\n');

        BinaryInstruction instr = {};
        LineResult lineResult = GetInstructionFromString(&instr, &symbols, inputBuffer);

        if (lineResult == LineResult::VALID)
        {
            // NOTE(Peacock): Should we make sure the bit order on disk is what we expect?
            marieMem[memoryIndex++] = instr.word; // ((instr.byte0 << 8) | (instr.byte1));
        }
        else if (lineResult == LineResult::ERROR)
        {
            fprintf(stderr, "Error: Invalid instruction on line %d...\n<< %s >>\nAborting...\n", line, inputBuffer);

            break;
        }

        ++line;
        fgets(inputBuffer, INPUT_BUFFER_SIZE, inputFile);
    }

    Free(&symbols);

    fclose(inputFile);

    fwrite(marieMem, sizeof(uint16), 4096, outputFile);

    free(marieMem);
    fclose(outputFile);

    return 0;
}

