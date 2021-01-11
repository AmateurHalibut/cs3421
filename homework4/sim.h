#ifndef SIM_H
#define SIM_H

/*
 Stores instructions from a hex value
*/
union instruction
{
    unsigned int hex;
    struct
    {
        unsigned int func : 6;  // function code
        unsigned int shamt : 5; // shift ammount
        unsigned int rd : 5;
        unsigned int rt : 5;
        unsigned int rs : 5;
        unsigned int opcode : 6;
    } r_type; // r-type intruction

    struct
    {
        int immed : 16; // immediate value
        unsigned int rt : 5;
        unsigned int rs : 5;
        unsigned int opcode : 6;
    } i_type; // i-type intruction

    struct
    {
        unsigned int immed : 26; // immediate value
        unsigned int opcode : 6;
    } j_type; // j-type intruction
};

// Function prototypes
void printInstr(union instruction instruc, FILE **out);
char *getReg(unsigned int reg);
char *getRType(unsigned int func);

#endif