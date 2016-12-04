.syntax unified
.cpu cortex-m4
.thumb
.text
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOB_MODER, 0x48000400
	.equ GPIOC_MODER, 0x48000800
	.equ OSPEEDR, 0x48000810
	.equ DELAY, 140000
	.global delay
	.global GPIOINIT

.func delay
delay:
	push {r4-r11,lr}
	ldr r4,=DELAY
	delay_loop:
		bl read_button
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
