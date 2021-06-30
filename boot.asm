ORG 0
BITS 16

_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:step2

handle_zero:
    mov ah, 0eh
    mov al, '0'
    int 10h
    ret

handle_one:
    mov ah, 0eh
    mov al, '1'
    int 10h
    ret

step2:
    cli
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti

    mov word [ss:0x00], handle_zero
    mov word [ss:0x02], 0x7c0

    mov word [ss:0x04], handle_one
    mov word [ss:0x06], 0x7c0

    int 0x0
    int 0x1
    mov si, message
    call print
    jmp $

print:
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop

.done:
    ret
    
print_char:
    mov ah, 0eh
    int 10h
    ret

message: db 'Hello World!', 0

times 510 - ($ - $$) db 0
dw 0xAA55