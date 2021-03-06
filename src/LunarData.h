//�� ��ũ�����ݡ�
//�� ����ʱ�䣺2021-05-24 14:07:12 ����ţ���� ���£�С��ʮ�� ����һ
//�� ��ȡ�������ܴ�СΪ��567 �ֽڡ�
//�� ע�⣬���������Ѿ����У�Բ���֤��ȷ�������޸ģ����򽫵��´���

//E-mail: paradise300@163.com

#ifndef _LUNARDATA_H_
#define _LUNARDATA_H_

#define START_YEAR 2000	//����������ʼ��ݣ�������
#define END_YEAR 2051	//����������ֹ��ݣ����������꣩

//1999��ũ��ʮ�¼��Ժ��������������ӦcPreMonth�е���ţ���ǰΪ-1����ʾ1999��ũ��ʮ���Ժ������¡�
char const cPreLeapIndex=-1;

//1999��ũ��ʮ�¼��Ժ���·ݣ�ÿ�³�һ��2000���ڵ�������
//cPreMonth�зֱ��Ӧũ����ʮ�¡�ʮһ�¡�ʮ���¡����¡�
int const cPreMonth[4]={-54,-24,6,35};

//ũ���·���Ϣ��һ����3���ֽڱ�ʾ
//+-----------------------------------------------------------------------+
//| ��23λ |        ��22-17λ       |  ��16-13λ |       ��12-0λ         |
//|--------+------------------------+------------+------------------------|
//|  ����  | ũ�����³�һ���������� |    ����    | һ��λ��Ӧһ���·ݴ�С |
//+-----------------------------------------------------------------------+
//�·ݴ�С�������·�С���ڵ�λ���·ݴ���ڸ�λ�������������λ��
//��1900��Ϊ����3���ֽڵ�����չ���ɶ�����λ��
//  0       011110        1000                     1 0 1 1 0 1 1 0 1 0 0 1 0
//����  1��31�գ����ڣ�  �����   ������������ʮ���£�ʮһ�¡�������¡����¡����¡������µ�����
//ũ���·ݶ�Ӧ��λΪ0����ʾ�����Ϊ29�죨С�£���Ϊ1��ʾ��30�죨���£���
unsigned char const byMonthInfo[153]={
	0x46,0x06,0x93,0x2E,0x95,0x2B,0x54,0x05,0x2B,0x3E,0x0A,0x5B,	//2000-2003
	0x2A,0x55,0x5A,0x4E,0x05,0x6A,0x38,0xFB,0x55,0x60,0x0B,0xA4,	//2004-2007
	0x4A,0x0B,0x49,0x32,0xBA,0x93,0x58,0x0A,0x95,0x42,0x05,0x2D,	//2008-2011
	0x2C,0x8A,0xAD,0x50,0x0A,0xB5,0x3D,0x35,0xAA,0x62,0x05,0xD2,	//2012-2015
	0x4C,0x0D,0xA5,0x36,0xDD,0x4A,0x5C,0x0D,0x4A,0x46,0x0C,0x95,	//2016-2019
	0x30,0x95,0x2E,0x54,0x05,0x56,0x3E,0x0A,0xB5,0x2A,0x55,0xB2,	//2020-2023
	0x50,0x06,0xD2,0x38,0xCE,0xA5,0x5E,0x07,0x25,0x48,0x06,0x4B,	//2024-2027
	0x32,0xAC,0x97,0x56,0x0C,0xAB,0x42,0x05,0x5A,0x2C,0x6A,0xD6,	//2028-2031
	0x52,0x0B,0x69,0x3D,0x77,0x52,0x62,0x0B,0x52,0x4C,0x0B,0x25,	//2032-2035
	0x36,0xDA,0x4B,0x5A,0x0A,0x4B,0x44,0x04,0xAB,0x2E,0xA5,0x5B,	//2036-2039
	0x54,0x05,0xAD,0x3E,0x0B,0x6A,0x2A,0x5B,0x52,0x50,0x0D,0x92,	//2040-2043
	0x3A,0xFD,0x25,0x5E,0x0D,0x25,0x48,0x0A,0x55,0x32,0xB4,0xAD,	//2044-2047
	0x58,0x04,0xB6,0x40,0x05,0xB5,0x2C,0x6D,0xAA 	//2048-2050
};


 


//1999�궬������2000�������������������ݽ�������2000�ꡰ���š��ļ����ϡ�
char const cPreDongzhiOrder=-10;

//ÿ�����š���÷����÷��������Ϣ��һ���������ֽڱ�ʾ��
//+---------------------------------------------------+
//|  ��15-11λ |  ��10-6λ  |  ��5-1λ   |   ��ĩλ   |
//|------------+------------+------------+------------|
//|    ��÷    |    ��÷    |    ����    |    ĩ��    |
//+---------------------------------------------------+
//1.��һ�š����Ƕ��������󵽡��žš���ÿ�����š����9�죬��˳������������ʡ����š���Ϣʡ�ԡ�
//2.����������ġ��з����ڡ���������10�죬����ĩ�����ڡ��з�����10���20�죬��ˡ��з�����Ϣʡ�ԡ�
//��÷��Ϣ������������150Ϊ��÷��һ�յ�����������
//��÷��Ϣ������������180Ϊ��÷��һ�յ�����������
//������Ϣ������������180Ϊ������һ�յ�����������
//ĩ����Ϣ������λΪ��1������ʾĩ���������30�죬Ϊ��0����ʾĩ���������20�졣
unsigned short const wExtermSeason[51]={
	0x4359,0x61E1,0x3B97,0x6261,0x3C2A,0x5A9F,0x3468,0x5B1F,0x34E8,0x535D,	//2000-2009
	0x7A26,0x53DD,0x7AA6,0x4C1B,0x72E4,0x4C9B,0x7365,0x41D9,0x6BA3,0x4259,	//2010-2019
	0x6C23,0x3A97,0x6461,0x3B17,0x61E1,0x3368,0x5A1F,0x33E8,0x5A9F,0x2C26,	//2020-2029
	0x52DD,0x7CA6,0x535D,0x71E4,0x4B9B,0x7264,0x4C1B,0x6AA3,0x4459,0x6B23,	//2030-2039
	0x41D9,0x6361,0x3A17,0x63E1,0x3AAA,0x5C1F,0x32E8,0x5C9F,0x3368,0x51DD,	//2040-2049
	0x2BA6 	//2050
};

#endif //_LUNARDATA_H_
