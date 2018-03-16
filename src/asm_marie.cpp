
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>

#include "marie.h"
#include "list.h"

using SymbolList = TList<Symbol>;

#define OUTPUT_FILE_DEFAULT "a.marie"

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

    // Remove any tabs or spaces at the start of the line
    RemoveStartingSpaces(&src);

    size_t srcLength = StringLength(src);
    if (!srcLength)
    {
        // If the line is empty we'll skip it
        result = LineResult::SKIP;
    }
    else
    {
        // If the line has a label, ignore it
        // since we've already created the symbol table
        // in the first pass
        if (StringContains(src, ','))
        {
            src += StringLengthTo(src, ',') + 1;

            // Make sure there arn't any spaces
            RemoveStartingSpaces(&src);
        }

        // Get the new line length and the length to the next space
        // if they ARE the same, we don't have an address with the opcode
        // if they ARE NOT the same, we do have an address with the opcode
        srcLength = StringLength(src);
        size_t lengthToSpace = StringLengthTo(src, ' ');

        char *addrString = nullptr;

        if (srcLength != lengthToSpace)
        {
            // One past the space
            addrString = src + lengthToSpace + 1;

            // Set the space to a nullterminator
            // to seperate the opcode and address
            src[lengthToSpace] = '\0';

            // Loop through the symbol table and look for a matching symbol
            for (uint32 i = 0; (i < symbols->count) && (!addr); ++i)
            {
                Symbol *symbol = PeakAt(symbols, i);

                // case sensitive
                if (strcmp(symbol->name, addrString) == 0)
                {
                    addr = symbol->addr;
                }
            }

            // If we didn't find a symbol, assume the address string is a constant
            if (!addr)
            {
                int base = 10;

                // If the constant has a prefix of "0x", "0X', "x" or "X", we treat it as a hex constant
                if (tolower(*addrString) == 'x' ||
                    ((*addrString == '0') && (tolower(*(addrString + 1)) == 'x')))
                {
                    base = 16;

                    if (*addrString == '0')
                    {
                        addrString += 2;
                    }
                    else
                    {
                        addrString += 1;
                    }
                }
                
                // We have our address
                addr = CharsToNum<uint16>(addrString, base);
            }
        }

        // Lookup the opcode
        for (const SrcInstruction &str : SRC_INSTRUCTIONS)
        {
            if (strcmp(src, str.name) == 0)
            {
                opCode = str.opCode;

                result = LineResult::VALID;

                break;
            }
        }

        // If we didn't find the opcode, it could still be a DEC or HEX constant
        if (result != LineResult::VALID)
        {
            if (stricmp(src, "DEC") == 0)
            {
                instr->word = addr;
                result = LineResult::VALID;
            }
            else if (stricmp(src, "HEX") == 0)
            {
                instr->word = CharsToNum<uint16>(addrString, 16);
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

    RemoveStartingSpaces(&line);

    size_t srcLength = StringLength(line);
    if (!srcLength)
    {
        // Empty line
        result = LineResult::SKIP;
    }
    else
    {
        char *read = line;
        while (*read && *read != ',')
        {
            ++read;
        }

        size_t lineLength = StringLength(line);
        size_t delta = read - line;

        if (delta != lineLength)
        {
            *read = '\0';
            symbol->name = static_cast<char*>(malloc(StringLength(line) + 1));
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

