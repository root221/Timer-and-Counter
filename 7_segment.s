.syntax unified
.cpu cortex-m4
.thumb
.text
	.global MAX7219_send
	.global MAX7219_init
	.equ CLK,0x20
	.equ DATA,0x8
	.equ LOAD,0x10
	.equ GPIOB_BASE,0x48000400
	.equ BSRR_OFFSET,0x18
	.equ BRR_OFFSET,0x28
	.equ SCAN_LIMIT,0x0B
	.equ DISPLAY_TEST,0x0F
	.equ DECODE_MODE,0x09
	.equ INTENSY,0x0A
.func MAX7219_init
MAX7219_init:
	push {r4-r11,lr}
	ldr r0,=#DISPLAY_TEST
	ldr r1,=#0x00
	bl MAX7219_send
	ldr r0,=#DECODE_MODE
	ldr r1,=#0xFF
	bl MAX7219_send
	ldr r0,=#SCAN_LIMIT
	ldr r1,=#0x01
	bl MAX7219_send
	ldr r0,=#INTENSY
	ldr r1,=#0x08
	bl MAX7219_send
	movs r0, #0x0C
	movs r1, #0x00
	bl MAX7219_send
	pop {r4-r11,lr}
	bx lr
.endfunc

.func MAX7219_send
MAX7219_send: //r0 address r1 data
	push {r4-r11,lr}
	lsl r0,r0,#8
	add r0,r0,r1
	ldr r1,=#GPIOB_BASE
	ldr r2,=#LOAD
	ldr r3,=#DATA
	ldr r4,=#CLK
	ldr r5,=#BSRR_OFFSET
	ldr r6,=#BRR_OFFSET
	movs r7,#16
	MAX7219_send_loop:
	movs r8,#1
	sub r9,r7,#1
	lsl r8,r8,r9
	str r4,[r1,r6]
	tst r8,r0
	beq bit_not_set
	str r3,[r1,r5]
	b if_done
	bit_not_set:
		str r3,[r1,r6]
	if_done:
		str r4,[r1,r5]
		subs r7,r7,#1
		cmp r7,0
		bne MAX7219_send_loop
		str r2,[r1,r6]
		str r2,[r1,r5]
	pop {r4-r11,lr}
	bx lr
.endfunc
