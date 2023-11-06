GLOBAL write
GLOBAL read
GLOBAL getSeconds
GLOBAL clear
GLOBAL getInfoReg
GLOBAL setFontSize
GLOBAL getScreenResolution
GLOBAL drawRect
GLOBAL getTicks
GLOBAL getMemory
GLOBAL playSound
GLOBAL kaboom
GLOBAL setFontColor
GLOBAL getFontColor
GLOBAL malloc
GLOBAL free
GLOBAL getMemoryInfo
GLOBAL getProcessesInfo
GLOBAL freeProcessesInfo
GLOBAL createProcess
GLOBAL killProcess
GLOBAL setPriority

read:
    mov rax, 0
    int 80h
    ret

write:
    mov rax, 1
    int 80h
    ret

clear:
    mov rax, 2
    int 80h
    ret

getSeconds:
    mov rax, 3
    int 80h
    ret

getInfoReg:
    mov rax, 4
    int 80h
    ret

setFontSize:
    mov rax, 5
    int 80h
    ret

getScreenResolution:
    mov rax, 6
    int 80h
    ret

drawRect:
    mov rax, 7
    mov r10, rcx
    int 80h
    ret

getTicks:
    mov rax, 8
    int 80h
    ret

getMemory:
    mov rax, 9
    int 80h
    ret

playSound:
    mov rax, 10
    int 80h
    ret

setFontColor:
    mov rax, 11
    int 80h
    ret

getFontColor:
    mov rax, 12
    int 80h
    ret

malloc:
    mov rax, 13
    int 80h
    ret

free:
    mov rax, 14
    int 80h
    ret

getMemoryInfo:
    mov rax, 15
    int 80h
    ret

getProcessesInfo:
    mov rax, 16
    int 80h
    ret

freeProcessesInfo:
    mov rax, 17
    int 80h
    ret

createProcess:
    mov rax, 18
    int 80h
    ret

killProcess:
    mov rax, 19
    int 80h
    ret

setPriority:
    mov rax, 20
    int 80h
    ret