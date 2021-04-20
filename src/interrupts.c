#include "stm32f0xx.h"
#include <stdbool.h>

void delay_ms(uint16_t ms) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; // вкл. тактирование
	TIM6->PSC = 8000 - 1;
	TIM6->ARR = ms;
	TIM6->EGR |= TIM_EGR_UG; // переинициализация
	TIM6->SR &= ~TIM_SR_UIF;
	TIM6->CR1 |= TIM_CR1_CEN;
	while (!(TIM6->SR & TIM_SR_UIF)) {
	}
	TIM6->CR1 &= ~TIM_CR1_CEN;
}

void init_button();
bool flag = false;

int main(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // вкл тактирование портов со светодиодами
	SET_BIT(GPIOC->MODER, GPIO_MODER_MODER7_0); // регистр альтернативной функции для синего светодиода
	CLEAR_BIT(GPIOC-> OTYPER, GPIO_OTYPER_OT_7); //подтягивающий резистор
	CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEEDR7_0); //регистр скорости

	SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_0); // зеленый светодиод
	CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_9);
	CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEEDR9_0);

	init_button();
	CLEAR_BIT(GPIOC->ODR, GPIO_ODR_9);

	while (1) {

		SET_BIT(GPIOC->ODR, GPIO_ODR_7);

		delay_ms(50);
		CLEAR_BIT(GPIOC->ODR, GPIO_ODR_7);
		delay_ms(50);

		if (flag) // по нажатию кнопки
			SET_BIT(GPIOC->ODR, GPIO_ODR_9);
		else
			CLEAR_BIT(GPIOC->ODR, GPIO_ODR_9);
	}
}
//обработчик прерываний
void EXTI0_1_IRQHandler() {
	if ((GPIOA->IDR & GPIO_IDR_0) == GPIO_IDR_0)
		flag = 1;
	else
		flag = 0;
	EXTI->PR |= EXTI_PR_PR0;
}

void init_button() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	NVIC_EnableIRQ(EXTI0_1_IRQn); //разрешить прерывание
	SET_BIT(EXTI->IMR, EXTI_IMR_MR0);
	SET_BIT(EXTI->RTSR, EXTI_RTSR_RT0); //верхний фронт
	SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0); //нижний фронт
}

