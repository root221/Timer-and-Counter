#include <stdint.h>
#include "stm32l476xx.h"
#include "core_cm4.h"
void systick_init(){
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->LOAD = (uint32_t) 5000000;
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
int flag = 0;
void SysTick_Handler(){
	flag = !flag;
	GPIOB->ODR = flag << 3;
}

void debounce(){
	int k =0 ;
	for(int i=5500;i>=0;i--){
		k++;
	}
}
int sysflag = 1;
void read_button(){
	int button = (GPIOC->IDR & (1<<13)) >> 13;
	if(!button){

		debounce();
		button = (GPIOC->IDR & (1<<13)) >> 13;
		if (!button){
			return;
		}
		debounce();
	}
	else{
		return;
	}

	if (sysflag){
		SysTick->CTRL &= 0xfffffffe;
		sysflag = 0;
	}
	else{
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		sysflag = 1;
	}
}
int main(){
	system_clock_config();
	GPIO_init();
	systick_init();
	while(1){
		read_button();
	}
}
