#ifndef VM_H
#define VM_H
#endif

#define DEFAULT_STACK_SIZE 256
#define DEFAULT_PROG_SIZE 256
#define DEFAULT_NUM_REGS 0xF

#include <stdint.h>

typedef enum 
{
    /*
     * Operator format:
     *  - 1st nibble, operation (this enum)
     *  - 2nd nibble, reg1, first register argument
     *  - 3rd nibble, reg2, second register argument
     *  - 4th nibble, reg3, third register argument
     *  - 3rd + 4th nibble, 2nd byte, imm, special value usage depends on op
     */

    NOOP  = 0x0, // do nothing

    LOAD  = 0x1, // load imm into reg1
    ADD   = 0x2, // add reg2 + reg3 into reg1
    SUB   = 0x3, // subtract reg2 - reg3 into reg1
    CMP   = 0x4, // same as SUB

    CHNG  = 0x5, // change the instruction @imm to value of reg1

    PUSH  = 0x6, // push value at reg1 onto the stack
    POP   = 0x7, // pop stack into reg1

    JMPL  = 0x8, // jump pc to imm if value at reg1 < 0
    JMPG  = 0x9, // jump pc to imm if value at reg1 > 0
    JMPE  = 0xA, // jump pc to imm if value at reg1 = 0
    JMPU  = 0xB, // jump pc to imm unconditionally

    READ  = 0xC, // read 2 bytes from stdin to reg1

    PRNS  = 0xD, // print chars from stack from top until 0x0000, imm=1 for ints
    PRNT  = 0xE, // print chars from program from imm until first 0x0000

    HALT  = 0xF  // halt execution

} VM_CODE;

typedef struct
{
    // fifo socket fd and name
    int fd;
    const char * fifo;
    // registers
    int16_t regs[ DEFAULT_NUM_REGS ];
    // program array, contains all instructions to be executed
    uint16_t prog[ DEFAULT_PROG_SIZE ];
    // stack for pushin/popping arbitrary uint16_t
    uint16_t stack[ DEFAULT_STACK_SIZE ];
} VM;
