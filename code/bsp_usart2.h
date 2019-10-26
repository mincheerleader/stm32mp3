#ifndef __USART2_H
#define	__USART2_H


#include "stm32f10x.h"
#include <stdio.h>


#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART2
#define             macUSART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macUSART_CLK                             RCC_APB1Periph_USART2
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_2
#define             macUSART_RX_PORT                         GPIOA 
#define             macUSART_RX_PIN                          GPIO_Pin_3
#define             macUSART_IRQ                             USART2_IRQn
#define             macUSART_INT_FUN                         USART1_IRQHandler


void USARTx_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

void delay(u32 time);
#define Delay_us(x)	delay(x)       
#define Delay_ms(x) Delay_us(1000*x)	 //µ¥Î»ms
#define delay_vs(x) delay(x)

#endif /* __USART2_H */
