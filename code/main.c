#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "mmc_sd.h"
#include "VS1053.h"
//#include "pwm.h"
//#include "touch.h"
#include "ini.h"
//#include "flac.h"
//#include "LCD.h"
//#include "play.h"
#include "file.h"
//#include "ds18b.h"
#include "bsp_usart2.h"
//flag== 1 IN 0 OUT 2 null 3 outfile 4 nextfile 5 nextscan 6lastscan
struct folder folderdata[256]={0};
struct file   filedata[256]={0};
short  int    point=0,pcount=0,count=0,folderstack=0;
extern u8 SD_DATA[512];
short pcm[48]=
{
	0x52,0x49,0x46,0x46,0xff,0xff,0xff,0xff,
	0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
	0x10,0x00,0x00,0x00,0x01,0x00,0x02,0x00,
	0x44,0xac,0x00,0x00,0x10,0xb1,0x02,0x00,
	0x04,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
};
//delay
void delay(u32 time)
{    
   u32 i=0;  
   while(time--)
   {
      i=10;  
      while(i--) ;    
   }
}

//clear data
void cleardata(void)
{
	int i=0,j=0;
	for(j=0;j<256;j++)
	{
		for(i=0;i<11;i++)
		{
			filedata[j].filename[i]=0x00;
		}
		filedata[j].fileSector=0x00;
		filedata[j].filecut   =0x00;
		filedata[j].filelong  =0x00;
		filedata[j].filenums  =0x00;
	}
}
//readfile

//MAIN
//extern u8 SD_DATA[512];
//u8 SD_DATA8[512]={0};
extern long int BytesPerSector;         
extern long int SectorsPerCluster;      
extern long int ReserveSectors;      
extern long int FatTableNums;              
extern long int SectorsPerFat;      
extern long int HiddenSector;
extern long int RootDir;
extern long int StartRootClusterNumber;
int main(void)
{
	int SD_Init_flag=0;
	long file_long=0;
	long file_addr=0;
//	short int flag=-1;
//	unsigned char i=0,j=0;
//	int i=0;
	int i=0;
	//drive//

	sys_ini();
	delay(30000);
	VS_Init();
//	USARTx_Config();
	delay(30000);
	mp3_Init();
	delay(30000);
	SD_Init_flag=SD_Init();
//	printf("SD_Init_flag=%d\t",SD_Init_flag);
	delay(30000);
	ini_FAT();	//0扇区读取以及根目录的读取
	delay(30000);
//	printf("m1\n\n");
//	printf("VS_Ram_Test()=%02x\n\n",VS_Ram_Test());
//	VS_Set_All();
//	VS_Reset_DecodeTime();
//	VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
	
	file_addr=filedata[4].fileSector;
	file_long=filedata[4].filelong;
	for(i=0;i<4096;i++)		//VS1053清缓存
		{		
				VS_XDCS_CLR;
				VS_XCS_SET;				
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));		
				spi_write(0x00);
				VS_XDCS_SET;
				VS_XCS_CLR;
		}	
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	while(1)
	{	
			play_music(	file_long,
									file_addr);

	}

	
}
