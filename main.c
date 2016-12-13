#include <stdint.h>
#include "stm32l476xx.h"
#include "core_cm4.h"
extern void display(int key_value);
extern void MAX7219INIT();
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
	GPIOB->MODER = (GPIOB->MODER & 0xfffff03f) | 0x540;
	GPIOB->OSPEEDR = GPIOB->OSPEEDR | 0x540;


	GPIOC->MODER = (GPIOC->MODER & 0xf3ff0000) | 0x5500;
	GPIOC->OSPEEDR = GPIOC->OSPEEDR | 0x5500;
	GPIOC->PUPDR = GPIOC->PUPDR | 0xAA;
}
volatile int col = 0;
void SysTick_Handler(){

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
	EXTI->IMR1 = (uint32_t) 0xF;
	//EXTI->PR1 = 0xf;
	EXTI->RTSR1 = (uint32_t) 0xF;
	NVIC_SetPriority(6, (uint32_t)5);
	NVIC_SetPriority(7, 5);
	NVIC_SetPriority(8, 5);
	NVIC_SetPriority(9, 5);

}
volatile int key = 0;
volatile int Array[15]={1,2,3,10,4,5,6,11,7,8,9,12,15,0,14,13};
void EXTI0_IRQHandler(void){
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
	//EXTI->IMR1 = 0;
	EXTI->PR1 |= 0xf;
	//NVIC->ICER[0] = 0x3c0;
	//NVIC->ICPR[0] = 0x3c0;
}

int main(){
	system_clock_config();
	GPIO_init();
	systick_init();
	MAX7219INIT();
	EXTI_Setup();

	while(1){


	}
}
