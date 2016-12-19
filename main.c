#include <stdint.h>
#include "stm32l476xx.h"
#include "core_cm4.h"
#define LCD_RSPin 13
#define LCD_RWPin 5
#define LCD_ENPin 6

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
	RCC->AHB2ENR = RCC->AHB2ENR | 0x7;

	GPIOC->MODER = (GPIOC->MODER & 0x03ffffff) | 0x54000000;
	GPIOC->PUPDR = (GPIOC->PUPDR & 0x03ffffff) | 0xA8000000;
	GPIOC->OSPEEDR = (GPIOC->OSPEEDR & 0x03ffffff) | 0x54000000;
	GPIOB->MODER =  (GPIOB->MODER & 0xffff0000) |0x5555 ;
	GPIOB->PUPDR = (GPIOB->PUPDR & 0xffff0000) | 0xAAAA;
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & 0xffff0000) | 0x5555;
	GPIOB->OTYPER = 0;

	GPIOA->MODER =  (GPIOA->MODER & 0xffff0000) |0x5555 ;
	GPIOA->PUPDR = (GPIOA->PUPDR & 0xffff0000) | 0xAAAA;
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0xffff0000) | 0x5555;
	GPIOA->OTYPER = 0;
	//GPIOB->ODR = 0xff;
}
int flag = 0;
void SysTick_Handler(){
	flag = !flag;
	GPIOB->ODR = flag << 3;
}
void wait(){
	int k=0;
	for(int i=0;i<5500;i++){
		k = k + 1;
	}
}
int write_to_LCD(int input,int is_cmd){
    if(is_cmd==1){
    		//GPIOC->BSRR &= 0xffffefff;
    		GPIOC->BRR |= 1 <<(LCD_RSPin );
    		//GPIOC->BSRR |= 1 << (LCD_RSPin - 1);

    		GPIOA->BRR |= 1 <<(LCD_RWPin);

    		/*for(int index=0;index<8;index++){
    			int set = input & (1 << index);
    			if(set){
    				GPIOA->BSRR |= (1<<index);
    			}
    			else
    				GPIOA->BRR |= (1<<index);
    		}*/
    		GPIOB->BRR |= 0xff;
    		GPIOB->BSRR |= input;
    		GPIOA->BSRR |= 1 << (LCD_ENPin);
    		wait();

    		GPIOA->BRR |= 1<< (LCD_ENPin);
    		wait();
    }

}

void init_LCD(){

//	write_to_LCD(0x30,1);
//	write_to_LCD(0x30,1);
//	write_to_LCD(0x30,1);
//	write_to_LCD(0x20,1);
	write_to_LCD(0x38,1);//function setting 00111000
	write_to_LCD(0x06,1);
	write_to_LCD(0x0f,1); //display on  00001110

    write_to_LCD(0x01,1);//clear screen  00000001


  //  write_to_LCD(0x80,1);//MOVE to top left 0000 0010
}




int main(){
//	system_clock_config();
	GPIO_init();
	init_LCD();
	//systick_init();
	//GPIOC->BSRR |= 1 <<(LCD_RSPin - 1);
    while(1){

    }
}
/*
PB 3 3.3
PB 4 3.3
PB 5 3.3
PB 2 3.3
PB 1 3.3
PB 0 3.3
PB 6 3.3
PB 7 3.3
*/
