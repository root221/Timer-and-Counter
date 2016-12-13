#include <stdint.h>
#include "stm32l476xx.h"
#include "core_cm4.h"
extern void display(int key_value);
extern void MAX7219INIT();
volatile int state[3] = {0,1,2}; // 0 input, 1 count down, 2 alarm
volatile int current_state = 0;
volatile int sec = 10;
uint32_t TIM_ARR_VAL = 34118;
uint32_t TIM_PSC_VAL = 0;
void GPIO_init_AF(){
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_0;
}
void PWM_channel_init(){
	TIM2->CCR2 = (uint32_t)0;
	TIM2->CCMR1 |= 110 << 12; // PWN mode1
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM2->CR1 |= TIM_CR1_ARPE ;
	TIM2->CCER |= TIM_CCER_CC2E;
}
clear(){
	TIM2->CR1 &= 0xfffffffe;
	GPIOB->MODER &= 0xffffff7f;
	GPIOB->ODR = 0;
}
void initailize_timer(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= (!(1<<4));
	TIM2->ARR = (uint32_t)TIM_ARR_VAL;
	TIM2->PSC = (uint32_t)TIM_PSC_VAL;
	TIM2->EGR = TIM_EGR_UG;
}
void timer_start(){
	TIM2->CCR2 = (uint32_t)(30339 * 50)/100;
	TIM2->ARR = (uint32_t)30339;
	TIM2->CR1 |= 1;
	GPIOB->MODER &= 0xffffff3f;
	GPIOB->MODER |= 0x80;
}




void systick_init(){
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->LOAD = (uint32_t) 1000000;

}
void system_clock_config(){
		// 10M Hz
		RCC->CFGR &= 0xfffff000;
		RCC->CR &= 0xFEFFFFFF;
		while((RCC->CR & RCC_CR_PLLRDY) > 0);
		RCC->PLLCFGR &= 0xf9ff808f;
		RCC->PLLCFGR |= 2; //set clock source
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_1;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_2;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_1;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_1;
		RCC->CR |= RCC_CR_PLLON;
		RCC->CR |= RCC_CR_HSION;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
		RCC->CFGR |= 15;
}

void GPIO_init(){
	RCC->AHB2ENR = RCC->AHB2ENR | 0x6 ;
	//Set PB3,PB4,PB5 as output mode
	GPIOB->MODER = (GPIOB->MODER & 0xfffff03C) | 0x581;
	GPIOB->OSPEEDR = GPIOB->OSPEEDR | 0x541;


	GPIOC->MODER = (GPIOC->MODER & 0xf3ff0000) | 0x5500;
	GPIOC->OSPEEDR = GPIOC->OSPEEDR | 0x5500;
	GPIOC->PUPDR = GPIOC->PUPDR | 0xAA;
}
volatile int col = 0;
void SysTick_Handler(){
	if(sec==0 && current_state==1){
		current_state = 2;
		//TODO start alarm
		timer_start();
	}
	if(current_state == 1){
		sec--;
		display(sec);
		return;
	}

	NVIC->ICER[0] = (uint32_t) 0x3C0;
	col++;
	col %= 4;
	//GPIOC->ODR = 0;
	GPIOC->ODR = 1 << (col + 4);
	NVIC->ISER[0] = (uint32_t) 0x3C0;

	//EXTI->PR1 |= 0xf;
	//EXTI_Setup();


}

void debounce(){
	int k =0 ;
	for(int i=5500;i>=0;i--){
		k++;
	}
}
void EXTI_Setup(){

	RCC->APB2ENR |= 0x1;
	SYSCFG->EXTICR[0] = 0;
	SYSCFG->EXTICR[0] |= (uint32_t)0x2222; // 0 010 PC
	SYSCFG->EXTICR[3] = 0 ;
	SYSCFG->EXTICR[3] |= (uint32_t) 0x20 ;// PC13

	EXTI->IMR1 = (uint32_t) 0xF;
	EXTI->IMR1 |= 1 << 13;

	EXTI->RTSR1 = (uint32_t) 0xF;
	EXTI->FTSR1 = 1 << 13;
	NVIC->ISER[1] |= 1 << 8;
	NVIC_SetPriority(6, (uint32_t)5);
	NVIC_SetPriority(7, 5);
	NVIC_SetPriority(8, 5);
	NVIC_SetPriority(9, 5);
	NVIC_SetPriority(-1,6);
}
volatile int key = 0;
volatile int Array[16]={1,2,3,10,4,5,6,11,7,8,9,12,15,0,14,13};

void EXTI13_IRQHandler(void){
	if(current_state == 0){
		SysTick->LOAD = (uint32_t) 10000000;
		sec = key;
		current_state = 1; //count down

	}
	else if(current_state == 2){
		current_state = 0;
		SysTick->LOAD = (uint32_t) 1000000;
		//TODO close alarm
		clear();
	}
	EXTI->PR1 |= 1 << 13; // clear pending
}

void EXTI0_IRQHandler(void){
	if(current_state != 0){
		return;
	}
	int value = EXTI->PR1;
	if(value==1)
		value = 0;
	else if(value==2)
		value = 1;
	else if(value==4)
		value = 2;
	else if(value==8)
		value = 3;


	key = Array[4*value+col];
	display(key);
	debounce();
	EXTI->PR1 |= 0xf;

}

int main(){
	system_clock_config();
	GPIO_init();
	systick_init();
	GPIO_init_AF();
	PWM_channel_init();
	initailize_timer();
	MAX7219INIT();
	EXTI_Setup();

	while(1){


	}
}
