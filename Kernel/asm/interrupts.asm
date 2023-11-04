
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL forceTimerTick
GLOBAL forceChangeOfProcess
GLOBAL setup_stack

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _syscallHandler

GLOBAL _ex00Handler
GLOBAL _ex06Handler
GLOBAL _ex0DHandler
GLOBAL _ex0EHandler

EXTERN irqDispatcher
EXTERN syscallDispatcher
EXTERN exceptionDispatcher
EXTERN load_main
EXTERN schedule

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
	pushState ; Se cargan 15 registros en stack

	mov rsi, rsp
	add rsi, 15*8 ; RIP 
	mov rdx, rsp
	add rdx, 18*8 ; RSP 

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	popState
	; El iretq necesita que le manden los valores de RIP|CS|RFLAGS|SP|SS de userland por stack
	add rsp, 8
	push load_main
	iretq
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

setup_stack:				; RDI EL STACK - RSI CODE - RDX ARGS - R10 WRAPPER
	mov r8, rsp 			; preservo viejo RSP
	mov rsp, rdi
	push 0x0 				; el SS
	push rdi 				; el RSP
	push 0x202 				; el RFLAGS
	push 0x8 ; el CS

	push rcx 				; el RIP ahora es el wrapper.

	push 0x0 				; el RAX
	push 0x1 				; rbx
    push 0x2 				; rcx
    push 0x3 				; rdx
    push 0x4 				; rbp
    push rsi 				; RDI->PRIMER ARGUMENTO WRAPPER (code)
    push rdx 				; RSI->SEGUNDO ARGUMENTO WRAPPER. (args del code)
    push 0x7 				; r8
    push 0x8 				; r9
    push 0x9 				; r10
    push 0x10 				; r11
    push 0x11 				; r12
    push 0x12 				; r13
    push 0x13 				; r14
    push 0x14 				; r15
	mov rax, rsp 			; el RSP
	mov rsp, r8 			; restauro el RSP
	ret

;8254 Timer (Timer Tick)
_irq00Handler:

	pushState

	mov rdi, 0 ; pasaje de parametro
	call irqDispatcher

	mov rdi, rsp
	call schedule
	mov rsp, rax

	; Send EOI
	mov al, 20h
	out 20h, al

	popState

	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Syscall
_syscallHandler:
	pushState
	mov rbp, rsp

	push r9
	mov r9, r8
	mov r8, r10
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax
	call syscallDispatcher
	mov [aux], rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	mov rsp, rbp
	popState
	mov rax, [aux]
	iretq

;Zero Division Exception
_ex00Handler:
	exceptionHandler 0

;Invalid Op Code Exception
_ex06Handler:
	exceptionHandler 6

; General Protection
_ex0DHandler:
	exceptionHandler 13

; Page Fault
_ex0EHandler:
	exceptionHandler 14

haltcpu:
	cli
	hlt
	ret

forceTimerTick:
	int 20h
	ret

forceChangeOfProcess:
	mov rsp, rdi
	popState
	iretq

SECTION .bss
	aux resq 1