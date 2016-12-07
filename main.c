#include <stdint.h>
#include "stm32l476xx.h"
#define TIME_SEC 6.73
extern void delay();
extern void GPIOINIT();
extern void display(int val);
extern void MAX7219INIT();
int pattern = 0;

uint32_t TIM_ARR_VAL = 34118;
uint32_t TIM_PSC_VAL = 0;
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
	GPIOB->MODER &= 0xffffff3f;
	GPIOB->MODER |= 0x80;
}
void GPIO_init(){
	RCC->AHB2ENR = RCC->AHB2ENR | 0x6 ;
	//Set PB3,PB4,PB5 as output mode
	GPIOB->MODER = (GPIOB->MODER & 0xfffff03f) | 0xA80;
	GPIOB->OSPEEDR = GPIOB->OSPEEDR | 0x540;


	GPIOC->MODER = (GPIOC->MODER & 0xffff0000) | 0x5500;
	GPIOC->OSPEEDR = GPIOC->OSPEEDR | 0x5500;
	GPIOC->PUPDR = GPIOC->PUPDR | 0xAA;
}
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
int main(){
	system_clock_config();
	//GPIOINIT();
	GPIO_init();
	//MAX7219INIT();
	GPIO_init_AF();
	initailize_timer();
	//19113
	PWM_channel_init(17059);
	int array[16]={0,1,2,100,3,4,5,200,6,7,-1,-1,-1,-1,-1,-1};
	int freqs[8] = {38225,34047,30339,28636,25509,22726,20246,19108};
	int duty = 50;
	while(1){
		int press_num=-1;
		for(int i=3;i>=0;i--){
			int pos = i + 4;
			GPIOC->ODR = (1<< pos);
			int j;
			for(j=0;j<4;j++){
				if( ((GPIOC->IDR & 0xf )& (1<<j))!=0){
					press_num = array[i+j*4];
					break;
				}
			}
			if(press_num>=0){
				delay();
				if (press_num < 100){
					TIM2->CCR2 = (uint32_t)(freqs[press_num] * duty)/100;
					TIM2->ARR = (uint32_t)freqs[press_num];
					timer_start();
					if( ((GPIOC->IDR & 0xf )& (1<<j)) == 0)
						clear();
				}
				else if (press_num == 100){
					if (duty < 90){
						duty += 5;
					}
				}
				else if (press_num == 200){
					if (duty > 10){
						duty -= 5;
					}
				}
				break;
			}
		}
	}
}
