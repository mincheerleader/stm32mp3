/*����Ӧ��
R1:����1��Ч��������Ӧ��󲿷������
7  6  5  4  3  2  1  0
0  |  |  |  |  |  |  |
   |  |  |  |  |  |  |______��ֹ״̬������ִ�д���
   |  |  |  |  |  |_________�߳���λ
   |  |  |  |  |____________�Ƿ�����
   |  |  |  |_______________CRC����
   |  |  |__________________�߳����д���
   |  |_____________________��ַ����
   |________________________��������

R1b����R1����ͬ�ĸ�ʽ��������˸��ӵ�æ�źš�æ�ź�ʱ����8λΪһ�飬ȫ0ʱΪæ����ȫ��ʱΪ׼����

д����Ӧ��:
7  6  5  4  3  2  1  0
x  x  x  0  Status   1
Status="010":����ȷ��  "101":����CRC��������  "110":����д���������

���ͺͽ��յ����ݸ�ʽ����1�����ڵ����������д�Ͷ�����StartBlock��0xfe��+UserData�����512�ֽڣ�+CRC��2Bytes��
                            ����ͨ������CMD12��ֹͣ��ȡ����
                      ��2�����ڶ��д��0xfc+UserData�����512�ֽڣ�+CRC��2Bytes)������StopTran��0xfd����ֹͣд������

������ʧ��ʱ�Ĵ������ݱ�ʶ����1Byte��7  6  5  4  3  2  1  0
                                     0  0  0  0  |  |  |  |___ִ�д���
                                                 |  |  |______���ڿ���������
                                                 |  |_________����ECCУ�����
                                                 |____________��ַ�������ķ�Χ
*/

#ifndef _MMC_SD_DRIVER_H_
#define _MMC_SD_DRIVER_H_
 
#include "stm32f10x.h" 

/* SD�����Ͷ��� */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

/* SPI�����ٶ�����*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define NO_RELEASE      0
#define RELEASE         1
							  

/* SD��ָ��� */
#define CMD0    0      //����λ  (Ӧ���ʽ��R1)
#define CMD1    1      //MMC����ʼ��ʼ��
#define CMD8	8     //ʶ�𿨵İ汾
#define CMD9    9     //����9 ����CSD����   (Ӧ���ʽ��R1)
#define CMD10   10     //����10����CID����   (Ӧ���ʽ��R1)
#define CMD12   12      //����12��ֹͣ���ݴ���    (Ӧ���ʽ��R1b)
#define CMD16   16      //����16������SectorSize Ӧ����0x00   (Ӧ���ʽ��R1)
#define CMD17   17      //����17����sector    (Ӧ���ʽ��R1)
#define CMD18   18      //����18����Multi sector    (Ӧ���ʽ��R1)
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block    (Ӧ���ʽ��R1)
#define CMD24   24      //����24��дsector    (Ӧ���ʽ��R1)
#define CMD25   25     //����25��дMulti sector    (Ӧ���ʽ��R1)
#define ACMD41  41      //����41��Ӧ����0x00    (Ӧ���ʽ��R1)
#define CMD55   55      //����55��Ӧ����0x01    (Ӧ���ʽ��R1)
#define CMD58   58      //����58����OCR��Ϣ     (Ӧ���ʽ��R1)
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00    (Ӧ���ʽ��R1)
					 
#define	SD_CS_ENABLE()  GPIO_ResetBits(GPIOA,GPIO_Pin_4) //ѡ��SD��
#define SD_CS_DISABLE() GPIO_SetBits(GPIOA,GPIO_Pin_4)	//ȡ��ѡ��				    	 

/* Private function prototypes -----------------------------------------------*/
void SPI_ControlLine(void);
void SPI_SetSpeed(u8 SpeedSet);

u8 SPI_ReadWriteByte(u8 TxData);                //SPI���߶�дһ���ֽ�
u8 SD_WaitReady(void);                          //�ȴ�SD������
u8 SD_SendCommand(u8 cmd, u32 arg,u8 crc);     //SD������һ������
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg,u8 crc);
u8 SD_Init(void);                               //SD����ʼ��
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD��������(OK)
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //��һ��sector��OK��
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); //дһ��sector��OK��
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); //�����sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);  //д���sector
u8 SD_Read_Bytes(unsigned long address,unsigned char *buf,unsigned int offset,unsigned int bytes);//��ȡһbyte
#endif
