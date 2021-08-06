BITS 32

SECTION .data

testbyte:   db 01
testword:   dw 02
testdword:  dd 03
testdword2: dd 04

SECTION .text

call [jump_table + ecx * 4]         ; switch jump  FF 24 8D 48 80 40 00

nop
first_test:
offset_ft0:  dd first_instruction
offset_ft_1: dd jt_3
offset_ft_2: dd jt_2 
offset_ft_3: dd jt_3
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop

first_instruction:
mov  edx, [eax-8]
add [ebp + 100000 + eax * 2],bl       ; 00
add [ebx + 100000 + ecx * 4],bl       ; 00
add [ebp + 100000 + edx * 8],bl       ; 00
add [ebp + 100000 + esi * 2],bl       ; 00
add [ecx + 100000 + ebx * 4],bl       ; 00
add [edx + 100000 + edi * 8],bl       ; 00
add [ebp + 10 + eax * 8],bl       ; 00
add [testword],bx       ; 01
add bl,[testbyte]       ; 02
add bx,[testword]       ; 03
add al,19               ; 04
add eax,1999            ; 05
push es                 ; 06
pop es                  ; 07
or  al,ch               ; 08 
or  ax,cx               ; 09 
or  eax,ecx             ; 09 
or  al,[ebp]            ; 0A 
or  ax,[ebp]            ; 0B 
or  eax,[ebp]           ; 0B 
or  al,19               ; 0C 
or  ax,19               ; 0D 
or  eax,19              ; 0D 
push cs                 ; 0E 
sldt sp                 ; 0F 00 /0
str  ax                 ; 0F 00 /1
lldt bx                 ; 0F 00 /2
lldt [testbyte]         ; 0F 00 /2
ltr  cx                 ; 0F 00 /3
verr dx                 ; 0F 00 /4
verr bp                 ; 0F 00 /5
sgdt [testbyte]         ; 0F 01 /0
sidt [testbyte]         ; 0F 01 /1
lgdt [testbyte]         ; 0F 01 /2
lidt [testbyte]         ; 0F 01 /3
smsw dx                 ; 0F 01 /4
lmsw ax                 ; 0F 01 /6
invlpg [eax]            ; 0F 01 /7
lsl ecx,[testdword]     ; 0F 03
wbinvd                  ; 0F 09
femms                   ; OF OE     
mov eax,DR0             ; 0F 21
mov ebx,DR3             ; 0F 21
mov CR0,ecx             ; 0F 22
mov CR2,edx             ; 0F 22
mov DR0,esi             ; 0F 23
mov DR3,edi             ; 0F 23
mov eax,TR6             ; 0F 24
mov ebx,TR7             ; 0F 24
mov TR6,ecx             ; 0F 26
mov TR7,edx             ; 0F 26
wrmsr                   ; 0F 30
rdmsr                   ; 0F 32
ja NEAR near_target     ; 0F 87
seto  bh                ; 0F 90
setno cl                ; 0F 91
setb  ch                ; 0F 92
setae dl                ; 0F 93
sete  dh                ; 0F 94
setnz BYTE [testword]   ; 0F 95
setbe al                ; 0F 96
seta  ah                ; 0F 97
sets  bl                ; 0F 98
setns bh                ; 0F 99
seto  BYTE [testword]   ; 0F 9A
setnp al                ; 0F 9B
setl  ah                ; 0F 9C
setge bl                ; 0F 9D
setle bh                ; 0F 9E
setg  cl                ; 0F 9F
push fs                 ; 0F A0
pop fs                  ; 0F A1
shld [testbyte],ax,164  ; 0F A4
shld [testbyte],ebx,164 ; 0F A4
shld [testbyte],cx,cl   ; 0F A5
shld [testbyte],edx,cl  ; 0F A5
push gs                 ; 0F A8
pop gs                  ; 0F A9
rsm                     ; 0F AA
shrd [testbyte],ax,164  ; 0F AC
shrd [testbyte],ebx,164 ; 0F AC
shrd [testbyte],cx,cl   ; 0F AD
shrd [testbyte],edx,cl  ; 0F AD
fxsave       [jump_table + ecx * 4]         ; OF AE /0 
fxrstor      [jump_table + ecx * 4]         ; OF AE /1 
lss eax, [testbyte]     ; 0F B2
lfs eax, [testbyte]     ; 0F B4
lgs eax, [testbyte]     ; 0F B5
bsf eax,[testword]      ; 0F BC
movzx ax,[testword]     ; 0F B6
movzx ebx,BYTE [testword]                   ; 0F B6
movzx ecx,WORD [testword]                   ; 0F B7
movsx ax,[testword]     ; 0F BE
movsx eax,BYTE [testword]                   ; 0F BE
movsx eax,WORD [testword]                   ; 0F BE
xadd bl,ch              ; 0F C0
xadd bx,cx              ; 0F C1
xadd ebx,ecx            ; 0F C1
CMPXCHG8B [ebx]         ; 0F C7
push ss                 ; 16 
pop ss                  ; 17
sbb BYTE [testword],cl  ; 18 
sbb WORD [testword],cx                      ; 19 
sbb DWORD [testword],ecx                    ; 19 
sbb ch, BYTE [testword]                     ; 1A 
sbb cx, WORD [testword]                     ; 1B 
sbb ecx, DWORD [testword]                   ; 1B 
sbb al,28               ; 1C 
sbb ax,300              ; 1D 
sbb eax,300000          ; 1D 
push ds                 ; 1E 
pop ds                  ; 15
and eax,2147483648      ; 25
sub bl,ch               ; 28
sub cx,dx               ; 29
sub ecx,edx             ; 29
sub bl,[testbyte]       ; 2A
sub cx,[testbyte]       ; 2B
sub edx,[testbyte]      ; 2B
sub al,44               ; 2C
sub ax,444              ; 2D
sub eax,444444          ; 2D
xor [testword],bl       ; 30
xor bh,bl               ; 30
xor ecx,eax             ; 31
xor eax,ecx             ; 31
xor cl,[testword]       ; 32
xor bx,[ebp + 33]       ; 33
xor eax,[ebp + 33]      ; 33
xor al,52               ; 34
xor ax,13621            ; 35
xor eax,892679477       ; 35
aas                     ; 3F
dec esp                 ; 4C
push ax                 ; 50
push eax                ; 50
push cx                 ; 51
push ecx                ; 51
push dx                 ; 52
push edx                ; 52
push bx                 ; 53
push ebx                ; 53
push sp                 ; 54
push esp                ; 54
push bp                 ; 55
push ebp                ; 55
push si                 ; 56
push esi                ; 56
push di                 ; 57
push edi                ; 57
pop ax                  ; 58
pop eax                 ; 58
pop cx                  ; 59
pop ecx                 ; 59
pop dx                  ; 5A
pop edx                 ; 5A
pop bx                  ; 5B
pop ebx                 ; 5B
pop sp                  ; 5C
pop esp                 ; 5C
pop bp                  ; 5D
pop ebp                 ; 5D
pop si                  ; 5E
pop esi                 ; 5E
pop di                  ; 5F
pop edi                 ; 5F
pushaw                  ; 60
pushad                  ; 60
popaw                   ; 61
popad                   ; 61
bound ax,[testword]     ; 62
bound eax,[testdword]   ; 62
push WORD 900           ; 68
push 90000              ; 68
imul ebx,ebx,260        ; 69 
push BYTE 9             ; 6A
outsb                   ; 6E
outsw                   ; 6F
outsd                   ; 6F
add bl,10               ; 80 /0
or  bh,1                ; 80 /1
sbb ch,3                ; 80 /3
sub dl,5                ; 80 /5
xor dl,6                ; 80 /6
or  bx,10000            ; 81 /1
or  ebx,10000           ; 81 /1
sbb cx,33333            ; 81 /3
sbb ecx,33334           ; 81 /3
sub dx,33334            ; 81 /5
sub edx,33334           ; 81 /5
xor dx,33334            ; 81 /6
xor edx,33334           ; 81 /6
or  bx,BYTE 1           ; 83 /1
or  ebx,BYTE 1          ; 83 /1
sbb cx,BYTE 3           ; 83 /3
sbb ecx,BYTE 3          ; 83 /3
sub dx,BYTE 5           ; 83 /5
sub edx,BYTE 5          ; 83 /5
xor dx,BYTE 5           ; 83 /6
xor edx,BYTE 5          ; 83 /6
test bp,144             ; 84   
test ebp,14444          ; 85   
test esp,144444         ; 85   
xchg al,[testbyte]      ; 86
xchg [testbyte],al      ; 86
xchg bx,[testbyte]      ; 87
xchg [testbyte],ebx     ; 87
mov [testword],cl       ; 88
mov [testword],cx       ; 89
mov cl,[testword]       ; 8A
mov cx,[testword]       ; 8B
mov cx,ds               ; 8C
mov ecx,ds              ; 8C
mov ds,cx               ; 8E
mov ds,ecx              ; 8E
pop WORD [eax]          ; 8F /0
pop DWORD [ebp + 5]     ; 8F /0
pop DWORD [ebx]         ; 8F /0
nop                     ; 90
xchg cx,ax              ; 91
xchg dx,ax              ; 92
xchg bx,ax              ; 93
xchg sp,ax              ; 94
xchg bp,ax              ; 95
xchg si,ax              ; 96
xchg di,ax              ; 97
wait                    ; 9B
pushfw                  ; 9C
pushfd                  ; 9C
popfw                   ; 9D
popfd                   ; 9D
sahf                    ; 9E
lahf                    ; 9F
mov al,[testdword]      ; A0
mov ax,[testdword]      ; A1
mov eax,[testdword]     ; A1
mov [testdword],al      ; A2
mov [testdword],ax      ; A3
mov [testdword],eax     ; A3
movsb                   ; A4
movsw                   ; A5
movsd                   ; A5
test al,165             ; A8
test ax,43433           ; A9
test eax,2846468521     ; A9
stosb                   ; AA
stosw                   ; AB
stosd                   ; AB
lodsb                   ; AC
lodsw                   ; AD
scasb                   ; AE
scasw                   ; AF
scasd                   ; AF
mov al,5                ; B0
mov cl,6                ; B1
mov dl,7                ; B2
mov bl,8                ; B3
mov ah,9                ; B4
mov ch,10               ; B5
mov dh,11               ; B6
mov bh,12               ; B7
mov ax,505              ; B8
mov eax,505             ; B8
mov cx,506              ; B9
mov ecx,506             ; B9
mov dx,507              ; BA
mov edx,507             ; BA
mov bx,508              ; BB
mov ebx,508             ; BB
mov sp,509              ; BC
mov esp,509             ; BC
mov bp,510              ; BD
mov ebp,510             ; BD
mov si,511              ; BE
mov esi,511             ; BE
mov di,512              ; BF
mov edi,511             ; BF
rol BYTE  [testdword],20 ; CO /0
ror BYTE  [testdword],21 ; CO /1
rcl BYTE  [testdword],22 ; CO /2
rcr BYTE  [testdword],23 ; CO /3
sal BYTE  [testdword],23 ; CO /4
shr BYTE  [testdword],23 ; CO /5
sar BYTE  [testdword],23 ; CO /7
rol WORD  [testdword],20 ; C1 /0
rol DWORD [testdword],20 ; C1 /0
ror WORD  [testdword],21 ; C1 /1
ror DWORD [testdword],21 ; C1 /1
rcl WORD  [testdword],22 ; C1 /2
rcl DWORD [testdword],22 ; C1 /2
rcr WORD  [testdword],23 ; C1 /3
rcr DWORD [testdword],23 ; C1 /3
shl DWORD [testword],24  ; C1 /4
shl eax,25              ; C1 /5
ret 300                 ; C2      
les ax, [eax]           ; C4
les ax, [eax]           ; C4
les eax, [testbyte]     ; C4
lds ax, [ebx]           ; C5
lds eax, [testbyte]     ; C5
mov BYTE [testdword], 9 ; C6
mov WORD [testdword], 9 ; C7
mov DWORD [testdword],9 ; C7
retf 300                ; CA      
rol BYTE  [testdword],1 ; DO /0
ror BYTE  [testdword],1 ; DO /1
rcl BYTE  [testdword],1 ; DO /2
rcr BYTE  [testdword],1 ; DO /3
sal BYTE  [testdword],1 ; DO /4
shr BYTE  [testdword],1 ; DO /5
sar BYTE  [testdword],1 ; DO /7
rol WORD  [testdword],1 ; D1 /0
rol DWORD [testdword],1 ; D1 /0
ror WORD  [testdword],1 ; D1 /1
ror DWORD [testdword],1 ; D1 /1
rcl WORD  [testdword],1 ; D1 /2
rcl DWORD [testdword],1 ; D1 /2
rcr WORD  [testdword],1 ; D1 /3
rcr DWORD [testdword],1 ; D1 /3
shr WORD  [testdword],1 ; D1 /5
shr DWORD [testdword],1 ; D1 /5
rol BYTE  [testdword],cl ; D2 /0
ror BYTE  [testdword],cl ; D2 /1
rcl BYTE  [testdword],cl ; D2 /2
rcr BYTE  [testdword],cl ; D2 /3
sal BYTE  [testdword],cl ; D2 /4
shr BYTE  [testdword],cl ; D2 /5
shr BYTE  [testdword],cl ; D2 /7
rol WORD  [testdword],cl ; D3 /0
rol DWORD [testdword],cl ; D3 /0
ror WORD  [testdword],cl ; D3 /1
ror DWORD [testdword],cl ; D3 /1
rcl WORD  [testdword],cl ; D3 /2
rcl DWORD [testdword],cl ; D3 /2
rcr WORD  [testdword],cl ; D3 /3
rcr DWORD [testdword],cl ; D3 /3
shl WORD  [testdword],cl ; D3 /4
shl DWORD [testdword],cl ; D3 /4
shr WORD  [testdword],cl ; D3 /5
shr DWORD [testdword],cl ; D3 /5
sar WORD  [testdword],cl ; D3 /7
sar DWORD [testdword],cl ; D3 /7
xlatb                    ; D7
fadd st0,st1             ; D8 C1
fadd st0,st2             ; D8 C2
fadd st0,st3             ; D8 C3
fadd st0,st4             ; D8 C4
fadd st0,st5             ; D8 C5
fadd st0,st6             ; D8 C6
fadd st0,st7             ; D8 C7
fmul st0                 ; D8 C8
fmul st1                 ; D8 C9
fmul st2                 ; D8 CA
fmul st3                 ; D8 CB
fmul st4                 ; D8 CC
fmul st5                 ; D8 CD
fmul st6                 ; D8 CE
fmul st7                 ; D8 CF
fcom st0                 ; D8 D0
fcom st1                 ; D8 D1
fcom st2                 ; D8 D2
fcom st3                 ; D8 D3
fcom st4                 ; D8 D4
fcom st5                 ; D8 D5
fcom st6                 ; D8 D6
fcom st7                 ; D8 D7
fcomp st0                ; D8 D8
fcomp st1                ; D8 D9
fcomp st2                ; D8 DA
fcomp st3                ; D8 DB
fcomp st4                ; D8 DC
fcomp st5                ; D8 DD
fcomp st6                ; D8 DE
fcomp st7                ; D8 DF
fsub    st0, st1         ; D8 E1
fsub    st0, st2         ; D8 E2
fsub    st0, st3         ; D8 E3
fsub    st0, st4         ; D8 E4
fsub    st0, st5         ; D8 E5
fsub    st0, st6         ; D8 E6
fsub    st0, st7         ; D8 E7
fsubr   st0, st1         ; D8 E9
fsubr   st0, st2         ; D8 EA
fsubr   st0, st3         ; D8 EB
fsubr   st0, st4         ; D8 EC
fsubr   st0, st5         ; D8 ED
fsubr   st0, st6         ; D8 EE
fsubr   st0, st7         ; D8 EF
fdiv    st0, st1         ; D8 F1
fdiv    st0, st2         ; D8 F2
fdiv    st0, st3         ; D8 F3
fdiv    st0, st4         ; D8 F4
fdiv    st0, st5         ; D8 F5
fdiv    st0, st6         ; D8 F6
fdiv    st0, st7         ; D8 F7
fdivr   st0, st1         ; D8 F9
fdivr   st0, st2         ; D8 FA
fdivr   st0, st3         ; D8 FB
fdivr   st0, st4         ; D8 FC
fdivr   st0, st5         ; D8 FD
fdivr   st0, st6         ; D8 FE
fdivr   st0, st7         ; D8 FF
fadd  DWORD [jump_table + eax * 4]    ; D8 /0
fmul  DWORD [jump_table + ecx * 4]    ; D8 /1
fcom  DWORD [jump_table + ebx * 4]    ; D8 /2
fsub  DWORD [jump_table + ecx * 4]    ; D8 /4 
fsubr DWORD  [jump_table + ecx * 4]   ; D8 /5 
fdiv  DWORD [jump_table + ecx * 4]    ; D8 /6
fdivr DWORD [jump_table + edx * 4]    ; D8 /7
fld  st0                 ; D9 CO
fld  st1                 ; D9 C1
fld  st2                 ; D9 C2
fld  st3                 ; D9 C3
fld  st4                 ; D9 C4
fld  st5                 ; D9 C5
fld  st6                 ; D9 C6
fld  st7                 ; D9 C7
fxch st0          	 ; D9 C8
fxch st1                 ; D9 C9
fxch st2                 ; D9 CA
fxch st3                 ; D9 CB
fxch st4		 ; D9 CC
fxch st5		 ; D9 CD
fxch st6		 ; D9 CE
fxch st7		 ; D9 CF
fnop                     ; D9 D0
fchs                     ; D9 E0
fabs                     ; D9 E1 
ftst                     ; D9 E4 
fxam                     ; D9 E5
fld1                     ; D9 E8 
fldl2t                   ; D9 E9 
fldl2e                   ; D9 EA 
fldpi                    ; D9 EB 
fldlg2                   ; D9 EC 
fldln2                   ; D9 ED 
fldz                     ; D9 EE 
f2xm1                    ; D9 F0 
fyl2x                    ; D9 F1
fptan                    ; D9 F2 
fpatan                   ; D9 F3 
fxtract                  ; D9 F4
fprem1		         ; D9 F5
fdecstp                  ; D9 F6
fincstp                  ; D9 F7
fprem		         ; D9 F8		
fyl2xp1                  ; D9 F9
fsqrt                    ; D9 FA
fsincos                  ; D9 FB
frndint                  ; D9 FC
fscale                   ; D9 FD
fsin                     ; D9 FE
fcos                     ; D9 FF
fld  DWORD [jump_table + ecx * 4]      ; D9 /0
fst  DWORD [jump_table + edi * 4]      ; D9 /2
fstp DWORD [jump_table + esi * 4]      ; D9 /3
fldenv     [jump_table + ecx * 4]      ; D9 /4 
fldcw      [jump_table + ecx * 4]      ; D9 /5 
fstenv     [jump_table + eax * 4]      ; 9B D9 /6
fnstenv    [jump_table + ebx * 4]      ; D9 /6
fstcw      [jump_table + ecx * 4]      ; 9B D9 /7
fnstcw     [jump_table + edx * 4]      ; D9 /7
fucompp	                               ; DA E9  
fiadd DWORD [jump_table + eax * 4]     ; DA /0
fimul DWORD [jump_table + ebx * 4]     ; DA /1
ficom DWORD [jump_table + ecx * 4]     ; DA /2
ficomp DWORD [jump_table + edx * 4]    ; DA /3
fisub  DWORD [ebp + 100000 + eax * 2]  ; DA /4
fisubr DWORD [ebp + 100000 + ebx * 2]  ; DA /5
fidiv  DWORD [jump_table + ecx * 4]    ; DA /6
fidivr DWORD [jump_table + edx * 4]    ; DA /7
feni                     ; 9B DB E0
fneni                    ; DB E0
fdisi                    ; 9B DB E1
fndisi                   ; DB E1
fclex                    ; 9B DB E2
fnclex                   ; DB E2
finit                    ; 9B DB E3
fninit                   ; DB E3
fsetpm                   ; DB E4  
fucomi   st0             ; DB E8  
fucomi   st1             ; DB E9  
fucomi   st2             ; DB EA  
fucomi   st3             ; DB EB  
fucomi   st4             ; DB EC  
fucomi   st5             ; DB ED  
fucomi   st6             ; DB EE  
fucomi   st7             ; DB EF  
fild  DWORD [jump_table + ecx * 4]     ; DB /0
fist  DWORD [jump_table + ecx * 4]     ; DB /2 
fistp DWORD [jump_table + ecx * 4]     ; DB /3 
fld   TWORD [jump_table + ecx * 5]     ; DB /5
fstp  TWORD [jump_table + eax * 4]     ; DB /7
fadd st1,st0             ; DC C1
fadd st2,st0             ; DC C2
fadd st3,st0             ; DC C3
fadd st4,st0             ; DC C4
fadd st5,st0             ; DC C5
fadd st6,st0             ; DC C6
fadd st7,st0             ; DC C7
fmul st0,st0             ; DC C8
fmul st1,st0             ; DC C9
fmul st2,st0             ; DC CA
fmul st3,st0             ; DC CB
fmul st4,st0             ; DC CC
fmul st5,st0             ; DC CD
fmul st6,st0             ; DC CE
fmul st7,st0             ; DC CF
fsubr   st0, st0         ; DC E0
fsubr   st1, st0         ; DC E1
fsubr   st2, st0         ; DC E2
fsubr   st3, st0         ; DC E3
fsubr   st4, st0         ; DC E4
fsubr   st5, st0         ; DC E5
fsubr   st6, st0         ; DC E6
fsubr   st7, st0         ; DC E7
fsub    st0, st0         ; DC E8
fsub    st1, st0         ; DC E9
fsub    st2, st0         ; DC EA
fsub    st3, st0         ; DC EB
fsub    st4, st0         ; DC EC
fsub    st5, st0         ; DC ED
fsub    st6, st0         ; DC EE
fsub    st7, st0         ; DC EF
fdivr   st0, st0         ; DC F0
fdivr   st1, st0         ; DC F1
fdivr   st2, st0         ; DC F2
fdivr   st3, st0         ; DC F3
fdivr   st4, st0         ; DC F4
fdivr   st5, st0         ; DC F5
fdivr   st6, st0         ; DC F6
fdivr   st7, st0         ; DC F7
fdiv    st0, st0         ; DC F8
fdiv    st1, st0         ; DC F9
fdiv    st2, st0         ; DC FA
fdiv    st3, st0         ; DC FB
fdiv    st4, st0         ; DC FC
fdiv    st5, st0         ; DC FD
fdiv    st6, st0         ; DC FE
fdiv    st7, st0         ; DC FF
fadd  QWORD  [testbyte]               ; DC /0
fmul  QWORD [jump_table + ecx * 4]    ; DC /1
fcom  QWORD [jump_table + ecx * 4]    ; DC /2
fsub  QWORD [jump_table + ecx * 4]    ; DC /4 
fsubr QWORD  [jump_table + ecx * 4]   ; DC /5 
fdiv  QWORD [jump_table + ecx * 4]    ; DC /6
fdivr QWORD [jump_table + ecx * 4]    ; DC /7
ffree st0                ; DD C0
ffree st1                ; DD C1
ffree st2                ; DD C2
ffree st3                ; DD C3
ffree st4                ; DD C4
ffree st5                ; DD C5
ffree st6                ; DD C6
ffree st7                ; DD C7
fst  st0                 ; DD D0
fst  st1                 ; DD D1
fst  st2                 ; DD D2
fst  st3                 ; DD D3
fst  st4                 ; DD D4
fst  st5                 ; DD D5
fst  st6                 ; DD D6
fst  st7                 ; DD D7
fstp  st0                ; DD D8
fstp  st1                ; DD D9
fstp  st2                ; DD DA
fstp  st3                ; DD DB
fstp  st4                ; DD DC
fstp  st5                ; DD DD
fstp  st6                ; DD DE
fstp  st7                ; DD DF
fucom    st0             ; DD E0  
fucom    st1             ; DD E1  
fucom    st2             ; DD E2  
fucom    st3             ; DD E3  
fucom    st4             ; DD E4  
fucom    st5             ; DD E5  
fucom    st6             ; DD E6  
fucom    st7             ; DD E7  
fucomp   st0             ; DD E8  
fucomp   st1             ; DD E9  
fucomp   st2             ; DD EA  
fucomp   st3             ; DD EB  
fucomp   st4             ; DD EC  
fucomp   st5             ; DD ED  
fucomp   st6             ; DD EE  
fucomp   st7             ; DD EF  
fld  QWORD [jump_table + ecx * 4]     ; DD /0
fst QWORD [jump_table + ebp * 4]      ; DD /2
fstp QWORD [jump_table + esp ]        ; DD /3
frstor  [edx]                         ; DD /4	
fsave [jump_table + ecx * 4]          ; 9B DD /6		
fnsave  [ebx]                         ; DD /6		
fstsw [jump_table + edi * 4]          ; 9B DD /7
fnstsw [jump_table + esi * 4]         ; DD /7
faddp st1,st0           ; DE C1
faddp st2,st0           ; DE C2
faddp st3,st0           ; DE C3
faddp st4,st0           ; DE C4
faddp st5,st0           ; DE C5
faddp st6,st0           ; DE C6
faddp st7,st0           ; DE C7
fmulp st0               ; DE C8
fmulp st1               ; DE C9
fmulp st2               ; DE CA
fmulp st3               ; DE CB
fmulp st4               ; DE CC
fmulp st5               ; DE CD
fmulp st6               ; DE CE
fmulp st7               ; DE CF
fsubrp   st0            ; DE E0  
fsubrp   st1            ; DE E1  
fsubrp   st2            ; DE E2  
fsubrp   st3            ; DE E3  
fsubrp   st4            ; DE E4  
fsubrp   st5            ; DE E5  
fsubrp   st6            ; DE E6  
fsubrp   st7            ; DE E7  
fcompp                  ; DE D9
fsubp   st0             ; DE E8  
fsubp   st1             ; DE E9  
fsubp   st2             ; DE EA  
fsubp   st3             ; DE EB  
fsubp   st4             ; DE EC  
fsubp   st5             ; DE ED  
fsubp   st6             ; DE EE  
fsubp   st7             ; DE EF  
fdivrp  st0, st0        ; DE F0 
fdivrp  st1, st0        ; DE F1 
fdivrp  st2, st0        ; DE F2 
fdivrp  st3, st0        ; DE F3 
fdivrp  st4, st0        ; DE F4 
fdivrp  st5, st0        ; DE F5
fdivrp  st6, st0        ; DE F6 
fdivrp  st7, st0        ; DE F7 
fdivp   st0, st0        ; DE F8 
fdivp   st1, st0        ; DE F9 
fdivp   st2, st0        ; DE FA 
fdivp   st3, st0        ; DE FB 
fdivp   st4, st0        ; DE FC 
fdivp   st5, st0        ; DE FD 
fdivp   st6, st0        ; DE FE 
fdivp   st7, st0        ; DE FF 
fiadd  WORD [jump_table + eax * 4]     ; DE /0
fimul  WORD [jump_table + ebx * 4]     ; DE /1
ficom  WORD [jump_table + ecx * 4]     ; DE /2
ficomp WORD [jump_table + edx * 4]     ; DE /3
fisub  WORD  [ebp + 100000 + eax * 2]  ; DE /4
fisubr WORD  [ebp + 100000 + ebx * 2]  ; DE /5
fidiv   WORD [jump_table + ecx * 4]    ; DE /6
fidivr  WORD [jump_table + edx * 4]    ; DE /7
ffreep st0              ; DF C0
ffreep st1              ; DF C1
ffreep st2              ; DF C2
ffreep st3              ; DF C3
ffreep st4              ; DF C4
ffreep st5              ; DF C5
ffreep st6              ; DF C6
ffreep st7              ; DF C7
fstsw ax                ; 9B DF E0
fnstsw ax               ; DF E0 
fucomip  st0            ; DF E8  
fucomip  st1            ; DF E9  
fucomip  st2            ; DF EA  
fucomip  st3            ; DF EB  
fucomip  st4            ; DF EC  
fucomip  st5            ; DF ED  
fucomip  st6            ; DF EE  
fucomip  st7            ; DF EF  
fild  WORD [jump_table + ecx * 4]    ; DF /0
fist  WORD [jump_table + ecx * 4]    ; DF /2 
fistp  WORD [jump_table + ecx * 4]   ; DF /3 
fbld [ecx * 4]          ; DF /4
fild QWORD [jump_table + ecx * 4]    ; DF /5
fbstp [ecx * 4 + 999]   ; DF /6
fistp QWORD [jump_table + ecx * 4]   ; DF /7
out 255,al              ; E6
out 255,ax              ; E7
out 255,eax             ; E7
out dx,al               ; EE
out dx,ax               ; EF
out dx,eax              ; EF
lock nop                ; F0
not ah                  ; F6 /2
not bx                  ; F6 /2
not ecx                 ; F7 /2
not DWORD [testdword + edx * 2]      ; F7 /3
test bl,246                          ; F6 /0
neg bl                               ; F6 /3
neg BYTE [testdword + edi * 2]       ; F6 /3
test bx,63479                        ; F7 /0
test ecx,16250871                    ; F7 /0
neg WORD [testdword + eax * 4 + ebp] ; F7 /3
neg DWORD [testdword + edx * 2]      ; F7 /3
mul bl                               ; F6 /4
mul BYTE [testdword + esi * 2]       ; F6 /4
mul WORD [testdword + esi * 2]       ; F7 /4
mul DWORD [testdword + esi * 2]      ; F7 /4
mul esi                              ; F7 /4
stc                                  ; F9     
sti                                  ; FB     
std                                  ; FD     
push WORD [testdword + esi * 2]      ; FF /6
push DWORD [testdword + esi * 2]     ; FF /6

jmp  [first_test + ecx * 4]          ; switch jump  FF 24 8D 48 80 40 00
call [jump_table + ecx * 4]          ; switch jump  FF 24 8D 48 80 40 00
jmp  [0x408048 + ecx * 4]            ; switch jump  FF 24 8D 48 80 40 00

nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop                     ; 90
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
jt_1:
nop
nop
nop
jt_2:
nop
jt_3:
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
near_target:
nop
jump_table:
offset_fi :  dd first_instruction
offset_jt_1: dd jt_1
offset_jt_2: dd jt_2
offset_jt_3: dd jt_3

