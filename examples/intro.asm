JMP 0x10        ; as required by the exercise

; ***                                     ***
; *** the program from the first exercise ***
; ***                                     ***

.offset 0x10    ; as required by the exercise
MVI L, 0x1f     ; set L to 0x1f
MVI A, 0x76     ; set accu to 0x76
MOV M, A        ; move accu to mem addr L
INR L           ; increment L
MOV A, L        ; move L to accu
CPI 0xff        ; compare accu with 0xff
JNZ 0x12        ; jump to addr 0x12 if zero flag unset
MVI A, 0x76     ; move 0x76 to accu
STA 0x10        ; store accu at 0x10
HLT             ; halt
