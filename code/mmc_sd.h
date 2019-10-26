/*命令应答
R1:（置1有效），用于应答大部分命令的
7  6  5  4  3  2  1  0
0  |  |  |  |  |  |  |
   |  |  |  |  |  |  |______禁止状态（正在执行处理）
   |  |  |  |  |  |_________檫除复位
   |  |  |  |  |____________非法命令
   |  |  |  |_______________CRC错误
   |  |  |__________________檫除序列错误
   |  |_____________________地址错误
   |________________________参数错误

R1b：和R1有相同的格式，但添加了附加的忙信号。忙信号时连续8位为一组，全0时为忙，非全零时为准备好

写数据应答:
7  6  5  4  3  2  1  0
x  x  x  0  Status   1
Status="010":数据确认  "101":由于CRC错误被抛弃  "110":由于写入错误被抛弃

发送和接收的数据格式：（1）对于单块读、单块写和多块读：StartBlock（0xfe）+UserData（最大512字节）+CRC（2Bytes）
                            多块读通过发送CMD12来停止读取数据
                      （2）对于多块写：0xfc+UserData（最大512字节）+CRC（2Bytes)。发送StopTran（0xfd）来停止写入数据

读数据失败时的错误数据标识：（1Byte）7  6  5  4  3  2  1  0
                                     0  0  0  0  |  |  |  |___执行错误
                                                 |  |  |______卡内控制器错误
                                                 |  |_________卡内ECC校验错误
                                                 |____________地址超出卡的范围
*/

#ifndef _MMC_SD_DRIVER_H_
#define _MMC_SD_DRIVER_H_
 
#include "stm32f10x.h" 

/* SD卡类型定义 */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

/* SPI总线速度设置*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD传输数据结束后是否释放总线宏定义 */
#define NO_RELEASE      0
#define RELEASE         1
							  

/* SD卡指令表 */
#define CMD0    0      //卡复位  (应答格式：R1)
#define CMD1    1      //MMC卡开始初始化
#define CMD8	8     //识别卡的版本
#define CMD9    9     //命令9 ，读CSD数据   (应答格式：R1)
#define CMD10   10     //命令10，读CID数据   (应答格式：R1)
#define CMD12   12      //命令12，停止数据传输    (应答格式：R1b)
#define CMD16   16      //命令16，设置SectorSize 应返回0x00   (应答格式：R1)
#define CMD17   17      //命令17，读sector    (应答格式：R1)
#define CMD18   18      //命令18，读Multi sector    (应答格式：R1)
#define ACMD23  23      //命令23，设置多sector写入前预先擦除N个block    (应答格式：R1)
#define CMD24   24      //命令24，写sector    (应答格式：R1)
#define CMD25   25     //命令25，写Multi sector    (应答格式：R1)
#define ACMD41  41      //命令41，应返回0x00    (应答格式：R1)
#define CMD55   55      //命令55，应返回0x01    (应答格式：R1)
#define CMD58   58      //命令58，读OCR信息     (应答格式：R1)
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00    (应答格式：R1)
					 
#define	SD_CS_ENABLE()  GPIO_ResetBits(GPIOA,GPIO_Pin_4) //选中SD卡
#define SD_CS_DISABLE() GPIO_SetBits(GPIOA,GPIO_Pin_4)	//取消选中				    	 

/* Private function prototypes -----------------------------------------------*/
void SPI_ControlLine(void);
void SPI_SetSpeed(u8 SpeedSet);

u8 SPI_ReadWriteByte(u8 TxData);                //SPI总线读写一个字节
u8 SD_WaitReady(void);                          //等待SD卡就绪
u8 SD_SendCommand(u8 cmd, u32 arg,u8 crc);     //SD卡发送一个命令
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg,u8 crc);
u8 SD_Init(void);                               //SD卡初始化
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD卡读数据(OK)
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //读一个sector（OK）
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); //写一个sector（OK）
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); //读多个sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);  //写多个sector
u8 SD_Read_Bytes(unsigned long address,unsigned char *buf,unsigned int offset,unsigned int bytes);//读取一byte
#endif
