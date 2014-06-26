; init
MVI L,100
MVI A,0
MOV M,A

; input factor1
IN 0
MVI L,101
MOV M,A

; input factor2
IN 0
MVI L,102
MOV M,A

; call multiply function
CALL mult
OUT 0
HLT

; multiply
mult: MVI L,102
MOV A,M
CPI 0
JZ end
DCR A
MOV M,A
MVI L,100
MOV A,M
MVI L,101
ADD M
MVI L,100
MOV M,A
JMP mult
end:
MVI L,100
MOV A,M
RET
