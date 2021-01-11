/*
 *  CS3421 Assignment 4
 *  Name: Alex Hromada
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "sim.h"

int main(int argc, char **argv)
{

    char insInput[12];          // instruction read from file
    int textSize;               // Size of text segment
    int dataSize;               // Size of data segment
    int i;                      // Loop counter
    unsigned int lineCount = 0; // total number of lines in file
    union instruction *instrs;  //
    FILE *inputf = fopen(argv[1], "r");
    FILE *outputf = fopen("log.txt", "w+");

    fgets(insInput, 12, inputf);
    sscanf(insInput, "%d %d", &textSize, &dataSize);

    fprintf(outputf, "insts: \n");

    // decoding
    // Read instructions
    instrs = malloc(textSize * sizeof(union instruction));
    for (i = 0; i < textSize; i++)
    {
        fgets(insInput, 12, inputf);
        sscanf(insInput, "%x", &(instrs[i].hex));
        fprintf(outputf, "%4d: ", lineCount);
        printInstr(instrs[i], &outputf);
        lineCount++;
    }

    fprintf(outputf, "\n");

    int gp = lineCount;

    int *data;   // data array
    int dataval; // value stored in location of data array

    // Read data
    data = malloc(dataSize * sizeof(int));
    fprintf(outputf, "data: \n");
    for (i = 0; i < dataSize; i++)
    {
        fgets(insInput, 12, inputf);
        sscanf(insInput, "%x", &dataval);
        data[i] = dataval;
        fprintf(outputf, "%4d: %d\n", lineCount, data[i]);
        lineCount++;
    }

    fclose(inputf);

    int regs[32];
    // initialize register array
    for (i = 0; i < 32; i++)
    {
        regs[i] = 0;
    }
    regs[28] = gp;
    int hi = 0; // $hi register value
    int lo = 0; // $lo register value
    unsigned int pc = 0;

    // execution
    while (1 == 1)
    {
        // Step 1: get instruction from instrs[pc]
        unsigned int opcode = instrs[pc].r_type.opcode;
        unsigned int rs = instrs[pc].r_type.rs;
        unsigned int rt = instrs[pc].r_type.rt;
        unsigned int rd = instrs[pc].r_type.rd;
        unsigned int func = instrs[pc].r_type.func;
        int immedI = instrs[pc].i_type.immed;
        int immedJ = instrs[pc].j_type.immed;

        fprintf(outputf, "PC: %u\n", pc);
        fprintf(outputf, "inst: ");
        printInstr(instrs[pc], &outputf);
        fprintf(outputf, "\n");

        switch (opcode)
        {
        case 0:
            switch (func)
            {
            case 12:
                switch (regs[2])
                {
                case 1: // Print an integer from $a0 followed by new line
                    printf("%d\n", regs[4]);
                    pc = pc + 1;
                    break;
                case 5: // Read an integer from stdin and assign to $v0
                    scanf("%d", &regs[2]);
                    pc = pc + 1;
                    break;
                case 10: // End the program
                    fprintf(outputf, "exiting simulation\n");
                    return 0;
                default:
                    pc = pc + 1;
                    break;
                }
                break;
            case 16: // mfhi
                regs[rd] = hi;
                pc = pc + 1;
                break;
            case 18: // mflo
                regs[rd] = lo;
                pc = pc + 1;
                break;
            case 24: // mult
                lo = regs[rs] * regs[rt];
                hi = regs[rs] * regs[rt];
                pc = pc + 1;
                break;
            case 26: // div
                if (rt == 0)
                {
                    printf("divide by zero\nexiting simulation\n");
                    return 0;
                }
                lo = regs[rs] / regs[rt];
                hi = regs[rs] % regs[rt];
                pc = pc + 1;
                break;
            case 33: // addu
                regs[rd] = regs[rs] + regs[rt];
                pc = pc + 1;
                break;
            case 35: // subu
                regs[rd] = regs[rs] - regs[rt];
                pc = pc + 1;
                break;
            case 36: // bitwise and
                regs[rd] = regs[rs] & regs[rt];
                pc = pc + 1;
                break;
            case 37: // bitwise or
                regs[rd] = regs[rs] | regs[rt];
                pc = pc + 1;
                break;
            case 42: // slt
                if (regs[rs] < regs[rt])
                    regs[rd] = 1;
                else
                    regs[rd] = 0;
                pc = pc + 1;
                break;
            default:
                printf("illegal instruction\nexiting simulation\n");
                return 0;
            }
            break;
        case 2: // j
            pc = immedJ;
            break;
        case 4: // beq
            if (regs[rs] == regs[rt])
                pc = pc + immedI;
            else
                pc = pc + 1;
            break;
        case 5: // bne
            if (regs[rs] != regs[rt])
                pc = pc + immedI;
            else
                pc = pc + 1;
            break;
        case 9: // addiu
            regs[rt] = regs[rs] + immedI;
            pc = pc + 1;
            break;
        case 35:                          // lw
            if ((regs[rs] + immedI) < gp) //  Make sure the address is within the data segment
            {
                fprintf(outputf, "illegal data address\nexiting simulation\n");
                return 0;
            }
            if (regs[rs] == gp) // load from 0 index for $gp
            {
                regs[rt] = data[immedI];
            }
            else // load value from data array
            {
                regs[rt] = data[regs[rs] + immedI];
            }
            pc = pc + 1;
            break;
        case 43:                          // sw
            if ((regs[rs] + immedI) < gp) //  Make sure the address is within the data segment
            {
                fprintf(outputf, "illegal data address\nexiting simulation\n");
                return 0;
            }
            if (regs[rs] == gp) // Set the 0 index of data[] since $gp is start of data segment
            {
                data[immedI] = regs[rt];
            }
            else // Insert value into data array
            {
                data[regs[rs] + immedI] = regs[rt];
                if (immedI > dataSize)
                    dataSize = immedI;
            }
            pc = pc + 1;
            break;
        default:
            printf("illegal instruction\nexiting simulation\n");
            return 0;
        }

        // Log output: Registers
        fprintf(outputf, "regs: \n");
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(0), regs[0], getReg(1), regs[1], getReg(2), regs[2], getReg(3), regs[3]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(4), regs[4], getReg(5), regs[5], getReg(6), regs[6], getReg(7), regs[7]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(8), regs[8], getReg(9), regs[9], getReg(10), regs[10], getReg(11), regs[11]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(12), regs[12], getReg(13), regs[13], getReg(14), regs[14], getReg(15), regs[15]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(16), regs[16], getReg(17), regs[17], getReg(18), regs[18], getReg(19), regs[19]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(20), regs[20], getReg(21), regs[21], getReg(22), regs[22], getReg(23), regs[23]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(24), regs[24], getReg(25), regs[25], getReg(26), regs[26], getReg(27), regs[27]);
        fprintf(outputf, "%8s =\t%d\t%s =\t%d\t%s =\t%d\t%s =\t%d\n", getReg(28), regs[28], getReg(29), regs[29], getReg(30), regs[30], getReg(31), regs[31]);
        fprintf(outputf, "     $lo =\t%d\t$hi =\t%d\n", lo, hi);
        fprintf(outputf, "\n");

        // Log output
        fprintf(outputf, "data memory:\n");
        for (i = 0; i < dataSize; i++)
        {
            fprintf(outputf, "   data[%3d] = \t%d\n", i, data[i]);
        }

        fprintf(outputf, "\n");
    }

    fclose(outputf);

    free(instrs);
    free(data);
    return 0;
}

/*
    Prints a line of an instruction
    For debugging
*/
void printInstr(union instruction instruc, FILE **out)
{

    char *rs;
    char *rt;
    char *rd;
    char *function;
    char *immediate;

    // Check opcode
    switch (instruc.r_type.opcode)
    {
    case 0: // 0 is RType

        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        rd = getReg(instruc.r_type.rd);
        function = getRType(instruc.r_type.func);

        switch (instruc.r_type.func)
        {
        case 12:
            fprintf(*out, "%s\n", function); // print j instruction
            break;
        case 16:
        case 18:
            fprintf(*out, "%s\t%s\n", function, rd); // print mflo, mfhi
            break;
        case 24:
        case 26:
            fprintf(*out, "%s\t%s,%s\n", function, rs, rt); // print mult, div
            break;
        case 33:
        case 35:
        case 36:
        case 37:
        case 42:
            fprintf(*out, "%s\t%s,%s,%s\n", function, rd, rs, rt); // print remain r type instructions
            break;
        default:
            break;
        }
        break;
    case 2: // j instruction
        fprintf(*out, "j\t%d\n", instruc.j_type.immed);
        break;
    case 4: // beq instruction
        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        fprintf(*out, "beq\t%s,%s,%d\n", rs, rt, instruc.i_type.immed);
        break;
    case 5: // bne instruction
        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        fprintf(*out, "bne\t%s,%s,%d\n", rs, rt, instruc.i_type.immed);
        break;
    case 9:
        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        fprintf(*out, "addiu\t%s,%s,%d\n", rt, rs, instruc.i_type.immed);
        break;
    case 35:
        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        fprintf(*out, "lw\t%s,%d(%s)\n", rt, instruc.i_type.immed, rs);
        break;
    case 43:
        rs = getReg(instruc.r_type.rs);
        rt = getReg(instruc.r_type.rt);
        fprintf(*out, "sw\t%s,%d(%s)\n", rt, instruc.i_type.immed, rs);
        break;
    default:
        fprintf(*out, "\n");
        break;
    }
}

/*
    Helper function for printing logs
    Get's instruction name from R type func codes
*/
char *getRType(unsigned int func)
{
    switch (func)
    {
    case 12:
        return "syscall";
    case 16:
        return "mfhi";
    case 18:
        return "mflo";
    case 24:
        return "mult";
    case 26:
        return "div";
    case 33:
        return "addu";
    case 35:
        return "subu";
    case 36:
        return "and";
    case 37:
        return "or";
    case 42:
        return "slt";
    default:
        return "unknown";
    }
}

/*
    Helper function for printing logs
    Get's the register from rs, rd, rt values
*/
char *getReg(unsigned int reg)
{
    switch (reg)
    {
    case 0:
        return "$zero";
    case 1:
        return "$at";
    case 2:
        return "$v0";
    case 3:
        return "$v1";
    case 4:
        return "$a0";
    case 5:
        return "$a1";
    case 6:
        return "$a2";
    case 7:
        return "$a3";
    case 8:
        return "$t0";
    case 9:
        return "$t1";
    case 10:
        return "$t2";
    case 11:
        return "$t3";
    case 12:
        return "$t4";
    case 13:
        return "$t5";
    case 14:
        return "$t6";
    case 15:
        return "$t7";
    case 16:
        return "$s0";
    case 17:
        return "$s1";
    case 18:
        return "$s2";
    case 19:
        return "$s3";
    case 20:
        return "$s4";
    case 21:
        return "$s5";
    case 22:
        return "$s6";
    case 23:
        return "$s7";
    case 24:
        return "$t8";
    case 25:
        return "$t9";
    case 26:
        return "$k0";
    case 27:
        return "$k1";
    case 28:
        return "$gp";
    case 29:
        return "$sp";
    case 30:
        return "$fp";
    case 31:
        return "$ra";
    default:
        return "Unknown";
    }
}