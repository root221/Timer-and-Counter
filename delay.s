.syntax unified
.cpu cortex-m4
.thumb
.text
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOC_MODER, 0x48000800
	.equ BSRR_OFFSET, 0x18
  	.equ BRR_OFFSET, 0x28
    	.equ cs, 0x10  //gpiob pin4 0b10000
    	.equ din, 0x8  //pb pin3 0b1000
    	.equ clk, 0x20 //pb pin5 0b100000
    	.equ DISPLAY_TEST, 0x0F
    	.equ SCAN_LIMIT, 0x0B
    	.equ DECODE_MODE, 0x09
    	.equ INTENSITY, 0x0A
    	.equ SHUTDOWN, 0x0C
    	.equ DIGIT0, 0x01
	.equ OSPEEDR, 0x48000810
	.equ DELAY, 140000
	.global delay
	.global GPIOINIT
	.global display
	.global MAX7219INIT
.func delay
delay:
	push {r4-r11,lr}
	ldr r4,=DELAY
	delay_loop:
		//bl read_button
		subs r4,#1
		bne  delay_loop
	pop {r4-r11,lr}
	bx lr
.endfunc

.func GPIOINIT
GPIOINIT:
	push {r4-r11,lr}
	// clock enable b, c
	movs r0, 0x6
	ldr r1, =RCC_AHB2ENR
	str r0, [r1]
	// set PB 3, 4, 5 as output mode
	movs r0, #0x540
	ldr r1, =GPIOB_MODER
	ldr r2, [r1]
	and r2, #0xfffff03f
	orrs r2, r2, r0
	str r2, [r1]
	// set output speed for pb 3, 4, 5
	movs r0,  #0xA80
	ldr r1, =OSPEEDR
	strh r0, [r1]
	// set pc 13 ( user button ) as input
	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	and r2, #0xf3ffffff
	str r2, [r1]
	pop {r4-r11,lr}
	bx lr
.endfunc

.func display
display:
	push {r4-r11,lr}
	movs r6, r0
	movs r2, #10
	movs r11, #1 //counter
	next_bit:
		movs r0, r11
		sdiv r3, r6, r2
		mul r5, r3, r2
		subs r1, r6, r5
		cmp r11, #3
		it eq
		addeq r1, #0xF0
		BL MAX7219Send
		add r11, #1
		movs r6, r3
		cmp r6, #0
		bgt next_bit
		//ble return
	pop {r4-r11,lr}
	bx lr
.endfunc
.func MAX7219INIT
MAX7219INIT:
	push {r4-r11,lr}
	ldr r0, =#DECODE_MODE
	ldr r1, =#0xFF
	BL MAX7219Send
	ldr r0, =#DISPLAY_TEST
	ldr r1, =#0x00
	BL MAX7219Send
	ldr r0, =#SCAN_LIMIT
	ldr r1, =0x07
	BL MAX7219Send
	ldr r0, =#INTENSITY
	ldr r1, =#0x0f
	BL MAX7219Send
	ldr r0, =#SHUTDOWN
	ldr r1, =#0x01
	BL MAX7219Send
	//ldr r0, =DIGIT0
	//movs r1, #1
	movs r0, #3
	movs r1, #0x80
	BL MAX7219Send
	movs r0, #1
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #2
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #4
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #5
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #6
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #7
	movs r1, #0x0F
	BL MAX7219Send
	movs r0, #8
	movs r1, #0x0F
	BL MAX7219Send
	pop {r4-r11,pc}
.endfunc
MAX7219Send:
	push {r0, r1, r2, r3, r4-r11, lr}
    lsl r0, r0, #8
    add r0, r0, r1
    ldr r1, =GPIOB_MODER
    ldr r2, =#cs
    ldr r3, =#din
    ldr r4, =#clk
    ldr r5, =#BSRR_OFFSET
    ldr r6 , =#BRR_OFFSET
    movs r7, #16
    send_loop:
		movs r8, #1
		sub r9, r7, #1
		lsl r8, r8, r9
		str r4, [r1, r6] // reset
		tst r0, r8
		beq not_set
		str r3, [r1, r5] // set din
		b if_done
	not_set:
		str r3, [r1, r6] // reset din
	if_done:
		str r4, [r1, r5] // set .... clk trigger
		subs r7, r7, #1
		bgt send_loop
		str r2, [r1, r6] // reset
		str r2, [r1, r5] // set .... cs trigger
    		pop {r0, r1, r2, r3, r4-r11, pc}
