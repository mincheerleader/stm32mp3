#include "mmc_sd.h"
#include "file.h"
//#include "LCD.h"
#include "bsp_usart2.h"
#include  "VS1053.h"
//SD_DATA
u8 SD_DATA[512];
u8 FAT_DATA[512];
//FAT����
long int BytesPerSector;         
long int SectorsPerCluster;      
long int ReserveSectors;      
long int FatTableNums;              
long int SectorsPerFat;      
long int HiddenSector;
long int RootDir;
long int StartRootClusterNumber;


long  int ReadDir;                 //����Ŀ¼
long  int	firstcut;	
long  int	nextcut;  	
long  int	firstSector;
long  int Sector;
long  int filelong; 
long  int fileSector;             //�ļ������ַ
long  int Readfile;               //���ļ�Ŀ¼
long  int pianyi=0;                 //����ƫ��
//DBRFAT
//void read_DBR(void)
//{
//	BytesPerSector=(SD_DATA[12]<<8)+SD_DATA[11];
//	SectorsPerCluster=SD_DATA[13];
//	ReserveSectors=(SD_DATA[15]<<8)+SD_DATA[14];
//	FatTableNums=(SD_DATA[17]<<8)+SD_DATA[16];
//	SectorsPerFat=(SD_DATA[37]<<8)+SD_DATA[36];
//	RootDir=pianyi+ReserveSectors+FatTableNums*SectorsPerFat;	
//}

long int read_DBR(void)
{
	long int RootDir2=0;
	
	BytesPerSector					=	(SD_DATA[12]<<8)+SD_DATA[11];	//ÿ�����ֽ���0B-0C
	SectorsPerCluster				=	SD_DATA[13];									//ÿ��������0D
	ReserveSectors					=	(SD_DATA[15]<<8)+SD_DATA[14];	//����������0e-0f
	FatTableNums						=	SD_DATA[16];									//FAT�����10
	SectorsPerFat						=	(SD_DATA[39]<<24)+(SD_DATA[38]<<16)+(SD_DATA[37]<<8)+SD_DATA[36];			//ÿFAT������24-27
	HiddenSector						=	(SD_DATA[31]<<24)+(SD_DATA[30]<<16)+(SD_DATA[29]<<8)+SD_DATA[28];			//����������1C-1F
	StartRootClusterNumber	=	(SD_DATA[47]<<24)+(SD_DATA[46]<<16)+(SD_DATA[45]<<8)+SD_DATA[44];			//��ʼ�غ�2C-2F
	
	RootDir2								=	ReserveSectors+HiddenSector+(SectorsPerFat*FatTableNums)+((StartRootClusterNumber-2)*SectorsPerCluster);			//��Ŀ¼��ʼ���� = ���������� +  ����������+ һ��FAT�������� �� FAT����� + (��ʼ�غ�-2) x ÿ�ص�������
	
	return(RootDir2);
}
//DIR
void read_Dir(char filecount)
{
	firstcut=(SD_DATA[21+filecount*32]<<24)+(SD_DATA[20+filecount*32]<<16)+(SD_DATA[27+filecount*32]<<8)+SD_DATA[26+filecount*32];
	firstSector=RootDir+(firstcut-2)*SectorsPerCluster;
	filelong=(SD_DATA[31+filecount*32]<<24)+(SD_DATA[30+filecount*32]<<16)+(SD_DATA[29+filecount*32]<<8)+SD_DATA[28+filecount*32];
}
//readfile
char read_file(struct file *file_data,long int folderSector,long int foldercut)
{
	char countfile=2;
	int  i=0;//ѭ������
	int  filenum=0;
	if(foldercut==2)//���� . ��..
	{
		countfile=0;
	}
	SD_ReadSingleBlock(folderSector,SD_DATA);
	Readfile=folderSector;
	nextcut=foldercut;
	//putnums(0,80,pianyi);
	while(1)
	{			
		if(countfile>15)
		{
			countfile=0;
			Readfile++;
			if((Readfile-folderSector)==SectorsPerCluster)
			{
				
				SD_ReadSingleBlock((ReserveSectors+pianyi+nextcut/128),FAT_DATA);//putnums(16,80,(ReserveSectors+pianyi+nextcut/128));
				if((FAT_DATA[4*(nextcut%128)+3]==0x0f)&&(FAT_DATA[4*(nextcut%128)+2]==0xff)&&(FAT_DATA[4*(nextcut%128)+1]==0xff)&&(FAT_DATA[4*(nextcut%128)]==0xff))
				{
					return 1;
				}	
				nextcut=(FAT_DATA[4*(nextcut%128)+3]<<24)+(FAT_DATA[4*(nextcut%128)+2]<<16)+(FAT_DATA[4*(nextcut%128)+1]<<8)+(FAT_DATA[4*(nextcut%128)]);
				folderSector=RootDir+(nextcut-2)*SectorsPerCluster;
				Readfile=folderSector;
			}
			SD_ReadSingleBlock(Readfile,SD_DATA);
		}
		if((SD_DATA[0+countfile*32]==0)&&(SD_DATA[1+countfile*32]==0)&&(SD_DATA[2+countfile*32]==0)&&(SD_DATA[3+countfile*32]==0))
		{
			return 1;
		}
		read_Dir(countfile);		
		if((filelong==0)||
		   ((SD_DATA[8+countfile*32]=='M')&&(SD_DATA[9+countfile*32]=='P')&&(SD_DATA[10+countfile*32]=='3'))||
		   ((SD_DATA[8+countfile*32]=='W')&&(SD_DATA[9+countfile*32]=='M')&&(SD_DATA[10+countfile*32]=='A'))||
		   ((SD_DATA[8+countfile*32]=='M')&&(SD_DATA[9+countfile*32]=='I')&&(SD_DATA[10+countfile*32]=='D'))||
		   ((SD_DATA[8+countfile*32]=='W')&&(SD_DATA[9+countfile*32]=='A')&&(SD_DATA[10+countfile*32]=='V'))||
		   ((SD_DATA[8+countfile*32]=='F')&&(SD_DATA[9+countfile*32]=='L')&&(SD_DATA[10+countfile*32]=='A'))||
		   ((SD_DATA[8+countfile*32]=='T')&&(SD_DATA[9+countfile*32]=='X')&&(SD_DATA[10+countfile*32]=='T'))||
		   ((SD_DATA[8+countfile*32]=='B')&&(SD_DATA[9+countfile*32]=='I')&&(SD_DATA[10+countfile*32]=='N'))||
		   ((SD_DATA[8+countfile*32]==' ')&&(SD_DATA[9+countfile*32]==' ')&&(SD_DATA[10+countfile*32]==' '))||
		   ((SD_DATA[8+countfile*32]=='O')&&(SD_DATA[9+countfile*32]=='G')&&(SD_DATA[10+countfile*32]=='G')))
		{
			if(SD_DATA[0+countfile*32]!=0xE5)
			{
				for(i=0;i<11;i++)
				{
					file_data->filename[i]=SD_DATA[i+countfile*32];
				}
				file_data->fileSector=firstSector;
				file_data->filecut   =firstcut;
				file_data->filelong  =filelong;
				file_data->filenums  =filenum;
				filenum++;
				file_data++;
				
			}
			
		}
		countfile++;
	}		
}



//ini_FAT
extern struct folder folderdata[256];
extern struct file   filedata[256];
extern short  int    point,pcount,count,folderstack;
void ini_FAT(void)
{

	long int dir=0,filestartaddress=0;
	unsigned char i=0;
	int x=0;
	
	x=SD_ReadSingleBlock(Sector_0,SD_DATA);
//	printf("x=%x\t",x);
	delay(300000);
	dir=read_DBR();
//	printf("dir=%ld\t",dir);	
	
	folderdata[folderstack].folderSector=dir;
	folderdata[folderstack].foldercut   =2;
	folderdata[folderstack].folderlong  =0x00;
	folderdata[folderstack].foldernums  =0x00;
	folderstack++;
	
	delay(300000);
	SD_ReadSingleBlock(folderdata[folderstack-1].folderSector,SD_DATA);//����Ŀ¼
	for(point=0;point<256;point++)									//pointΪĿ¼����ÿ����ΪһĿ¼
	{
		for(i=0x00;i<=0x07;i++)
			filedata[point].filename[i]=SD_DATA[point*2*16+i];						//8*16��8Ϊ�� 16Ϊÿ�����ݵĸ������ļ���Ŀ¼�е�λ�ã�����ʵ���������
		for(i=0x08;i<=0x0A;i++)
			filedata[point].filetype[i-0x08]=SD_DATA[point*2*16+i];
		
		filestartaddress=(SD_DATA[(point*2+1)*16+0x05]<<24)+(SD_DATA[(point*2+1)*16+0x04]<<16)+(SD_DATA[(point*2+1)*16+0x0B]<<8)+SD_DATA[(point*2+1)*16+0x0A];	
		filedata[point].fileSector=((ReserveSectors+SectorsPerFat*FatTableNums+(filestartaddress-2)*SectorsPerCluster)*BytesPerSector+logic_offset)/512;
		//�߼���ַ���ļ���ʼ��ַ = (���������� + FAT�������� * FAT�����(2) + (�ļ���ʼ�غ�-2)*ÿ��������)*ÿ�����ֽ���
		//���߼���ַ+�����ַƫ�ƣ�/512Ϊʵ��������ַ
		
		filedata[point].filelong=(SD_DATA[(point*2+1)*16+0x0F]<<24)+(SD_DATA[(point*2+1)*16+0x0E]<<16)+(SD_DATA[(point*2+1)*16+0x0D]<<8)+SD_DATA[(point*2+1)*16+0x0C];
	}


//���Բ鿴
//	for(i=0;i<=7;i++)
//		printf("filename%d: %02x\t",i,filedata[4].filename[i]);
//	for(i=0x08;i<=0x0A;i++)
//		printf("filetype%d=%02x\t",i,filedata[4].filetype[i-0x08]);
//		
//	printf("SD_DATA[9*16+0x05]=%02x\t",SD_DATA[9*16+0x05]);
//	printf("SD_DATA[9*16+0x04]=%02x\t",SD_DATA[9*16+0x04]);
//	printf("SD_DATA[9*16+0x0b]=%02x\t",SD_DATA[9*16+0x0b]);
//	printf("SD_DATA[9*16+0x0a]=%02x\t",SD_DATA[9*16+0x0a]);
//	printf("\n");
//	printf("\n");
//	printf("ReserveSectors=%02x\t",(unsigned int)ReserveSectors);
//	printf("SectorsPerFat=%02x\t",(unsigned int)SectorsPerFat);
//	printf("FatTableNums=%02x\t",(unsigned int)FatTableNums);
//	printf("filestartaddress=%02x\t",(unsigned int)filestartaddress);
//	printf("BytesPerSector=%02x\t",(unsigned int)BytesPerSector);
//	printf("\n");
//	printf("\n");
//	printf("fileaddr=%ld\t",filedata[4].fileSector);
//	printf("filelong=%ld\t\n\n",filedata[4].filelong);

}
