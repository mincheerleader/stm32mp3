//#include "bsp_SysTick.h"
#include  "VS1053.h"
#include "flac.h"
#include "bsp_usart2.h"
#include "string.h"
//VS1053Ĭ�����ò���
_vs1053_obj vsset=
{
	220,	//����:220
	6,		//�������� 60Hz
	15,		//�������� 15dB	
	10,		//�������� 10Khz	
	15,		//�������� 10.5dB
	0,		//�ռ�Ч��	
};

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
unsigned char SPI2_ReadWriteByte(unsigned char writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(VS_SPI,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(VS_SPI, writedat);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(VS_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(VS_SPI);
}

char spi_write(u8 TxData)
{      
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//�ȴ���������	
	SPI_I2S_SendData(SPI2, TxData);//����һ��byte
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//�ȴ�������һ��byte
	return SPI_I2S_ReceiveData(SPI2); //�����յ�������
}
////////////////////////////////////////////////////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������
u8 VS_SPI_ReadWriteByte(u8 data)
{			  	 
	return SPI2_ReadWriteByte(data);	  
}
static void SPI_SetSpeed(u8 SpeedSet)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	SPI_Cmd(SPI2,ENABLE); 
}

void VS_SPI_SpeedLow(void)
{								 
	SPI_SetSpeed(SPI_BaudRatePrescaler_32);//���õ�����ģʽ 
}

void VS_SPI_SpeedHigh(void)
{						  
	SPI_SetSpeed(SPI_BaudRatePrescaler_8);//���õ�����ģʽ		 
}
//��ʼ��VS1053��IO��	 
void VS_Init(void)
{
	
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(VS_SPIGPIO_CLK|VS_GPIO_DREQ_CLK|VS_GPIO_RST_CLK|VS_GPIO_XDCS_CLK, ENABLE);
	
 	GPIO_InitStructure.GPIO_Pin = VS_DREQ;				 //DREQ
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //����
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(VS_GPIO_DREQ_PORT, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = VS_RST;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_Init(VS_GPIO_RST_PORT, &GPIO_InitStructure);
	
	/*��ʼ��STM32 SPI2�ӿ�*/
	RCC_APB1PeriphClockCmd(VS_SPI_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_SCLK | VS_MISO | VS_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // �������
	GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = VS_XCS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // �������
	GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = VS_XDCS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // �������
	GPIO_Init(VS_GPIO_XDCS_PORT, &GPIO_InitStructure);
	
	/* SPI2 ���� */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(VS_SPI, &SPI_InitStructure);
	
	/* ʹ�� SPI2 */
	SPI_Cmd(VS_SPI, ENABLE); 
	SPI2_ReadWriteByte(0xff);//��������	
	
}	  
////////////////////////////////////////////////////////////////////////////////	 	  
//����λVS10XX
void VS_Soft_Reset(void)
{	 
	u8 retry=0;  				   
	while(VS_DREQ_IN==0); 					//�ȴ�������λ����	   
	VS_SPI_ReadWriteByte(0Xff);			//��������
	retry=0;
	while(VS_RD_Reg(SPI_MODE)!=0x0800)	// ������λ,��ģʽ  
	{
		VS_WR_Cmd(SPI_MODE,0x0804);		// ������λ,��ģʽ	    
		Delay_ms(2);//�ȴ�����1.35ms 
		if(retry++>100)break; 	    
	}	 		 
	while(VS_DREQ_IN==0);//�ȴ�������λ����	 
	retry=0;
	while(VS_RD_Reg(SPI_CLOCKF)!=0X9800)//����VS1053��ʱ��,3��Ƶ ,1.5xADD 
	{
		VS_WR_Cmd(SPI_CLOCKF,0X9800);	//����VS1053��ʱ��,3��Ƶ ,1.5xADD
		if(retry++>100)break; 	    
	}	 
	Delay_ms(20);
}
//Ӳ��λMP3
//����1:��λʧ��;0:��λ�ɹ�	   
u8 VS_HD_Reset(void)
{
	u8 retry=0;
	VS_RST_CLR;
	Delay_ms(20);
	VS_XDCS_SET;//ȡ�����ݴ���
	VS_XCS_SET;//ȡ�����ݴ���
	VS_RST_SET;	   
	while(VS_DREQ_IN==0&&retry<200)//�ȴ�DREQΪ��
	{
		retry++;
		Delay_us(50);
	};
	Delay_ms(20);	
	if(retry>=200)return 1;
	else return 0;	    		 
}
//���Ҳ��� 
void VS_Sine_Test(void)
{											    
	VS_HD_Reset();	 
	VS_WR_Cmd(0x0b,0X2020);	  //��������	 
 	VS_WR_Cmd(SPI_MODE,0x0820);//����VS10XX�Ĳ���ģʽ     
	while(VS_DREQ_IN==0);     //�ȴ�DREQΪ��
	//printf("mode sin:%x\n",VS_RD_Reg(SPI_MODE));
 	//��VS1053�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨VS1053�����������Ҳ���Ƶ��ֵ��������㷽����VS1053��datasheet
	VS_SPI_SpeedLow();//���� 
	VS_XDCS_CLR;//ѡ�����ݴ���
	VS_SPI_ReadWriteByte(0x53);
	VS_SPI_ReadWriteByte(0xef);
	VS_SPI_ReadWriteByte(0x6e);
	VS_SPI_ReadWriteByte(0x24);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_XDCS_SET;
    //�˳����Ҳ���
    VS_XDCS_CLR;//ѡ�����ݴ���
	VS_SPI_ReadWriteByte(0x45);
	VS_SPI_ReadWriteByte(0x78);
	VS_SPI_ReadWriteByte(0x69);
	VS_SPI_ReadWriteByte(0x74);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	Delay_ms(100);
	VS_XDCS_SET;		 

//  //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
//  VS_XDCS_CLR;//ѡ�����ݴ���      
//	VS_SPI_ReadWriteByte(0x53);
//	VS_SPI_ReadWriteByte(0xef);
//	VS_SPI_ReadWriteByte(0x6e);
//	VS_SPI_ReadWriteByte(0x44);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	Delay_ms(100);
// 	VS_XDCS_SET;
//    //�˳����Ҳ���
//    VS_XDCS_CLR;//ѡ�����ݴ���
//	VS_SPI_ReadWriteByte(0x45);
//	VS_SPI_ReadWriteByte(0x78);
//	VS_SPI_ReadWriteByte(0x69);
//	VS_SPI_ReadWriteByte(0x74);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	Delay_ms(100);
//	VS_XDCS_SET; 
}	 
//ram ���� 
//����ֵ:RAM���Խ��
// VS1053����õ���ֵΪ0x83FF����������;																			 
u16 VS_Ram_Test(void)
{ 
	VS_HD_Reset();     
 	VS_WR_Cmd(SPI_MODE,0x0820);// ����VS10XX�Ĳ���ģʽ
	while (VS_DREQ_IN==0); // �ȴ�DREQΪ��			   
 	VS_SPI_SpeedLow();//���� 
	VS_XDCS_CLR;  		    // xDCS = 1��ѡ��VS10XX�����ݽӿ�
	VS_SPI_ReadWriteByte(0x4d);
	VS_SPI_ReadWriteByte(0xea);
	VS_SPI_ReadWriteByte(0x6d);
	VS_SPI_ReadWriteByte(0x54);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	Delay_ms(150);  
	VS_XDCS_SET;
	return VS_RD_Reg(SPI_HDAT0);// VS1053����õ���ֵΪ0x83FF����������;   
}     					   
//��VS1053д����
//address:�����ַ
//data:��������
void VS_WR_Cmd(u8 address,u16 data)
{  
	while(VS_DREQ_IN==0);//�ȴ�����		  
	VS_SPI_SpeedLow();//���� 	   
	VS_XDCS_SET; 
	VS_XCS_CLR;	 
	VS_SPI_ReadWriteByte(VS_WRITE_COMMAND);//����VS10XX��д����
	VS_SPI_ReadWriteByte(address); 	//��ַ
	VS_SPI_ReadWriteByte(data>>8); 	//���͸߰�λ
	VS_SPI_ReadWriteByte(data);	 	//�ڰ�λ
	VS_XCS_SET;           
	VS_SPI_SpeedHigh();				//����	   
} 
//��VS10XXд����
//data:Ҫд�������
void VS_WR_Data(u8 data)
{
	VS_SPI_SpeedHigh();//����,��VS1003B,���ֵ���ܳ���36.864/4Mhz����������Ϊ9M 
	VS_XDCS_CLR;   
	VS_SPI_ReadWriteByte(data);
	VS_XDCS_SET;      
}         
//��VS10XX�ļĴ���           
//address���Ĵ�����ַ
//����ֵ��������ֵ
//ע�ⲻҪ�ñ��ٶ�ȡ,�����
u16 VS_RD_Reg(u8 address)
{ 
	u16 temp=0;    	 
  while(VS_DREQ_IN==0);//�ǵȴ�����״̬ 		  
	VS_SPI_SpeedLow();//���� 
	VS_XDCS_SET;       
	VS_XCS_CLR;      
	VS_SPI_ReadWriteByte(VS_READ_COMMAND);	//����VS10XX�Ķ�����
	VS_SPI_ReadWriteByte(address);       	//��ַ
	temp=VS_SPI_ReadWriteByte(0xff); 		//��ȡ���ֽ�
	temp=temp<<8;
	temp+=VS_SPI_ReadWriteByte(0xff); 		//��ȡ���ֽ�
	VS_XCS_SET;     
	VS_SPI_SpeedHigh();//����	  
   return temp; 
}  
//��ȡVS10xx��RAM
//addr��RAM��ַ
//����ֵ��������ֵ
u16 VS_WRAM_Read(u16 addr) 
{ 
	u16 res;			   	  
 	VS_WR_Cmd(SPI_WRAMADDR, addr); 
	res=VS_RD_Reg(SPI_WRAM);  
 	return res;
} 
//дVS10xx��RAM
//addr��RAM��ַ
//val:Ҫд���ֵ 
void VS_WRAM_Write(u16 addr,u16 val) 
{  		   	  
 	VS_WR_Cmd(SPI_WRAMADDR,addr);	//дRAM��ַ 
	while(VS_DREQ_IN==0); 				//�ȴ�����	   
	VS_WR_Cmd(SPI_WRAM,val); 		//дRAMֵ 
} 
//���ò����ٶȣ���VS1053��Ч�� 
//t:0,1,�����ٶ�;2,2���ٶ�;3,3���ٶ�;4,4����;�Դ�����
void VS_Set_Speed(u8 t)
{
	VS_WRAM_Write(0X1E04,t);		//д�벥���ٶ� 
}
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//������Ԥ��ֵ,�ײ�III
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//����Kbps�Ĵ�С
//����ֵ���õ�������
u16 VS_Get_HeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;  
 	HEAD0=VS_RD_Reg(SPI_HDAT0); 
  HEAD1=VS_RD_Reg(SPI_HDAT1);
  //printf("(H0,H1):%x,%x\n",HEAD0,HEAD1);
	switch(HEAD1)
	{        
			case 0x7665://WAV��ʽ
			case 0X4D54://MIDI��ʽ 
			case 0X4154://AAC_ADTS
			case 0X4144://AAC_ADIF
			case 0X4D34://AAC_MP4/M4A
			case 0X4F67://OGG
			case 0X574D://WMA��ʽ
			case 0X664C://FLAC��ʽ
			{
					//printf("HEAD0:%d\n",HEAD0);
					HEAD1=HEAD0*2/25;//�൱��*8/100
					if((HEAD1%10)>5)return HEAD1/10+1;//��С�����һλ��������
					else return HEAD1/10;
			}
			default://MP3��ʽ,�����˽ײ�III�ı�
			{
					HEAD1>>=3;
					HEAD1=HEAD1&0x03; 
					if(HEAD1==3)HEAD1=1;
					else HEAD1=0;
					return bitrate[HEAD1][HEAD0>>12];
			}
	}  
}
//�õ�ƽ���ֽ���
//����ֵ��ƽ���ֽ����ٶ�
u32 VS_Get_ByteRate(void)
{
	return VS_WRAM_Read(0X1E05);//ƽ��λ��
}
//�õ���Ҫ��������
//����ֵ:��Ҫ��������
u16 VS_Get_EndFillByte(void)
{
	return VS_WRAM_Read(0X1E06);//����ֽ�
}  
//����һ����Ƶ����
//�̶�Ϊ32�ֽ�
//����ֵ:0,���ͳɹ�
//		 1,VS10xx��ȱ����,��������δ�ɹ�����    
u8 VS_Send_MusicData(u8* buf)
{
	u8 n;
	if(VS_DREQ_IN!=0)  //�����ݸ�VS10XX
	{			   	 
		VS_XDCS_CLR;
		VS_XCS_SET;		
        for(n=0;n<32;n++)
		{
			VS_SPI_ReadWriteByte(buf[n]);	 			
		}
		VS_XDCS_SET; 
		VS_XCS_CLR;			
	}else return 1;
	return 0;//�ɹ�������
}
//�и�
//ͨ���˺����и裬��������л���������				
void VS_Restart_Play(void)
{
	u16 temp;
	u16 i;
	u8 n;	  
	u8 vsbuf[32];
	for(n=0;n<32;n++)vsbuf[n]=0;//����
	temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
	temp|=1<<3;					//����SM_CANCELλ
	temp|=1<<2;					//����SM_LAYER12λ,��������MP1,MP2
	VS_WR_Cmd(SPI_MODE,temp);	//����ȡ����ǰ����ָ��
	for(i=0;i<2048;)			//����2048��0,�ڼ��ȡSM_CANCELλ.���Ϊ0,���ʾ�Ѿ�ȡ���˵�ǰ����
	{
		if(VS_Send_MusicData(vsbuf)==0)//ÿ����32���ֽں���һ��
		{
			i+=32;						//������32���ֽ�
   			temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
 			if((temp&(1<<3))==0)break;	//�ɹ�ȡ����
		}	
	}
	if(i<2048)//SM_CANCEL����
	{
		temp=VS_Get_EndFillByte()&0xff;//��ȡ����ֽ�
		for(n=0;n<32;n++)vsbuf[n]=temp;//����ֽڷ�������
		for(i=0;i<2052;)
		{
			if(VS_Send_MusicData(vsbuf)==0)i+=32;//���	  
		}   	
	}else VS_Soft_Reset();  	//SM_CANCEL���ɹ�,�����,��Ҫ����λ 	  
	temp=VS_RD_Reg(SPI_HDAT0); 
    temp+=VS_RD_Reg(SPI_HDAT1);
	if(temp)					//����λ,����û�гɹ�ȡ��,��ɱ���,Ӳ��λ
	{
		VS_HD_Reset();		   	//Ӳ��λ
		VS_Soft_Reset();  		//����λ 
	} 
}
//�������ʱ��                          
void VS_Reset_DecodeTime(void)
{
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);//��������
	
	VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
}
//�õ�mp3�Ĳ���ʱ��n sec
//����ֵ������ʱ��
u16 VS_Get_DecodeTime(void)
{ 		
	u16 dt=0;	 
	dt=VS_RD_Reg(SPI_DECODE_TIME);      
 	return dt;
} 	    					  
//vs10xxװ��patch.
//patch��patch�׵�ַ
//len��patch����
void VS_Load_Patch(u16 *patch,u16 len) 
{
	u16 i; 
	u16 addr, n, val; 	  			   
	for(i=0;i<len;) 
	{ 
		addr = patch[i++]; 
		n    = patch[i++]; 
		if(n & 0x8000U) //RLE run, replicate n samples 
		{ 
			n  &= 0x7FFF; 
			val = patch[i++]; 
			while(n--)VS_WR_Cmd(addr, val);  
		}else //copy run, copy n sample 
		{ 
			while(n--) 
			{ 
				val = patch[i++]; 
				VS_WR_Cmd(addr, val); 
			} 
		} 
	} 	
} 		  	  
//�趨VS10XX���ŵ������͸ߵ���
//volx:������С(0~254)
void VS_Set_Vol(u8 volx)
{
    u16 volt=0; 			//�ݴ�����ֵ
    volt=254-volx;			//ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ 
	volt<<=8;
    volt+=254-volx;			//�õ��������ú��С
    VS_WR_Cmd(SPI_VOL,volt);//������ 
}
//�趨�ߵ�������
//bfreq:��Ƶ����Ƶ��	2~15(��λ:10Hz)
//bass:��Ƶ����			0~15(��λ:1dB)
//tfreq:��Ƶ����Ƶ�� 	1~15(��λ:Khz)
//treble:��Ƶ����  	 	0~15(��λ:1.5dB,С��9��ʱ��Ϊ����)
void VS_Set_Bass(u8 bfreq,u8 bass,u8 tfreq,u8 treble)
{
    u16 bass_set=0; //�ݴ������Ĵ���ֵ
    signed char temp=0;   	 
	if(treble==0)temp=0;	   		//�任
	else if(treble>8)temp=treble-8;
 	else temp=treble-9;  
	bass_set=temp&0X0F;				//�����趨
	bass_set<<=4;
	bass_set+=tfreq&0xf;			//��������Ƶ��
	bass_set<<=4;
	bass_set+=bass&0xf;				//�����趨
	bass_set<<=4;
	bass_set+=bfreq&0xf;			//��������    
	VS_WR_Cmd(SPI_BASS,bass_set);	//BASS 
}
//�趨��Ч
//eft:0,�ر�;1,��С;2,�е�;3,���.
void VS_Set_Effect(u8 eft)
{
	u16 temp;	 
	temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
	if(eft&0X01)temp|=1<<4;		//�趨LO
	else temp&=~(1<<5);			//ȡ��LO
	if(eft&0X02)temp|=1<<7;		//�趨HO
	else temp&=~(1<<7);			//ȡ��HO						   
	VS_WR_Cmd(SPI_MODE,temp);	//�趨ģʽ    
}	 

///////////////////////////////////////////////////////////////////////////////
//��������,��Ч��.
void VS_Set_All(void) 				
{			 
//	VS_Set_Vol(vsset.mvol);			//��������
//	VS_Set_Bass(vsset.bflimit,vsset.bass,vsset.tflimit,vsset.treble);  
//	VS_Set_Effect(vsset.effect);	//���ÿռ�Ч��
	VS_Set_Vol(220);			//��������
	VS_Set_Bass(6,15,10,15);  
	VS_Set_Effect(0);	//���ÿռ�Ч��
}


 void mp3_Init(void)
{
	//rst0;			             //Ӳ����λ
	VS_RST_CLR;
	delay_vs(30000);			   // ����2�������ʱ,����Ӳ����λ��ԭ�����Ա���Ҫ��ʱ����ϸ�ο������ֲ�
//	rst1;
	VS_RST_SET;
	delay_vs(30000);

	VS_SPI_SpeedLow();
//	command_write(0x00,0x08,0x97);			//������λ,��0�żĴ���д��0x0804   SM_SDINEWΪ1   SM_RESETΪ1
	VS_WR_Cmd(0x00,0x0804);
	delay_vs(30);
//	command_write(0x03,0x98,0x00);			//����ʱ�ӣ�SC��MULTΪ4��SC��ADDΪ3��SC_FREQΪ0
	VS_WR_Cmd(0x03,0x9800);
	delay_vs(30);
//	command_write(0x02,0x7a,0xf6);			//���øߵ���
	VS_WR_Cmd(0x02,0x7af6);
	delay_vs(30);
//	command_write(0x0b,0x40,0x40);			//����������(0x20Ϊ���˸о�)��������һ����,Ϊ32
	VS_WR_Cmd(0x0b,0x4040);
	delay_vs(30);
	VS_SPI_SpeedHigh();
	
	VS_XDCS_CLR;					          //��������sdi
//	spi_write(0xff);
//	spi_write(0xff);
//	spi_write(0xff);
//	spi_write(0xff);
	VS_SPI_ReadWriteByte(0xff);
	VS_SPI_ReadWriteByte(0xff);
	VS_SPI_ReadWriteByte(0xff);
	VS_SPI_ReadWriteByte(0xff);
	VS_XDCS_SET;
}		
/*--------------  ������VS1053�������� -------------------------*/
/*--------------  ���濪ʼ�����ֲ��Ų��� -------------------------*/
//#include "ff.h"
//#include "bsp_led.h"   
/*
************************************************************************
*						  FatFs
************************************************************************
*/
void play_music(	long file_long,
									long file_addr)
{
	long addr_count=0;
	int i=0;
	u8 music_DATA[512]={0};
	
	for(i=0;i<2048;i++)
		{		
				VS_XDCS_CLR;
				VS_XCS_SET;				
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));		
				spi_write(0x00);
				VS_XDCS_SET;
				VS_XCS_CLR;
		}
			
	for(addr_count=0;addr_count<file_long;addr_count++)
		{
				SD_ReadSingleBlock(file_addr,music_DATA);
		
				VS_XDCS_CLR;
				VS_XCS_SET;
					
				for(i=0;i<512;i++)
				{	
					while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));		
					spi_write(music_DATA[i]);
				}
				VS_XDCS_SET;
				VS_XCS_CLR;
				file_addr++;
		}
				
	for(i=0;i<2048;i++)
		{		
				VS_XDCS_CLR;
				VS_XCS_SET;				
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));		
				spi_write(0x00);
				VS_XDCS_SET;
				VS_XCS_CLR;
		}	

}




/*--------------  END OF FILE -----------------------*/