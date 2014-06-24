; another stuff
; MOV L, L

MVI L, 0x1f     ; do something
MVI A, 0x76
MOV M, A
INR L
MOV A, L
CPI 0xff
JNZ 0x12
MVI A, 0x76
STA 0x10
HLT
