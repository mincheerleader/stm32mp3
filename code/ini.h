#ifndef __INI_H__
#define __INI_H__
void Driver_MCU_Init(void);
void GPIOA_ini(void);
void GPIOB_ini(void);
void GPIOC_ini(void);
void GPIOD_ini(void);
void uart_ini(void);
void send_data(u8 a);
void print(char *z);
void putnum(long int x);
void adc_ini(void);
void sys_ini(void);
#endif
