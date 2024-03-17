## Memory layout:

Total memory size is 2^16 B.

|system|code/user|......|stack|
|256B  |......................|

Interpreting starts at 0x100.

## Stack terminology:

Stack grows downwards in address space.

<--- growing direction
top next ....
---> address direction

## Numbers:

Numbers are stored little endian on stack and in bytecode.

Numbers with no prefix are treated as literals.

Numbers can have #s suffix which provides
bytes size of number.

## Instuction format:

SSOOOOOO

SS - operand size (2^ss)
OOOOOO - opcode

## Mnemonics:

PNK    - upon reaching this instruction VM panics
HLT    - safely halts the VM
LITs   - interprets next bytes as literals to push on stack
DUPs   - a -- a a
DRPs   - a --
SWPs   - a b -- b a
OVRs   - a b -- a b a
ADDs   - a b -- (a+b)
SUBs   - a b -- (a-b)
JMP    - jumps to addr from top
JLZs   - jumps to addr from top when next is negative
MULs   - a b -- (a*b)  // signed
SHRs   - a b -- (a>>b) // signed
SHLs   - a b -- (a<<b)
XORs   - a b -- (a^b)
ANDs   - a b -- (a&b)
ORRs   - a b -- (a|b)
STMs   - store value from next into address from top
LDMs   - load value from addres from top

s - size of operand: none (1), 2, 4, 8
addrs are of size 2

## Labels:

:label - defines a label
@label - pushes label addr onto stack

## Compiler head movement:

]offset - sets writehead to specified offset

## Memory mapped IO:

First 256B is reserved for system memory mapped devices.

### Device 0 - System

0000         - result code
0001 - 000f  - reserved

### Device 1 - Console I/O

0010         - flags
0011         - input byte
0012         - output byte
0013 - 001f  - reserved
