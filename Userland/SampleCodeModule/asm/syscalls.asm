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
GLOBAL createProcessFds
GLOBAL killProcess
GLOBAL setPriority
GLOBAL waitpid
GLOBAL toggleBlockProcess
GLOBAL getPid
GLOBAL yield
GLOBAL semInit
GLOBAL semOpen
GLOBAL semClose
GLOBAL semWait
GLOBAL semPost
GLOBAL openPipe
GLOBAL closePipe
GLOBAL unblockProcess
GLOBAL sleep
GLOBAL getNextPipeId
GLOBAL getFds
GLOBAL openSharedMemory
GLOBAL closeSharedMemory

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

createProcessFds:
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

waitpid:
    mov rax, 21
    int 80h
    ret

toggleBlockProcess:
    mov rax, 22
    int 80h
    ret

getPid:
    mov rax, 23
    int 80h
    ret

yield:
    mov rax, 24
    int 80h
    ret

semInit:
    mov rax, 25
    int 80h
    ret

semOpen:
    mov rax, 26
    int 80h
    ret

semClose:
    mov rax, 27
    int 80h
    ret

semWait:
    mov rax, 28
    int 80h
    ret

semPost:
    mov rax, 29
    int 80h
    ret

openPipe:
    mov rax, 30
    int 80h
    ret

closePipe:
    mov rax, 31
    int 80h
    ret

unblockProcess:
    mov rax, 32
    int 80h
    ret

sleep:
    mov rax, 33
    int 80h
    ret

getNextPipeId:
    mov rax, 34
    int 80h
    ret

getFds:
    mov rax, 35
    int 80h
    ret

openSharedMemory:
    mov rax, 36
    int 80h
    ret