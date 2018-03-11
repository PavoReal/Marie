#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "marie.h"
#include "strings.h"

#define MEM_DUMP "mem-dump"

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

    bool running = true;
    while (running)
    {
        marie.regMAR = marie.regPC;
        
        marie.regIR.word = marie.memory[marie.regMAR];
        marie.regPC++;

        marie.regMAR = marie.regIR.addr;

        switch (marie.regIR.opCode)
        {
            case 0x1:
            {
                marie.regMBR = marie.memory[marie.regIR.addr];
                marie.regAC = marie.regMBR;
            } break;

            case 0x2:
            {
                marie.memory[marie.regIR.addr] = marie.regAC;
            } break;

            case 0x3:
            {
                marie.regMBR = marie.memory[marie.regIR.addr];
                marie.regAC += marie.regMBR;
            } break;

            case 0x4:
            {
                marie.regMBR = marie.memory[marie.regIR.addr];
                marie.regAC -= marie.regMBR;
            } break;

            case 0x5:
            {
                int8 input = {};

                printf("? ");
                scanf("%hhd", &input);

                marie.regIN = input;
                marie.regAC = marie.regIN;
            } break;

            case 0x6:
            {
                printf("%d\n", marie.regAC);
            } break;

            case 0x7:
            {
                running = false;
            } break;

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

            case 0x9:
            {
                marie.regPC = marie.regIR.addr;
            } break;

            case 0xA:
            {
                marie.regAC = 0;
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

    printf("[MARIE] - Register Dump:\n");
    printf("[MARIE] - Register AC  - %#X\n", marie.regAC);
    printf("[MARIE] - Register MAR - %#X\n", marie.regMAR);
    printf("[MARIE] - Register MBR - %#X\n", marie.regMBR);
    printf("[MARIE] - Register PC  - %#X\n", marie.regPC);
    printf("[MARIE] - Register IR  - %#X\n", marie.regIR.word);
    printf("[MARIE] - Register IN  - %#X\n", marie.regIN);
    printf("[MARIE] - Register OUT - %#X\n", marie.regOUT);

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

