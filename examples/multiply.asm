; init
mvi l,100
mvi a,0
mov m,a

; input factor1
in  0
mvi l,101
mov m,a

; input factor2
in  0
mvi l,102
mov m,a

; call multiply function
call mult
out 0
hlt

; multiply
mult: mvi l,102
mov a,m
cpi 0
jz end
dcr a
mov m,a
mvi l,100
mov a,m
mvi l,101
add m
mvi l,100
mov m,a
jmp mult
end:
mvi l,100
mov a,m
ret
