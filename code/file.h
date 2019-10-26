#ifndef __FILE_H__
#define __FILE_H__

#define Sector_0	8192		//�����������߼�������һ�� ��sd�������ַ=�߼���ַ+0x400000
#define logic_offset	0x400000
struct folder//�ļ���
{
	char     foldername[12];
	long int foldercut;
	long int folderSector;
	long int folderlong;
	int      foldernums;
};
struct file //�ļ�
{
	char     filename[12];
	char		 filetype[3];
	long int filecut;
	long int fileSector;
	long int filelong;
	int      filenums;
};
long int read_DBR(void);
void read_Dir(char filecount);
char read_file(struct file *file_data,long int folderSector,long int foldercut);
void ini_FAT(void);

#endif
