#include <stdint.h>
#include "stm32l476xx.h"
extern void delay();
extern void GPIOINIT();
int pattern = 0;
int freqs[5] = {1,6,10,16,40};
void system_clock_config(int press_num){
	if(press_num == 0 || press_num == 3){
		// 1M Hz
		RCC->CR |= RCC_CR_HSION;
		while((RCC->CR & RCC_CR_HSIRDY) == 0);
		RCC->CFGR &= 0xfffff000;
		if(press_num == 0)
			RCC->CFGR |= 11<<4;
		RCC->CFGR |= 5;
	}
	else if(press_num == 1){
		// 6M Hz
		RCC->CFGR &= 0xfffff000;
		RCC->CR &= 0xFEFFFFFF;
		while((RCC->CR & RCC_CR_PLLRDY) > 0);
		RCC->PLLCFGR &= 0xf9ff808f;
		RCC->PLLCFGR |= 2; //set clock source
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_4;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_1;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_1;
		RCC->CR |= RCC_CR_PLLON;
		RCC->CR |= RCC_CR_HSION;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
		RCC->CFGR |= 15;
	}
	else if(press_num == 2){
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
	else if(press_num == 4){
		// 40M Hz
		RCC->CFGR &= 0xfffff000;
		RCC->CR &= 0xFEFFFFFF;
		while((RCC->CR & RCC_CR_PLLRDY) > 0);
		RCC->PLLCFGR &= 0xf9ff808f;
		RCC->PLLCFGR |= 2; //set clock source
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_2;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_4;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_1;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_1;
		RCC->CR |= RCC_CR_PLLON;
		RCC->CR |= RCC_CR_HSION;
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
		RCC->CFGR |= 15;
	}

}
void debounce(int freq){
	int k =0 ;
	for(int i=550*freq;i>=0;i--){
		k++;
	}
}
void read_button(){
	int button = (GPIOC->IDR & (1<<13)) >> 13;
	if(!button){

		debounce(freqs[pattern]);
		button = (GPIOC->IDR & (1<<13)) >> 13;
		if (!button){
			return;
		}
		debounce(freqs[pattern]);
	}
	else{
		return;
	}
	pattern++;
	pattern = pattern % 5;
	system_clock_config(pattern);
}

int main(){
	system_clock_config(0);
	GPIOINIT();
	while(1){
		GPIOB->ODR |= 8;
		delay();
		GPIOB->ODR &= 0;
		delay();
	}
}
