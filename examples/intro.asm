jmp 0x10        ; as required by the exercise

; ***                                     ***
; *** the program from the first exercise ***
; ***                                     ***

.offset 0x10    ; as required by the exercise
mvi l, 0x1f     ; set L to 0x1f
mvi a, 0x76     ; set accu to 0x76
mov m, a        ; move accu to mem addr L
inr l           ; increment L
mov a, l        ; move L to accu
cpi 0xff        ; compare accu with 0xff
jnz 0x12        ; jump to addr 0x12 if zero flag unset
mvi a, 0x76     ; move 0x76 to accu
sta 0x10        ; store accu at 0x10
hlt             ; halt
