#include <stdint.h>
#include "stm32l476xx.h"
#define TIME_SEC 6.73
extern void delay();
extern void GPIOINIT();
extern void display(int val);
extern void MAX7219INIT();
int pattern = 0;
int freqs[5] = {1,6,10,16,40};
uint32_t TIM_ARR_VAL = 9999;
uint32_t TIM_PSC_VAL = 999;
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
void initailize_timer(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= (!(1<<4));
	TIM2->ARR = (uint32_t)TIM_ARR_VAL;
	TIM2->PSC = (uint32_t)TIM_PSC_VAL;
	TIM2->EGR = TIM_EGR_UG;
}
void timer_start(){
	TIM2->CR1 |= 1;
	int pre = 0;
	int sec = 0;
	while(1){
		int timer_value = TIM2->CNT;
		if (pre > timer_value){
			sec ++;
		}
		int limit = TIME_SEC * 10000;
		if (limit < (sec*10000 + timer_value)){
			display(limit / 100);
			break;
		}
		pre = timer_value;
		timer_value /= 100;
		display(sec*100 + timer_value);
	}
}
int main(){
	system_clock_config();
	GPIOINIT();
	MAX7219INIT();
	initailize_timer();
	timer_start();
}
