#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
//#include "stm32f10x_adc.h"
#include "stm32f10x.h"
#include "ini.h"
//UART 2 
//RCC
void Driver_MCU_Init(void)
{
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	RCC_HSICmd(DISABLE);
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);  
	if(HSEStartUpStatus == SUCCESS)
	{
      
		/* HCLK = SYSCLK 72 */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
		/* PCLK2 = HCLK   72*/
		RCC_PCLK2Config(RCC_HCLK_Div1); 
 
		/* PCLK1 = HCLK/2   36*/
		RCC_PCLK1Config(RCC_HCLK_Div2);
	
		RCC_PREDIV2Config(RCC_PREDIV2_Div5); //PLL2CLK = (HSE / 5) * 8 = 40 MHz
    
		RCC_PLL2Config(RCC_PLL2Mul_8);  //PLL2CLK = (HSE / 5) * 8 = 40 MHz
    
		RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2,RCC_PREDIV1_Div5);  // PREDIV1CLK = PLL2 / 5 = 8 MHz
    
		// Enable PLL2
		RCC_PLL2Cmd(ENABLE);
		while((RCC->CR & RCC_CR_PLL2RDY) == 0)
		{
		}
      
		/* PLL configuration: PLLCLK = PREDIV1 * 9 = 72 MHz */ 
		RCC_PLLConfig(RCC_PLLSource_PREDIV1,RCC_PLLMul_9);

		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
 
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
 
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}
//GPIO
void GPIOB_ini()
{
	//LCD set and LED
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_DeInit(GPIOB);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure SPI2 pins:, SCK and MOSI */   //VS1053 SPI2
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 ;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//    
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14 ;//vsMISO
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure); 
//	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//VS_DREQ
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//PB10 VS_REST
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB12 pin: VS_XCS
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure SPI3 pins:, SCK and MOSI */    //touch SPI3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//T_MISO
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
void GPIOC_ini()
{
	//LCD DATA Line
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_DeInit(GPIOC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void GPIOA_ini()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_DeInit(GPIOA);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//VS_XDCS
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//UART
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure SPI1 pins:, SCK, MISO and MOSI */  //SD_SPI1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 ;//MISOӦ��Ҫ��ʼ��Ϊ������������ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	//PA4 pin:SD_CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //GPIO_Pin_0|GPIO_Pin_1| //dacʹ��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//PA1 pin:T_IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
}
void GPIOD_ini()
{
	//SPI3 TOUCH_CS
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_DeInit(GPIOD);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
//UART
void uart_ini()
{	
	USART_InitTypeDef USART_InitStrue;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitStrue.USART_BaudRate=115200;
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStrue.USART_Parity=USART_Parity_No;
	USART_InitStrue.USART_StopBits=USART_StopBits_1;
	USART_InitStrue.USART_WordLength=USART_WordLength_8b;
	
	USART_Init(USART2,&USART_InitStrue);
	USART_Cmd(USART2,ENABLE);
	
}
void send_data(u8 a)
{ 
	int flag1=0;
	while(flag1==0)
	{
		flag1=USART_GetFlagStatus(USART2,USART_FLAG_TXE);
	}
	    
	USART_SendData(USART2,a);
	USART_ClearFlag(USART2,USART_FLAG_TXE);						
}
void print(char *z)
{ 
	while(*z)
	{
		send_data(*z);
		z++;
	}
}
void putnum(long int x)
{
	int ge=0,shi=0,bai=0,qian=0,wan=0,shiwan=0,baiwan=0;
	ge=(x%10);
	shi=((((x%100000)%10000)%1000)%100)/10;
	bai=(((x%100000)%10000)%1000)/100;
	qian=((x%100000)%10000)/1000;
	wan=(x%100000)/10000;
	shiwan=x/100000;
	baiwan=x/1000000;
	send_data(baiwan+48);
	send_data(shiwan+48);
	send_data(wan+48);
	send_data(qian+48);
	send_data(bai+48);
	send_data(shi+48);
	send_data(ge+48);
}



//sys
void sys_ini(void)
{
	Driver_MCU_Init();
	GPIOA_ini();
	GPIOB_ini();
	GPIOC_ini();
	GPIOD_ini();
	//uart_ini();
}
