# Bad VM

## References

* [https://github.com/parrt/simple-virtual-machine-C](https://github.com/parrt/simple-virtual-machine-C)
* [https://en.wikibooks.org/wiki/Creating_a_Virtual_Machine/Register_VM_in_C](https://en.wikibooks.org/wiki/Creating_a_Virtual_Machine/Register_VM_in_C)

## Examples

* demo code: asm/demo.vm
* demo assembled: asm/output.vm
* assembler: asm/assemble.py

Redirect stderr to /dev/null to hide

### VM
#### shell 1
```
$ ./vm [fifo for read] [assembled program]
$ ./vm /tmp/fifo asm/output.vm 2>/dev/null
8
7
6
5
4
3
2
1

hello world!
waiting to read 16 bits from FIFO /tmp/fifo...
```
#### shell 2
```
echo FFFF > /tmp/fifo
```

### Assembler

```
$ cd vm/asm
$ pipenv shell
$ ./assemble.py demo.vm output.vm
-----   PROGRAM DUMP   -----
INS   REG1  REG2  REG3   IMM
----  ----  ----  ----  ----
LOAD   r1    r0    rA    #0A
LOAD   r2    r0    r1    #01
LOAD   rF    r0    r0    #00
LOAD   rE    r0    r0    #00
SUB    r0    r1    r2    #12
JMPE   r0    r0    rC    #0C
SUB    r0    r0    r2    #02
ADD    rD    rE    r0    #E0
PUSH   rF    r0    r0    #00
PUSH   rD    r0    r0    #00
PRNS   r0    r0    r1    #01
JMPG   r0    r0    r6    #06
LOAD   rA    r1    r2    #12
PRNT   rA    r0    r0    #00
READ   r3    r0    r0    #00
CHNG   r3    r0    r0    #00
JMPU   r0    r0    r0    #00
HALT   r0    r0    r0    #00
NOOP   r0    r6    r8    #68
NOOP   r0    r6    r5    #65
NOOP   r0    r6    rc    #6c
NOOP   r0    r6    rc    #6c
NOOP   r0    r6    rf    #6f
NOOP   r0    r2    r0    #20
NOOP   r0    r7    r7    #77
NOOP   r0    r6    rf    #6f
NOOP   r0    r7    r2    #72
NOOP   r0    r6    rc    #6c
NOOP   r0    r6    r4    #64
NOOP   r0    r2    r1    #21
NOOP   r0    r0    r0    #00
```
