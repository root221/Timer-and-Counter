#include <stdint.h>
#include "stm32l476xx.h"
#include "core_cm4.h"
#include <string.h>
#define LCD_RSPin 1
#define LCD_RWPin 5
#define LCD_ENPin 6
void systick_init(){
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->LOAD = (uint32_t) 3000000;
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
	RCC->AHB2ENR = RCC->AHB2ENR | 0x7;

	GPIOC->MODER = (GPIOC->MODER & 0xf3ffffff);

	GPIOB->MODER =  (GPIOB->MODER & 0xffff0000) |0x5555 ;
	GPIOB->PUPDR = (GPIOB->PUPDR & 0xffff0000) | 0xAAAA;
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & 0xffff0000) | 0x5555;
	GPIOB->OTYPER = 0;

	GPIOA->MODER =  (GPIOA->MODER & 0xffff0000) |0x5555 ;
	GPIOA->PUPDR = (GPIOA->PUPDR & 0xffff0000) | 0xAAAA;
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0xffff0000) | 0x5555;
	GPIOA->OTYPER = 0;

}
void wait(){
	int k=0;
	for(int i=0;i<5500;i++){
		k = k + 1;
	}
}
int write_to_LCD(int input,int is_cmd){
    if(is_cmd==1)
    		GPIOA->BRR |= 1 <<(LCD_RSPin );
    	else
    		GPIOA->BSRR |= 1 << (LCD_RSPin);
    	GPIOA->BRR |= 1 <<(LCD_RWPin);
    GPIOB->BRR |= 0xff;
    	GPIOB->BSRR |= input;
    	GPIOA->BSRR |= 1 << (LCD_ENPin);
    	wait();
    	GPIOA->BRR |= 1<< (LCD_ENPin);
    	wait();
}
int offset = 16;
int addr=0;
int prefix = 0x80;
int t_prefix = 0xC0;
int counter = 0;
int count;
void SysTick_Handler(void){
	if(counter >= 16){
		prefix = 0xC0;
		t_prefix = 0x80;
	}
	else{
		prefix = 0x80;
		t_prefix = 0xC0;
	}
	if(addr == 14){
		write_to_LCD(prefix + addr,1);
		write_to_LCD(0x20,0);
		write_to_LCD(0x34,0);
		write_to_LCD(t_prefix ,1);
		write_to_LCD(0x37,0);
	}
	else if(addr == 15){
		write_to_LCD(prefix + addr,1);
		write_to_LCD(0x20,0);
		write_to_LCD(t_prefix ,1);
		write_to_LCD(0x34,0);
		write_to_LCD(0x37,0);
	}

	else{
		write_to_LCD(prefix + addr,1);
		write_to_LCD(0x20,0);
		write_to_LCD(0x34,0);
		write_to_LCD(0x37,0);
	}
	addr++;
	addr%=16;
	counter++;
	counter%=32;
	write_to_LCD(0x02,1);
}
void init_LCD(){
	write_to_LCD(0x38,1);//function setting 00110000
	write_to_LCD(0x06,1);
	write_to_LCD(0x0c,1); //display on  00001110
	write_to_LCD(0x01,1);//clear screen  00000001
	write_to_LCD(0x80,1);//MOVE to top left 0000 0010
}
void debounce(){
	int k =0 ;
	for(int i=5500;i>=0;i--){
		k++;
	}
}
int main(){
	system_clock_config();
	GPIO_init();
	init_LCD();
	systick_init();
	write_to_LCD(0x34,0);
	write_to_LCD(0x37,0);
}
