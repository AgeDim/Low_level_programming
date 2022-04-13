section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, 60
    syscall 

string_length:
    xor rax, rax		;Очистка rax
    .loop:
    	cmp byte [rdi+rax], 0	;Сравниваем следующий с нулем
    	je .end		;Если равенство, прыгаем на end
    	inc rax		;При неравенстве увеличиваем счетчик
    	jmp .loop		;Возвращаемся в начало loop
    .end:
    	ret


; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax
    xor rdi, rdi
    inc rdi
    xor rax, rax
    inc rax
    syscall
    ret

; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rdx, 1
    mov rsi, rsp
    mov rdi, 1
    mov rax, 1
    syscall
	pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA
    call print_char
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    xor rax, rax
        mov r11, 10
        mov rax, rdi
        push r13
        mov r13, rsp
        dec rsp
        mov byte[rsp], 0 
    .loop:
        xor rdx, rdx
        div r11
        add rdx, '0'
        dec rsp
        mov byte[rsp], dl
        test rax, rax
        jnz .loop
    mov rdi, rsp
    call print_string
    mov rsp, r13
    pop r13
    ret
    


; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
        cmp rdi, 0
        jge .out
        push rdi 
        mov rdi, '-'
        call print_char
        pop rdi
        neg rdi
    .out:
        call print_uint
    ret


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    .loop:
		xor rax, rax
        mov al, byte[rdi]
        cmp al, byte[rsi]
        jne .false
        inc rdi
        inc rsi
        cmp al, 0
        je .true
        jmp .loop
    .true:
        xor rax, rax
        inc rax
        ret
    .false:
    	xor rax, rax
    	ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    xor rdi, rdi
    xor rdx, rdx
    inc rdx
    xor rax, rax
    push rax 
    mov rsi, rsp
    syscall
    pop rax
    ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
    xor rax, rax    
    xor rcx, rcx    
.loop:
    cmp rcx, rsi    ; сравниваем количество прочитанных символов с размером буфера
    jae .not_fit    ; если количество символов превышает размер буфера, то выходим
    push rdx        
    push rsi        
    push rdi        
    push rcx        
    call read_char  ; читаем символ
    pop rcx         
    pop rdi         
    pop rsi         
    pop rdx         
    cmp al, 0xA     ; сравниваем символ с 0xA
    je .symbol   
    cmp al, 0x9     ; сравниваем символ с 0x9
    je .symbol   
    cmp al, 0x20    ; сравниваем символ с 0x20
    je .symbol  
    cmp al, byte 0  ; проверяем на нуль-терминатор
    je .finish         ; если нуль-терминатор, то это конец слова
    mov [rdi + rcx], al    
    inc rcx         
    jmp .loop       
.not_fit:
    xor rax, rax    
    ret             
.symbol:
    cmp rcx, 0     
    je .loop        
.finish:
    mov [rdi + rcx], byte 0     
    mov rax, rdi    
    mov rdx, rcx    
    ret
 
; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
xor	rcx, rcx
	xor	rax, rax
	xor	r8, r8
	mov	r9, 10
	.loop:
		mov	r8b, byte [rdi + rcx]	
		jmp	.isdigit
	.sec_loop:
		sub	r8b, '0'			; отнимаем '0' от символа, чтобы получить число из ASCII
		mul	r9				; умножаем rax на 10, для смещения
		add	rax, r8				; добавляем число к rax
		inc	rcx
		jmp	.loop

	.isdigit:
		cmp	r8, 0				
		je	.finish	
		cmp	r8, '0'			; меньше ASCII символа '0'
		jb	.finish	
		cmp	r8, '9'			; либо больше ASCII символа '9'
		ja	.finish				; если да, выходим из функции
		jmp	.sec_loop				; иначе продолжаем выполнение цикла
	
	.finish:
		mov	rdx, rcx
		ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    xor rdx, rdx
	xor rsi, rsi
	mov r10b, byte[rdi]
	
	cmp r10b, '-'
	je .neg
	
	call parse_uint
	jmp .finish
	
	.neg:
		inc rdi
		mov r10b, byte[rdi+rsi]
		cmp r10b, 0x20
		je .finish
		cmp r10b, 0x9
		je .finish
		cmp r10b, 0x10
		je .finish
		call parse_uint
		test rdx, rdx
		jz .finish
		inc rdx
		neg rax
	.finish:
		ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    push rdi
    push rsi
    push rdx
    call string_length
    pop rdx
    pop rsi
    pop rdi
    cmp rax, rdx
    ja .false
    .loop:
        xor r11, r11
        mov r11b, byte[rdi]
        mov byte[rsi], r11b
        inc rsi
        inc rdi
        cmp r11b, 0
        je .exit
        jmp .loop
    .false:
        xor rax, rax
        ret
    .exit:
        ret

