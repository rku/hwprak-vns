;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public
; License as published by the Free Software Foundation; either
; version 2 of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
;

; ***                                     ***
; *** the program from the first exercise ***
; ***                                     ***

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
