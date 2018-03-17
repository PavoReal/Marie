//
// TODO list
// - Redo command interface
// - Match with src file
// - Modular
// - Command line args?
// - Documentation
// - There's a bug with either memory or addition/subtraction
//    - Sign error?
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "marie.h"
#include "strings.h"

#define MEM_DUMP "mem-dump"

INTERNAL void
RegisterDump(Marie *marie)
{
    printf("[MARIE] - Register Dump:\n");
    printf("[MARIE] - AC  - %#X\n", marie->regAC);
    printf("[MARIE] - MAR - %#X\n", marie->regMAR);
    printf("[MARIE] - MBR - %#X\n", marie->regMBR);
    printf("[MARIE] - PC  - %#X\n", marie->regPC);
    printf("[MARIE] - IR  - %#X\n", marie->regIR.word);
    printf("[MARIE] - IN  - %#X\n", marie->regIN);
    printf("[MARIE] - OUT - %#X\n", marie->regOUT);
}

int
main(int argc, char **argv)
{
    FILE *inputFile  = nullptr;

    if (argc >= 2)
    {
        inputFile = fopen(argv[1], "rb");
    }
    else
    {
        fprintf(stderr, "[MARIE] Error: No input file provided\n");

        return 1;
    }

    Marie marie = {};
    marie.memory = static_cast<uint16*>(calloc(sizeof(uint16), 4096));

    fread(marie.memory, sizeof(uint16), 4096, inputFile);
    fclose(inputFile);

    bool stepping = true;
    bool running  = true;

    while (running)
    {
        bool step = false;

        const size_t CMD_BUFFER_SIZE = 32;
        char cmd[CMD_BUFFER_SIZE];

        if (stepping)
        {
            // 
            // TODO(Peacock): This command interface is trash, redo it
            //
            printf("[MARIE] PC: %#X - %#X\n", marie.regPC, marie.memory[marie.regPC]);
            printf("[MARIE] cmd? ");
            fgets(cmd, CMD_BUFFER_SIZE, stdin);

            switch (tolower(*cmd))
            {
                case 'r':
                    stepping = false;
                break;

                case 'q':
                    running = false;
                break;

                case 'd':
                    RegisterDump(&marie);
                break;

                case 's':
                    step = true;
                break;

                case 'm':
                {
                    for (unsigned i = marie.regPC - 5; i < marie.regPC; ++i)
                    {
                        printf("%#X - %#X\n", i, marie.memory[i]);
                    }

                    printf("<<%#X>> - %#X\n", marie.regPC, marie.memory[marie.regPC]);

                    for (unsigned i = marie.regPC + 1; i < marie.regPC + 6; ++i)
                    {
                        printf("%#X - %#X\n", i, marie.memory[i]);
                    }
                } break;

                default:
                    printf("[MARIE] Unknown command\n");
                break;
            }
        }
        else
        {
            step = true;
        }

        if (step)
        {
            marie.regMAR = marie.regPC;
            
            marie.regIR.word = marie.memory[marie.regMAR];
            marie.regPC++;

            marie.regMAR = marie.regIR.addr;

            switch (marie.regIR.opCode)
            {
                //
                // Jns x
                // 
                // MAR      <-- x
                // MBR      <-- PC 
                // MEM[MAR] <-- MBR
                // AC       <-- x + 1
                // PC       <-- AC
                // 
                case 0x0:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.regPC;
                    marie.memory[marie.regMAR] = marie.regMBR;
                    marie.regAC = marie.regIR.addr + 1;
                    marie.regPC = marie.regAC;
                } break;

                //
                // Load x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // AC  <-- MBR
                //
                case 0x1:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regAC  = marie.regMBR;
                } break;

                //
                // Store x
                //
                // MAR      <-- x
                // MBR      <-- AC
                // MEM[MAR] <-- MBR
                //
                case 0x2:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.regAC;
                    marie.memory[marie.regMAR] = marie.regMBR;
                } break;

                //
                // Add x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // AC  <-- AC + MBR
                //
                case 0x3:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regAC  = marie.regAC + marie.regMBR;
                } break;

                //
                // Subt x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // AC  <-- AC - MBR
                //
                case 0x4:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regAC  = marie.regAC - marie.regMBR;
                } break;

                //
                // Input
                //
                // IN <-- (stdin)
                // AC <-- IN
                //
                case 0x5:
                {
                    int8 input = {};

                    printf("? ");
                    scanf("%hhd", &input);

                    marie.regIN = input;
                    marie.regAC = marie.regIN;
                } break;

                //
                // Output
                //
                // OUT      <-- AC
                // (stdout) <-- OUT
                //
                case 0x6:
                {
                    marie.regOUT = static_cast<int8>(marie.regAC);
                    printf("%d\n", marie.regOUT);
                } break;

                //
                // Halt
                //
                case 0x7:
                {
                    running = false;
                } break;

                //
                // Skipcond x
                //
                // If      IR[11 - 10] = 00 then
                //     If AC < 0 then PC <-- PC + 1
                // else If IR[11 - 10] = 01 then
                //     If AC = 0 then PC <-- PC + 1
                // else If IR[11 - 10] = 10 then
                //     if AC > 0 then PC <-- PC + 1
                //
                case 0x8:
                {
                    if (marie.regIR.addr == 0x800)
                    {
                        if (marie.regAC > 0)
                        {
                            marie.regPC += 1;
                        }
                    }
                    else if (marie.regIR.addr == 0x0)
                    {
                        if (marie.regAC < 0)
                        {
                            marie.regPC += 1;
                        }
                    }
                    else if (marie.regIR.addr == 0x400)
                    {
                        if (marie.regAC == 0)
                        {
                            marie.regPC += 1;
                        }
                    }
                } break;

                //
                // Jump x
                //
                // PC <-- x
                //
                case 0x9:
                {
                    marie.regPC = marie.regIR.addr;
                } break;

                //
                // Clear
                //
                // AC <-- 0
                //
                case 0xA:
                {
                    marie.regAC = 0;
                } break;

                //
                // AddI x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // MAR <-- MBR
                // MBR <-- MEM[MAR]
                // AC  <-- AC + MBR
                //
                case 0xB:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regMAR = marie.regMBR;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regAC  = marie.regAC + marie.regMBR;
                } break;

                //
                // JumpI x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // MAR <-- MBR
                // MBR <-- MEM[MAR]
                // PC  <-- MBR
                //
                case 0xC:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regMAR = marie.regMBR;
                    marie.regMBR = marie.memory[marie.regMAR];
                } break;

                //
                // LoadI x
                //
                // MAR <-- x
                // MBR <-- MEM[MAR]
                // MAR <-- MBR
                // MBR <-- MEM[MAR]
                // AC  <-- MBR
                //
                case 0xD:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regMAR = marie.regMBR;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regAC  = marie.regMBR;
                } break;

                //
                // StoreI x
                //
                // MAR      <-- x
                // MBR      <-- MEM[MAR]
                // MAR      <-- MBR
                // MBR      <-- AC
                // MEM[MAR] <-- MBR
                //
                case 0xE:
                {
                    marie.regMAR = marie.regIR.addr;
                    marie.regMBR = marie.memory[marie.regMAR];
                    marie.regMAR = marie.regMBR;
                    marie.regMBR = marie.regAC;
                    marie.memory[marie.regMAR] = marie.regMBR;
                } break;

                default:
                {
                    fprintf(stderr, "[MARIE] - Invalid instruction...\n<< %#X >>\n", marie.regIR.word);
                    printf("[MARIE] - Skip over (y or n) ?");

                    char skip;
                    scanf("%c", &skip);

                    if (skip == 'n')
                    {
                        running = false;
                    }
                } break;
            }
        }
    }

    RegisterDump(&marie);

    FILE *memoryDumpFile = fopen(MEM_DUMP, "w");

    if (memoryDumpFile)
    {
        printf("[MARIE] - Dumping memory to file: %s\n", MEM_DUMP);

        fwrite(marie.memory, 1, 4096, memoryDumpFile);
        fclose(memoryDumpFile);
    }
    else
    {
        printf("[MARIE] - Could not dump memory to file: %s - Check if the file is locked...\n", MEM_DUMP);
    }

    free(marie.memory);

    return 0;
}

