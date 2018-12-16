/**
 * References: 
 *  - https://en.wikibooks.org/wiki/Creating_a_Virtual_Machine/Register_VM_in_C
 *
 * Notes:
 *  - Registers begin with r, r0, r1, r2
 *  - Immediate values begin with #, #100, #200
 *  - Memory addresses begin with @, @1000, @1004
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "vm.h"

int pc = 0; // program counter
int sp = 0; // stack pointer


// fetch the next word in the program
int fetch( VM *vm ) 
{
    return vm->prog[ pc++ ];
}

// Instruction fields
int instrNum = 0;
// operands
int reg1 = 0;
int reg2 = 0;
int reg3 = 0;
int imm = 0;

// decode a word
void decode( int instr ) 
{
    instrNum = (instr & 0xF000) >> 12;
    reg1 = (instr & 0x0F00) >> 8;
    reg2 = (instr & 0x00F0) >> 4;
    reg3 = (instr & 0x000F);
    imm = (instr & 0x00FF);
}

// the VM runs until this flag = 0
int running = 1;

// eval the last decoded instruction
void eval( VM *vm ) 
{
    switch ( instrNum ) 
    {
        case HALT:
            fprintf( stderr, "halt\n" );
            running = 0;
            break;
        case NOOP:
            fprintf( stderr, "noop\n" );
            break;
        case LOAD:
            fprintf( stderr, "load r%d #%d\n", reg1, imm );
            vm->regs[ reg1 ] = imm;
            break;
        case ADD:
            fprintf( stderr, "add r%d r%d r%d\n", reg1, reg2, reg3 );
            vm->regs[ reg1 ] = vm->regs[ reg2 ] + vm->regs[ reg3 ];
            break;
        case SUB:
            fprintf( stderr, "sub r%d r%d r%d\n", reg1, reg2, reg3 );
            vm->regs[ reg1 ] = vm->regs[ reg2 ] - vm->regs[ reg3 ];
            break;
        case CMP:
            fprintf( stderr, "cmp r%d r%d r%d\n", reg1, reg2, reg3 );
            vm->regs[ reg1 ] = vm->regs[ reg2 ] - vm->regs[ reg3 ];
            break;
        case CHNG:
            fprintf( stderr, "chng r%d, ins @%d\n", reg1, imm );
            vm->prog[ imm ] = vm->regs[ reg1 ];
            break;
        case PUSH:
            fprintf( stderr, "push r%d\n", reg1);
            vm->stack[ ++sp ] = vm->regs[ reg1 ];
            break;
        case POP:
            fprintf( stderr, "pop r%d\n", reg1 );
            vm->regs[ reg1 ] = vm->stack[ sp-- ];
            break;
        case JMPE:
            fprintf( stderr, "jmpe r%d @%d\n", reg1, imm );
            if ( vm->regs[ reg1 ] == 0 )
            {
                pc = imm;
            }
            break;
        case JMPL:
            fprintf( stderr, "jmpl r%d @%d\n", reg1, imm );
            if ( vm->regs[ reg1 ] < 0 )
            {
                pc = imm;
            }
            break;
        case JMPG:
            fprintf( stderr, "jmpg r%d @%d\n", reg1, imm );
            if ( vm->regs[ reg1 ] > 0 )
            {
                pc = imm;
            }
            break;
        case JMPU:
            fprintf( stderr, "pjmp pc=@%d\n", imm );
            pc = imm;
            break;
        case READ:
        {
            printf( "waiting to read 16 bits from FIFO %s...\n", vm->fifo );

            mkfifo( vm->fifo, 0666 );
            vm->fd = open( vm->fifo, O_RDONLY );
            char hex[5];
            int r = read( vm->fd, hex, 4 );
            close( vm->fd );
            hex[4] = 0;

            if ( r <= 0 )
            { // if there are errors, write NOOP
                vm->regs[ reg1 ] = 0x0000;
            }
            else
            {
                uint16_t num = strtol( hex, NULL, 16 );
                vm->regs[ reg1 ] = num;
            }
            break;
        }
        case PRNS:
        {
            char * fstr;
            if ( imm == 0 )
                fstr = "%c";
            else if ( imm == 1 )
                fstr = "%d";
            while ( sp > 0 && vm->stack[ sp ] != 0x0000 )
            {
                printf( fstr, vm->stack[ sp-- ] );
            }
            printf( "\n" );
            break;
        }
        case PRNT:
        {
            int16_t print_idx = vm->regs[ reg1 ];
            fprintf( stderr, "prnt r%d\n", reg1 );
            while ( vm->prog[ print_idx ] != 0x0000 )
            {
                printf( "%c", (uint8_t) vm->prog[ print_idx++ ] );
            }
            printf( "\n" );
            break;
        }

        default:
            printf( "op not implemented: %X\n", instrNum );
            break;
    }
}

void showRegs( VM *vm ) 
{
    fprintf( stderr, "regs = " );
    for (int i = 0; i < DEFAULT_NUM_REGS; i++ )
    {
        fprintf( stderr, "%04" PRIX16 " ", (uint16_t) vm->regs[ i ] );
    }
    fprintf( stderr, "\n" );
}

void run( VM *vm ) 
{
    while ( running ) 
    {
        showRegs( vm );
        int instr = fetch( vm );
        decode( instr );
        eval( vm );
    }
    showRegs( vm );
}

int main( int argc, const char * argv[] ) 
{
    VM *vm = calloc( 1, sizeof( VM ) );

    if ( argc < 2 )
    {
        printf( "Not implemented.\n" );
        return 1;
    }
    else 
    {
        FILE *fileptr;
        uint16_t *buffer;
        long filelen;
        
        fileptr = fopen(argv[ 2 ], "rb"); // Open the file in binary mode
        fseek(fileptr, 0, SEEK_END); // Jump to the end of the file
        filelen = ftell(fileptr); // Get the current byte offset in the file
        rewind(fileptr); // Jump back to the beginning of the file

        if ( filelen > 512 || filelen % 2 != 0 )
        { // program too long, addresses must be 0-FF uint16_t
          // or program not even
            return 1;
        }
        
        buffer = (uint16_t *)malloc((filelen / sizeof(uint16_t))*sizeof(uint16_t));
        fread(buffer, sizeof(uint16_t), filelen / sizeof(uint16_t), fileptr);
        fclose(fileptr); // Close the file

        for ( unsigned long i = 0; i < filelen / sizeof(uint16_t); i++ )
        {
            vm->prog[ i ] = buffer[ i ];
        }

        vm->fifo = argv[ 1 ];
    }
    run( vm );
    return 0;
}
