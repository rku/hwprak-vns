; the program from the first exercise
MVI L, 0x1f
MVI A, 0x76
MOV M, A
INR L
MOV A, L
CPI 0xff
JNZ 0x12
MVI A, 0x76
STA 0x10
HLT
