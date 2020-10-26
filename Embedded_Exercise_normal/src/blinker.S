
//ARM INSTRUCTIONS
//Instructions are conditional
//32-bit instruction


//How to load data from memory:
//LDR R0, =variable 	(Load address to register. You can use any global variable defined in main.c)
//LDR R1, [R0] 			(Load data from address, in this case 32-bit data)
//LDRB R1, [R0] 		(Load data from address, in this case 8-bit data)
//******
//LDR R0, =0x123456		(Load address to register, Use only if you know the memory address)
//LDR R1, [R0]			(load data from address, in this case 32-bit data)
//LDRB R1, [R0]			(load data from address, in this case 8-bit data)

//How to store data to memory:
//STR R1,[R0]			(R1 contains the data and R0 containts the address where  to store, 32-bit)
//STRB R1,[R0]			(R1 contains the data and R0 containts the address where  to store, 8-bit)


.text

.global blinker

blinker:

	push {r0, r1, r2, r3, r4}				//Push used register to stack

	ldr r0, =0x41200000
	ldr r1, =led_flags
	ldr r2, =led_dir
	ldrb r3, [r1]
	ldrb r4, [r2]
	
	strb r3, [r0]
	
	cmp r4, #0
	bne shift_right
	mov r3, r3, lsl #1
	b dir
shift_right:
	mov r3, r3, lsr #1
dir:
	cmp r3, #8
	beq dir_right
	cmp r3, #1
	bne end
	mov r4, #0
	b end
dir_right:
	mov r4, #1
end:
	strb r3, [r1]
	strb r4, [r2]

	pop {r4, r3, r2, r1, r0}				//Pop register from stack, Remember the pop order if used more than one register (first pushed, last popped)
	bx lr				//Return to place where this function have been called.
