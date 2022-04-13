extern string_equals

global find_word

%define offset 8

section .text

find_word:

.loop:
    push rdi
    push rsi 
    add rsi, offset  
    call string_equals
    pop rsi
    pop rdi
    test rax, rax
    jnz .word_found
    mov rsi, [rsi] 
    test rsi, rsi
    jz .word_not_found
    jmp .loop

.word_found:
    mov rax, rsi
    jmp .end 

.word_not_found: 
    mov rax, 0

.end:
ret
