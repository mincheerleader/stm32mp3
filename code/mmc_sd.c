#include "stm32f10x.h"
#include "mmc_sd.h"	
//#include "vs.h"
#include "stm32f10x_spi.h"
#include "bsp_usart2.h"
//SD SPI1
u8  SD_Type=0;//SD��������	
/*******************************************************************************
* Function Name  : SPI_ControlLine
* Description    : SPI1ģ���ʼ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_ControlLine(void)
{	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);  
}

/*******************************************************************************
* Function Name  : SPI_SetSpeed
* Description    : SPI1�����ٶ�Ϊ����
* Input          : u8 SpeedSet 
*                  ����ٶ���������0�������ģʽ����0�����ģʽ
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_SetSpeed(u8 SpeedSet)
{
  SPI_InitTypeDef  SPI_InitStructure;
  if(SpeedSet==SPI_SPEED_HIGH)//����
  {
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

  SPI_Init(SPI1, &SPI_InitStructure);
  /* SPI1 enable */
  SPI_Cmd(SPI1, ENABLE);
  }
  else//����
  {
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);
    /* SPI1 enable */
    SPI_Cmd(SPI1, ENABLE);	
  }
}
	 
/*******************************************************************************
* Function Name  : SPI_ReadWriteByte
* Description    : SPI1��дһ���ֽڣ�������ɺ󷵻ر���ͨѶ��ȡ�����ݣ�
* Input          : u8 TxData �����͵���
* Output         : None
* Return         : u8 RxData �յ�����
*******************************************************************************/
u8 SPI_ReadWriteByte(u8 TxData)
{      
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//�ȴ���������	
  SPI_I2S_SendData(SPI1, TxData);//����һ��byte
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);//�ȴ�������һ��byte
  return SPI_I2S_ReceiveData(SPI1); //�����յ�������
}

/*******************************************************************************
* Function Name  : SD_WaitReady
* Description    : �ȴ�SD��Ready
* Input          : None
* Output         : None
* Return         : u8 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_WaitReady(void)
{
  u8 r1;
  u16 retry=0;
  do
  {
    r1 = SPI_ReadWriteByte(0xFF);
    retry++;
    if(retry==0xfffe)
      return 1; 
  }while(r1!=0xFF); 
    return 0;
}	 
/*******************************************************************************
* Function Name  : SD_SendCommand
* Description    : ��SD������һ������
* Input          : u8 cmd   ���� 
*                  u32 arg  �������
*                  u8 crc   crcУ��ֵ
* Output         : None
* Return         : u8 r1 SD�����ص���Ӧ
*******************************************************************************/
u8 SD_SendCommand(u8 cmd, u32 arg,u8 crc)
{
  unsigned char r1;
  unsigned int Retry = 0;

  SD_CS_DISABLE();
  SPI_ReadWriteByte(0xff);//��߼����ԣ����û�������ЩSD�����ܲ�֧��
  SD_CS_ENABLE();//Ƭѡ���õͣ�ѡ��SD��
  
  /* ������������ */
  SPI_ReadWriteByte(cmd | 0x40);                      
  SPI_ReadWriteByte((u8)(arg >> 24));//����[31..24]
  SPI_ReadWriteByte((u8)(arg >> 16));//����[23..16]
  SPI_ReadWriteByte((u8)(arg >> 8));//����[15..8]
  SPI_ReadWriteByte((u8)arg);    //����[7..0]
  SPI_ReadWriteByte(crc);
    
  //�ȴ���Ӧ����ʱ�˳�
  while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
  {
    Retry++;
    if(Retry > 800)break; //����ʵ���ã�������Դ������
  }   
  //�ر�Ƭѡ
  SD_CS_DISABLE();
  //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
  SPI_ReadWriteByte(0xFF);

  //����״ֵ̬
  return r1;
}


/*******************************************************************************
* Function Name  : SD_SendCommand_NoDeassert
* Description    : ��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
* Input          : u8 cmd   ���� 
*                  u32 arg  �������
*                  u8 crc   crcУ��ֵ
* Output         : None
* Return         : u8 r1 SD�����ص���Ӧ
*******************************************************************************/
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg,u8 crc)
{
  unsigned char r1;
  unsigned int Retry = 0;

  SD_CS_DISABLE();
  SPI_ReadWriteByte(0xff);//��߼����ԣ����û�������ЩSD�����ܲ�֧��
  SD_CS_ENABLE();//Ƭѡ���õͣ�ѡ��SD��
  
  /* ������������ */
  SPI_ReadWriteByte(cmd | 0x40);                      
  SPI_ReadWriteByte((u8)(arg >> 24));//����[31..24]
  SPI_ReadWriteByte((u8)(arg >> 16));//����[23..16]
  SPI_ReadWriteByte((u8)(arg >> 8));//����[15..8]
  SPI_ReadWriteByte((u8)arg);    //����[7..0]
  SPI_ReadWriteByte(crc);
  
  //�ȴ���Ӧ����ʱ�˳�
  while((r1 = SPI_ReadWriteByte(0xFF))==0xFF)
  {
    Retry++;
    if(Retry > 600)break; //����ʵ���ã�������Դ������  
  }
  //������Ӧֵ
  return r1;
}

/*******************************************************************************
* Function Name  : SD_Init
* Description    : ��ʼ��SD��
* Input          : None
* Output         : None
* Return         : u8 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  99��NO_CARD
*******************************************************************************/
u8 SD_Init(void)
{
  u16 i;      // ����ѭ������
  u8 r1;      // ���SD���ķ���ֵ
  u16 retry;  // �������г�ʱ����
  u8 buff[6];
					 
  SPI_ControlLine(); 	//SPI�����ó�ʼ��
  SPI_SetSpeed(SPI_SPEED_LOW);
  SD_CS_ENABLE();
					  
  // ����ʱ���ȴ�SD���ϵ����
  for(i=0;i<0xf00;i++);

  //�Ȳ�������74�����壬��SD���Լ���ʼ�����
  for(i=0;i<10;i++)
  {
    SPI_ReadWriteByte(0xFF);//80clks
  }

  //-----------------SD����λ��idle��ʼ-----------------
  //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
  //��ʱ��ֱ���˳�
  retry = 0;
  do
  {
    //����CMD0����SD������IDLE״̬
    r1 = SD_SendCommand(CMD0, 0,0x95);
    retry++;
  }while((r1 != 0x01) && (retry<200));
  //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
  if(retry==200) return 1;   //��ʱ����1	
  
  //-----------------SD����λ��idle����-----------------	 
  //��ȡ��Ƭ��SD�汾��Ϣ
  r1 = SD_SendCommand_NoDeassert(CMD8, 0x1aa,0x87);	     
    //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
//	printf("RRr1=%d\t",r1);
  if(r1 == 0x05)
  {
     //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
     SD_Type = SD_TYPE_V1;	   
     //�����V1.0����CMD8ָ���û�к�������
     //Ƭѡ�øߣ�������������
     SD_CS_DISABLE();
     //�෢8��CLK����SD������������
     SPI_ReadWriteByte(0xFF);	  
     //-----------------SD����MMC����ʼ����ʼ-----------------	 
     //������ʼ��ָ��CMD55+ACMD41
     // �����Ӧ��˵����SD�����ҳ�ʼ�����
     // û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
     retry = 0;
     do
     {
       //�ȷ�CMD55��Ӧ����0x01���������
       r1 = SD_SendCommand(CMD55, 0, 0);
       if(r1 != 0x01)
         return r1;	  
       //�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������400��
       r1 = SD_SendCommand(ACMD41, 0, 0);
       retry++;
     }while((r1!=0x00) && (retry<400));
     // �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
     // ���л�Ӧ����SD����û�л�Ӧ����MMC��
        
     //----------MMC�������ʼ��������ʼ------------
     if(retry==400)
     {
        retry = 0;
        //����MMC����ʼ�����û�в��ԣ�
        do
        {
           r1 = SD_SendCommand(CMD1, 0, 0);
           retry++;
        }while((r1!=0x00)&& (retry<400));
        if(retry==400)return 1;   //MMC����ʼ����ʱ		    
        //д�뿨����
        SD_Type = SD_TYPE_MMC;
     }
     //----------MMC�������ʼ����������------------	    
     //����SPIΪ����ģʽ
     SPI_SetSpeed(SPI_SPEED_HIGH);   
     SPI_ReadWriteByte(0xFF);
        
     //��ֹCRCУ��	   
     r1 = SD_SendCommand(CMD59, 0, 0x95);
     if(r1 != 0x00)return r1;  //������󣬷���r1   	   
     //����Sector Size
     r1 = SD_SendCommand(CMD16, 512, 0x95);
     if(r1 != 0x00)return r1;//������󣬷���r1		 
     //-----------------SD����MMC����ʼ������-----------------

  }//SD��ΪV1.0�汾�ĳ�ʼ������	 
  //������V2.0���ĳ�ʼ��
  //������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
  else if(r1 == 0x01)
  {
    //V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
    buff[0] = SPI_ReadWriteByte(0xFF);  //should be 0x00
    buff[1] = SPI_ReadWriteByte(0xFF);  //should be 0x00
    buff[2] = SPI_ReadWriteByte(0xFF);  //should be 0x01
    buff[3] = SPI_ReadWriteByte(0xFF);  //should be 0xAA	    
    SD_CS_DISABLE();	  
    SPI_ReadWriteByte(0xFF);//the next 8 clocks			 
    //�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
    //if(buff[2]==0x01 && buff[3]==0xAA) //�粻�жϣ�����֧�ֵĿ�����
   // {	  
       retry = 0;
       //������ʼ��ָ��CMD55+ACMD41
       do
       {
    	  r1 = SD_SendCommand(CMD55, 0, 0);
    	  if(r1!=0x01)return r1;	   
    	  r1 = SD_SendCommand(ACMD41, 0x40000000, 1);
          if(retry>200)return r1;  //��ʱ�򷵻�r1״̬  
       }while(r1!=0);		  
       //��ʼ��ָ�����ɣ���������ȡOCR��Ϣ		   
       //-----------����SD2.0���汾��ʼ-----------
       r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
       if(r1!=0x00)return r1;  //�������û�з�����ȷӦ��ֱ���˳�������Ӧ��		 
       //��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
       buff[0] = SPI_ReadWriteByte(0xFF);
       buff[1] = SPI_ReadWriteByte(0xFF); 
       buff[2] = SPI_ReadWriteByte(0xFF);
       buff[3] = SPI_ReadWriteByte(0xFF);

       //OCR������ɣ�Ƭѡ�ø�
       SD_CS_DISABLE();
       SPI_ReadWriteByte(0xFF);

       //�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
       //���CCS=1��SDHC   CCS=0��SD2.0
       if(buff[0]&0x40)SD_Type = SD_TYPE_V2HC;    //���CCS	 
       else SD_Type = SD_TYPE_V2;	    
       //-----------����SD2.0���汾����----------- 
       //����SPIΪ����ģʽ
       SPI_SetSpeed(1);  
   // }	    
  }
    return r1;
}



/*******************************************************************************
* Function Name  : SD_ReceiveData
* Description    : ��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
* Input          : u8 *data(��Ŷ������ݵ��ڴ�>len)
*                  u16 len(���ݳ��ȣ�
*                  u8 release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�
* Output         : None
* Return         : u8 
*                  0��NO_ERR
*                  other��������Ϣ
*******************************************************************************/
u8 SD_ReceiveData(u8 *data, u16 len, u8 release)
{
  u16 retry;
  u8 r1;
  // ����һ�δ���
  SD_CS_ENABLE();
  //�ȴ�SD������������ʼ����0xFE
  retry = 0;										   
  do
  {
    r1 = SPI_ReadWriteByte(0xFF);
    retry++;
    if(retry>4000)  //4000�εȴ���û��Ӧ���˳�����(����ʵ����ԣ��˴���ö��Լ���
    {
      SD_CS_DISABLE();
        return 1;
    }
  }while(r1 != 0xFE);
		   
    //��ʼ��������
    while(len--)
    {
      *data = SPI_ReadWriteByte(0xFF);
      data++;
    }
    //������2��αCRC��dummy CRC��
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);
    //�����ͷ����ߣ���CS�ø�
    if(release == RELEASE)
    {
      //�������
      SD_CS_DISABLE();
      SPI_ReadWriteByte(0xFF);
    }											  					    
    return 0;
}

/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description    : ��SD����һ��block
* Input          : u32 sector ȡ��ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer)
{
  u8 r1;

  //����Ϊ����ģʽ
  //SPI_SetSpeed(0);
  //GPIO_SetBits(GPIOB,GPIO_Pin_1);
 if(SD_Type!=SD_TYPE_V2HC)
  {
     sector = sector<<9;//512*sector�����������ı߽�����ַ
  }
    
  //�������SDHC����sector��ַת��byte��ַ
//  sector = sector<<9;
  r1 = SD_SendCommand(CMD17, sector, 1);//������
  if(r1 != 0x00)return r1; 		   							  
  r1 = SD_ReceiveData(buffer, 512, RELEASE);		 

	if(r1==0)
			return 0;
	else
			return	r1;
}

/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description    : д��SD����һ��block
* Input          : u32 sector ������ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_WriteSingleBlock(u32 sector, const u8 *data)
{
  u8 r1;
  u16 i;
  u16 retry;

  //����Ϊ����ģʽ
  SPI_SetSpeed(SPI_SPEED_LOW);

  //�������SDHC����������sector��ַ������ת����byte��ַ
  if(SD_Type!=SD_TYPE_V2HC)
  {
     sector = sector<<9;//512*sector�����������ı߽�����ַ
  }

  r1 = SD_SendCommand(CMD24, sector, 0x00);
  if(r1 != 0x00)
  {
    return r1;  //Ӧ����ȷ��ֱ�ӷ���
  }
    
  //��ʼ׼�����ݴ���
  SD_CS_ENABLE();
  //�ȷ�3�������ݣ��ȴ�SD��׼����
  SPI_ReadWriteByte(0xff);
  SPI_ReadWriteByte(0xff);
  SPI_ReadWriteByte(0xff);
  //����ʼ����0xFE
  SPI_ReadWriteByte(0xFE);

  //��һ��sector������
  for(i=0;i<512;i++)
  {
     SPI_ReadWriteByte(*data++);
  }
  //��2��Byte��dummy CRC
  SPI_ReadWriteByte(0xff);
  SPI_ReadWriteByte(0xff);
    
  //�ȴ�SD��Ӧ��
  r1 = SPI_ReadWriteByte(0xff);
  if((r1&0x1F)!=0x05)
  {
     SD_CS_DISABLE();
     return r1;
  }
    
  //�ȴ��������
  retry = 0;
  while(!SPI_ReadWriteByte(0xff))//���Ա��ʱ�������߱�����
  {
     retry++;
     if(retry>65534)        //�����ʱ��д��û����ɣ������˳�
     {
        SD_CS_DISABLE();
        return 1;           //д�볬ʱ����1
     }
  }

  //д����ɣ�Ƭѡ��1
  SD_CS_DISABLE();
  SPI_ReadWriteByte(0xff);

  return 0;
}


/*******************************************************************************
* Function Name  : SD_ReadMultiBlock
* Description    : ��SD���Ķ��block
* Input          : u32 sector ȡ��ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte��
*                  u8 count ������count��block
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count)
{
  u8 r1;	 			 
  //SPI_SetSpeed(SPI_SPEED_LOW);//����Ϊ����ģʽ 
  
  if(SD_Type != SD_TYPE_V2HC)
    sector = sector<<9;//�������SDHC����������sector��ַ������ת����byte��ַ   
 // sector = sector<<9;//�������SDHC����sector��ַת��byte��ַ
 //SD_WaitReady();
 //�����������
  r1 = SD_SendCommand(CMD18, sector, 1);//������
  if(r1 != 0x00)return r1;	 
  do//��ʼ��������
  {
    if(SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00)
    {
       break;
    }
    buffer += 512;
  } while(--count);		 
  //ȫ��������ϣ�����ֹͣ����
  SD_SendCommand(CMD12, 0, 1);
  //�ͷ�����
  SD_CS_DISABLE();
  SPI_ReadWriteByte(0xFF);    
  if(count != 0)
    return count;   //���û�д��꣬����ʣ�����	 
  else 
    return 0;	 
}


/*******************************************************************************
* Function Name  : SD_WriteMultiBlock
* Description    : д��SD����N��block
* Input          : u32 sector ������ַ��sectorֵ���������ַ�� 
*                  u8 *buffer ���ݴ洢��ַ����С����512byte��
*                  u8 count д���block��Ŀ
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count)
{
  u8 r1;
  u16 i;	 		 
  SPI_SetSpeed(SPI_SPEED_LOW);//����Ϊ����ģʽ	 
  if(SD_Type != SD_TYPE_V2HC)
    sector = sector<<9;//�������SDHC����������sector��ַ������ת����byte��ַ  
  if(SD_Type != SD_TYPE_MMC) 
    r1 = SD_SendCommand(ACMD23, count, 0x01);//���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ����   
  r1 = SD_SendCommand(CMD25, sector, 0x01);//�����д��ָ��
  if(r1 != 0x00)return r1;  //Ӧ����ȷ��ֱ�ӷ���	 
  SD_CS_ENABLE();//��ʼ׼�����ݴ���   
  SPI_ReadWriteByte(0xff);//�ȷ�3�������ݣ��ȴ�SD��׼����
  SPI_ReadWriteByte(0xff);   
  //--------������N��sectorд���ѭ������
  do
  {
    //����ʼ����0xFC �����Ƕ��д��
     SPI_ReadWriteByte(0xFC);	  
     //��һ��sector������
     for(i=0;i<512;i++)
     {
        SPI_ReadWriteByte(*data++);
     }
     //��2��Byte��dummy CRC
     SPI_ReadWriteByte(0xff);
     SPI_ReadWriteByte(0xff);
        
     //�ȴ�SD��Ӧ��
     r1 = SPI_ReadWriteByte(0xff);
     if((r1&0x1F)!=0x05)
     {
        SD_CS_DISABLE();    //���Ӧ��Ϊ��������������ֱ���˳�
        return r1;
     }

     //�ȴ�SD��д�����
     if(SD_WaitReady()==1)
     {
        SD_CS_DISABLE();    //�ȴ�SD��д����ɳ�ʱ��ֱ���˳�����
        return 1;
     }	   
    }while(--count);//��sector���ݴ������
    
    //��������������0xFD
    r1 = SPI_ReadWriteByte(0xFD);
    if(r1==0x00)
    {
       count =  0xfe;
    }		   
    if(SD_WaitReady()) //�ȴ�׼����
    {
      SD_CS_DISABLE();
      return 1;  
    }
    //д����ɣ�Ƭѡ��1
    SD_CS_DISABLE();
    SPI_ReadWriteByte(0xff);  
    return count;   //����countֵ�����д����count=0������count=1
}
											 
/*******************************************************************************
* Function Name  : SD_Read_Bytes
* Description    : ��ָ������,��offset��ʼ����bytes���ֽ�
* Input          : u32 address ������ַ��sectorֵ���������ַ�� 
*                  u8 *buf     ���ݴ洢��ַ����С<=512byte��
*                  u16 offset  �����������ƫ����
                   u16 bytes   Ҫ�������ֽ���
* Output         : None
* Return         : u8 r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
u8 SD_Read_Bytes(unsigned long address,unsigned char *buf,unsigned int offset,unsigned int bytes)
{
  u8 r1;u16 i=0;  
  r1=SD_SendCommand(CMD17,address<<9,1);//���Ͷ���������      
  if(r1!=0x00)return r1;  //Ӧ����ȷ��ֱ�ӷ���
  SD_CS_ENABLE();//ѡ��SD��
  while (SPI_ReadWriteByte(0xff)!= 0xFE)//ֱ����ȡ�������ݵĿ�ʼͷ0XFE���ż���
  {
    i++;
    if(i>3000)
    {
      SD_CS_DISABLE();//�ر�SD��
      return 1;//��ȡʧ��
    }
  }; 		 
  for(i=0;i<offset;i++)
    SPI_ReadWriteByte(0xff);//����offsetλ 
  for(;i<offset+bytes;i++)
    *buf++=SPI_ReadWriteByte(0xff);//��ȡ��������	
  for(;i<512;i++) 
    SPI_ReadWriteByte(0xff); 	 //����ʣ���ֽ�
  SPI_ReadWriteByte(0xff);//����αCRC��
  SPI_ReadWriteByte(0xff);  
  SD_CS_DISABLE();//�ر�SD��
  return 0;
}
