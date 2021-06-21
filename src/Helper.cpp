#include "Helper.h"
#include "LunarData.h"
#include <stdio.h>
#include "EPD4IN_Device.h"
//#include "EPD_Http.h"
#include "pic2.h"
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
EPD4INC epd_drv1;


/*
注意，年干支计算请参照根目录下“农历简介.txt”中的方法。月干支计算比较复杂，
是以节气为界（注意，不是中气，节气指立春、惊蛰、清明、立夏、芒种、小暑、立
秋、白露、寒露、立冬、大雪、小寒这12个，其余的即为中气），1998年12月7日(大
雪)的月干支为甲子，可在此基础上进行前推和后推。日干支以2000年1月7日（甲子
日）为参考，只要算出天数差即可知道日干支。时干支则是根据日干支来推算的，天
干根据日的天干有以下对应关系，甲、己得甲，乙、庚得丙，丙、辛得戊，丁、壬得
庚，戊、癸得壬，时地支则为按每两个小时为一个地支来计量，前一天的23时至1时为
子时，1时至3时为丑时，依此类推。
*/

/*
本例中的星期算法采用的是一个通用算法，公式如下：
W = [Y-1] + [(Y-1)/4] - [(Y-1)/100] + [(Y-1)/400] + D
Y是年份数，D是这一天在这一年中的年内序数加1，也就是这一天在这一年中是第几天。
这个公式大致验证了一下是可行的，就本例所支持的年份区间内至少是正确的。
*/

char *Riming[30]={"初一","初二","初三","初四","初五","初六","初七","初八","初九","初十","十一","十二","十三","十四","十五","十六","十七","十八","十九","二十","廿一","廿二","廿三","廿四","廿五","廿六","廿七","廿八","廿九","三十"};
char *Yueming[12]={"正月","二月","三月","四月","五月","六月","七月","八月","九月","十月","冬月","腊月"};
char *Tiangan[10]={"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
char *Dizhi[12]={"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
char *Shengxiao[12]={"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪" };
char *Jieqi[24]={"立春", "雨水", "惊蛰", "春分", "清明", "谷雨", "立夏", "小满", "芒种", "夏至", "小暑", "大暑", "立秋", "处暑", "白露", "秋分", "寒露", "霜降", "立冬", "小雪", "大雪", "冬至", "小寒", "大寒"};
//char *Xingqi[7]={"星期日","星期一","星期二","星期三","星期四","星期五","星期六"};
char *Xingqi[7]={"日","一","二","三","四","五","六"};
char *Shujiu[9]={"一九","二九","三九","四九","五九","六九","七九","八九","九九"};
char *Meiyu[2]={"入梅","出梅"};
char *Sanfu[3]={"初伏","中伏","末伏"};
struct pl_area;
struct pl_area user_area1;
unsigned short DayOrdinal[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
unsigned short DayOrdinalLeap[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};

//判断闰年，参数：年份，返回值：0-平年，1-闰年
bool IsLeapYear(short sYear);

//计算日期在年内的序数，参数：年，月，日，年内序数，返回值：0-失败，1-成功
bool GetDayOrdinal(short sYear, unsigned short wMonth, unsigned short wDay,int *nDays);

//从年内序数计算月、日，参数：年，年内序数，月，日，返回值：0-失败，1-成功
bool GetDateFromOrdinal(short sYear,short sOrdinal,unsigned short* wMonth,unsigned short* wDay);

//检验年、月、日的合法性，参数：年，月，日，返回值：0-失败，1-成功
bool DateCheck(short sYear,unsigned short wMonth,unsigned short wDay);

//获取农历新年的公历年内序数，参数：农历年，返回值：农历新年的公历年内序数
short LunarGetNewYearOrdinal(short sLunarYear);

//获取农历月的天数，参数：农历年，农历月，是否为闰月，返回值：该农历月的天数，为0代表参数无效
unsigned short LunarGetDaysofMonth(short sLunarYear,unsigned short wLunarMonth,bool bLeapMonth);

//展开大小月数据表（某一年的），参数：农历年，从上一年十一月开始到当前年份（闰）十二月的每月天数，返回值：0-失败，1-成功
bool LunarExpandDX(short sLunarYear,int iDayOfMonth[15]);

//获取农历某一年的闰月情况，参数：农历年，返回值，该年的闰月月份，0表示无闰月
unsigned short LunarGetLeapMonth(short sLunarYear);

//获取农历月份信息
bool GetMonthInfo(unsigned short wYear,unsigned int* puData);

//公历转农历，参数：公历年、月、日，农历年、月、日，是否为闰月，返回值：0-失败，1-成功
bool Gongli2Nongli(short sYear,unsigned short wMonth,unsigned short wDay,short* sLunarYear,unsigned short* wLunarMonth,unsigned short* wLunarDay,bool* bLeapMonth);

//农历转公历，参数：家历年、月、日，是否为闰月，公历年、月、日，返回值：0-失败，1-成功
bool Nongli2Gongli(short sLunarYear,unsigned short wLunarMonth,unsigned short wLunarDay,bool bLeapMonth,short* sYear,unsigned short* wMonth,unsigned short* wDay);

//得到指定年份的节气信息，首个是小寒
bool GetJieQi(short sYear,unsigned short wMonth,unsigned short wJieQi[2]);

//计算星期，返回-1表示输入的年月日不正确或者超出年份范围
unsigned short GetDayOfWeek(short sYear,unsigned short wMonth,unsigned short wDay);

//计算某个月的天数，返回天数，如果返回0表示年或月有误
unsigned short GetDaysOfMonth(short sYear,unsigned short wMonth);

//公历节日及节气显示，参数：公历年、公历月、公历日
unsigned short G_HolidayShow(short sYear,unsigned short wMonth,unsigned short wDay,short left,short top);

//农历节日及杂项显示，参数：农历年、农历月、农历日、农历闰月
bool L_HolidayShow(int sLYear,int iLMonth,int iLDay,int iLeapMonth,short left,short top);

//查询某一年的数九及三伏，参数：公历年、上一年冬至（即一九）、初伏、末伏
bool GetExtremeSeason(short sYear,short* sYijiu,unsigned short* wChuFu,unsigned short* wMoFu);

//查询某一年的入梅、出梅，参数：公历年，入梅，出梅
bool GetMeiYu(short sYear,unsigned short* wRuMeiOrd,unsigned short* wChuMeiOrd);

//===============================================================================

bool IsLeapYear(short sYear)
{
	/*if (sYear<1600||sYear>=6400)//压缩算法规定了的年份区间（提取器只能导出此区间的数据，Lunar.dll支持-6000到20000（不含20000）之间的年份）
	{
		return E_FAIL;
	}*/
	if (sYear%4==0&&sYear%100!=0||sYear%400==0)//判断闰年的条件
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

bool GetDayOrdinal(short sYear, unsigned short wMonth, unsigned short wDay,int *nDays)
{
	//从日期算出距离元旦的天数
	int ret=0;
	if (DateCheck(sYear,wMonth,wDay)==0)//对输入的日期进行检查
	{
		return 0;
	}
	ret=IsLeapYear(sYear);//判断是否为闰年
	if (ret==-1)
	{
		return 0;
	}
	if (ret==1)
	{
		*nDays=DayOrdinalLeap[wMonth-1]+wDay-1;//元旦为序数0，因此减1
	}else
	{
		*nDays=DayOrdinal[wMonth-1]+wDay-1;
	}
	return 1;
}

bool GetDateFromOrdinal(short sYear,short sOrdinal,unsigned short* wMonth,unsigned short* wDay)
{
	//从年内序数计算日期
	int ret=0,i=0;
	if (sOrdinal<0)
	{
		return FALSE;
	}
	ret=IsLeapYear(sYear);
	if (ret==1)
	{
		if (sOrdinal>=366)//超出了该年的总天数
		{
			return FALSE;
		}
	}else
	{
		if (sOrdinal>=365)
		{
			return FALSE;
		}
	}
	if (ret!=-1)
	{
		//年符合，则计算;
		*wMonth=0;
		for (i=0;i<12;i++)
		{
			if (ret==1)
			{
				if (sOrdinal>=DayOrdinalLeap[i]&&sOrdinal<DayOrdinalLeap[i+1])//找出月份
				{
					*wMonth=i+1;
					*wDay=sOrdinal-DayOrdinalLeap[i]+1;//计算出“日”
					break;
				}
			}else
			{
				if (sOrdinal>=DayOrdinal[i]&&sOrdinal<DayOrdinal[i+1])
				{
					*wMonth=i+1;
					*wDay=sOrdinal-DayOrdinal[i]+1;
					break;
				}
			}
		}
	}else
	{
		*wDay=0;
		*wMonth=0;
		return FALSE;
	}
	return TRUE;
}

bool DateCheck(short sYear,unsigned short wMonth,unsigned short wDay)
{
	if (sYear<START_YEAR||sYear>=END_YEAR||wMonth>12||wMonth<1||wDay<1||wDay>31)
	{
		return 0;
	}
	if (wMonth==4||wMonth==6||wMonth==9||wMonth==11)
	{
		if (wDay==31)
		{
			return 0;
		}
	}else if (wMonth==2)
	{
		if (IsLeapYear(sYear)==0)
		{
			if (wDay>28)
			{
				return 0;
			}
		}else
		{
			if (wDay>29)
			{
				return 0;
			}
		}
	}
	return 1;
}

short LunarGetNewYearOrdinal(short sLunarYear)
{
	unsigned int uData=0;
	if (GetMonthInfo(sLunarYear,&uData)==FALSE)
	{
		return 0;
	}
	return (short)((uData>>17)&0x3F);//取出农历新年的年内序数
}

unsigned short LunarGetDaysofMonth(short sLunarYear,unsigned short wLunarMonth,bool bLeapMonth)
{
	int i=0;//循环变量
	unsigned int DX_data=0;//该年大小月情况
	unsigned int uData=0;
	int Acc_LeapMonth=0;
	if (sLunarYear==START_YEAR-1)//农历还在起始年份的前一年
	{
		if (cPreLeapIndex==-1)//无闰月
		{
			if (bLeapMonth==TRUE)
			{
				return 0;
			}
			return cPreMonth[wLunarMonth-9]-cPreMonth[wLunarMonth-10];
		}else
		{
			Acc_LeapMonth=cPreLeapIndex+9;
			if (Acc_LeapMonth>wLunarMonth)
			{
				if (bLeapMonth==TRUE)
				{
					return 0;
				}
				return cPreMonth[wLunarMonth-9]-cPreMonth[wLunarMonth-10];
			}else
			{
				if ((bLeapMonth==TRUE&&wLunarMonth==Acc_LeapMonth)||wLunarMonth>Acc_LeapMonth)
				{
					return cPreMonth[wLunarMonth-8]-cPreMonth[wLunarMonth-9];
				}else
				{
					return cPreMonth[wLunarMonth-9]-cPreMonth[wLunarMonth-10];
				}
			}
		}
	}
	if (GetMonthInfo(sLunarYear,&uData)==FALSE)
	{
		return 0;
	}
	DX_data=(unsigned short)(uData&0x1FFF);//整年大小月情况
	Acc_LeapMonth=LunarGetLeapMonth(sLunarYear);//获取真实闰月月份
	if (bLeapMonth)//指定查询的当前月是闰月
	{
		if (Acc_LeapMonth!=wLunarMonth)
		{
			return 0;//不存在的闰月
		}
		for (i=0;i<wLunarMonth;i++)
		{
			DX_data>>=1;//右移一位，即从末尾开始找该闰月月份所在的位
		}
	}else
	{
		if (Acc_LeapMonth>0)//存在闰月
		{
			if (wLunarMonth<=Acc_LeapMonth)//月份在闰月之前，倒找需要多找一位
			{
				for (i=0;i<wLunarMonth-1;i++)
				{
					DX_data>>=1;
				}
			}else
			{
				for (i=0;i<wLunarMonth;i++)//月份在闰月之后
				{
					DX_data>>=1;
				}
			}
		}else
		{
			for (i=0;i<wLunarMonth-1;i++)//无闰月
			{
				DX_data>>=1;
			}
		}
	}
	if (DX_data&0x1)
	{
		return 30;//大月
	}else
	{
		return 29;//小月
	}
}

bool LunarExpandDX(short sLunarYear,int iDayOfMonth[15])
{
	int i=0,pos=0,iLeapMonth=0;//循环变量，数组写入位置，闰月
	if (sLunarYear<START_YEAR||sLunarYear>=END_YEAR)
	{
		return FALSE;
	}
	for (i=0;i<15;i++)
	{
		iDayOfMonth[i]=0;//对数组清零
	}
	if (sLunarYear==START_YEAR)
	{
		if (cPreLeapIndex==-1)//处理起始年份之前一年数据
		{
			iDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];//农历上一年十一月总天数
			pos++;
		}else
		{
			if (cPreLeapIndex==1)//闰十月
			{
				iDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}else
			{
				//闰十一月或闰十二月
				iDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];
				pos++;
				iDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}
		}
		iDayOfMonth[pos]=LunarGetNewYearOrdinal(sLunarYear)-cPreMonth[2];//（闰）十二月
		pos++;
	}else
	{
		iLeapMonth=LunarGetLeapMonth(sLunarYear-1);//取得前一农历年的闰月情况
		if (iLeapMonth<11)//一月至十月的闰月
		{
			iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,11,0);//取上一年十一月天数
			pos++;
			iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,12,0);//取上一年十二月天数
			pos++;
		}else
		{
			iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,11,0);//取上一年十一月的天数
			pos++;
			if (iLeapMonth==11)//闰十一月
			{
				iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,11,1);//取上一年闰十一月的天数
				pos++;
				iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,12,0);//取上一年十二月天数
				pos++;
			}else if (iLeapMonth==12)
			{
				iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,12,0);//取上一年十二月天数
				pos++;
				iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear-1,12,1);//取上一年闰十二月天数
				pos++;
			}
		}
	}
	iLeapMonth=LunarGetLeapMonth(sLunarYear);//获取当前农历年的闰月情况
	if (iLeapMonth==0)//无闰月
	{
		for (i=0;i<12;i++)
		{
			iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear,i+1,0);//取每个农历月天数
			pos++;
		}
	}else
	{
		for (i=0;i<12;i++)
		{
			if (i==iLeapMonth)
			{
				iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear,i,1);//取闰月的天数
				pos++;
			}
			iDayOfMonth[pos]=LunarGetDaysofMonth(sLunarYear,i+1,0);//取非闰月的天数
			pos++;
		}
	}
	return TRUE;
}

bool GetMonthInfo(unsigned short wYear,unsigned int* puData)
{
	int iStartPos=(wYear-START_YEAR)*3;
	unsigned int uData=0;
	if (wYear<START_YEAR||wYear>=END_YEAR)
	{
		return FALSE;
	}
	uData=byMonthInfo[iStartPos];
	uData<<=8;
	uData|=byMonthInfo[iStartPos+1];
	uData<<=8;
	uData|=byMonthInfo[iStartPos+2];
	if (puData)
	{
		*puData=uData;
	}
	return TRUE;
}

unsigned short LunarGetLeapMonth(short sLunarYear)
{
	unsigned int data=0;
	unsigned short wLeapMonth=0;
	if (GetMonthInfo(sLunarYear,&data)==FALSE)
	{
		return 0;
	}
	wLeapMonth=(unsigned short)((data>>13)&0x0F);
	return wLeapMonth;
}

bool Gongli2Nongli(short sYear,unsigned short wMonth,unsigned short wDay,short* sLunarYear,unsigned short* wLunarMonth,unsigned short* wLunarDay,bool* bLeapMonth)
{
	int DaysNum=0,LunarNewYear=0,i=0,remain_days=0;//年内序数，农历新年的公历年内序数，循环变量，剩余天数
	int DaysOfLunarMonth[15]={0};//存放农历月份天数
	int iLeapMonthPre=0,iLeapMonth=0;//农历上一年闰月，今年闰月
	int ret=GetDayOrdinal(sYear,wMonth,wDay,&DaysNum);
	if (ret==0)
	{
		return 0;//日期不正确
	}
	*sLunarYear=sYear;
	LunarNewYear=LunarGetNewYearOrdinal(*sLunarYear);
	LunarExpandDX(*sLunarYear,DaysOfLunarMonth);//获取月份天数，数组从上一年十一月开始到今年（闰）十二月，包含闰月
	iLeapMonthPre=LunarGetLeapMonth(*sLunarYear-1);
	if (iLeapMonthPre==0)
	{
		iLeapMonth=LunarGetLeapMonth(*sLunarYear);//上一年没有闰月，则查询今年闰月
	}
	*bLeapMonth=FALSE;
	remain_days=DaysNum-LunarNewYear;//距离农历新年天数
	if (iLeapMonthPre>10)
	{
		i=3;//今年正月在“DaysOfLunarMonth”中的索引，上一年十一月或十二月有闰月
	}else
	{
		i=2;//上一年十一月和十二月无闰月时，今年正月的索引
	}
	if (LunarNewYear>DaysNum)//早于农历新年
	{
		*sLunarYear-=1;//农历年减1
		while(remain_days<0)
		{
			i--;//第一次先减去是因为当前i是正月，减1表示上一年十二月（或闰十二月）
			remain_days+=DaysOfLunarMonth[i];//加上上一年十二月、十一月的总天数（含闰月）直到日数大于0
		}
		if (iLeapMonthPre>10)//如果上一年十一月或十二月存在闰月
		{
			if (iLeapMonthPre==11)//闰十一月
			{
				if (i==0)//十一月（即在闰月之前）
				{
					*wLunarMonth=11+i;//转换到月份
				}else
				{
					*wLunarMonth=10+i;
					if (*wLunarMonth==iLeapMonthPre)
					{
						*bLeapMonth=TRUE;
					}
				}
			}else if (iLeapMonthPre==12)//闰十二月
			{
				if (i<2)//在闰月之前
				{
					*wLunarMonth=11+i;
				}else
				{
					*wLunarMonth=10+i;
					if (*wLunarMonth==iLeapMonthPre)
					{
						*bLeapMonth=TRUE;
					}
				}
			}
		}else
		{
			*wLunarMonth=11+i;
		}
		*wLunarDay=remain_days;
	}else
	{
		while (remain_days>=DaysOfLunarMonth[i])
		{
			remain_days-=DaysOfLunarMonth[i];//寻找农历月
			i++;//移至下个月
		}
		if (iLeapMonthPre>10)
		{
			*wLunarMonth=i-2;
		}else
		{
			if (iLeapMonth>0)
			{
				if (i-2<iLeapMonth)
				{
					*wLunarMonth=i-1;
				}else
				{
					*wLunarMonth=i-2;
					if (*wLunarMonth==iLeapMonth)
					{
						*bLeapMonth=TRUE;
					}
				}
			}else
			{
				*wLunarMonth=i-1;
			}
		}
		*wLunarDay=remain_days;
	}
	*wLunarDay+=1;//索引转换到数量
	return TRUE;
}

bool Nongli2Gongli(short sLunarYear,unsigned short wLunarMonth,unsigned short wLunarDay,bool bLeapMonth,short* sYear,unsigned short* wMonth,unsigned short* wDay)
{
 	int DaysOfLunarMonth[15]={0};//存放农历月份天数
 	int iLeapMonthPre=0,iLeapMonth=0;//农历年闰月
 	int LunarNewYear=0,i=0,remain_days=0;//年内序数，农历新年的公历年内序数，循环变量，剩余天数
 	int iDaysofYear=0;
 	if (sLunarYear<START_YEAR||sLunarYear>=END_YEAR||wLunarMonth<1||wLunarMonth>12||wLunarDay<1||wLunarDay>30)
 	{
 		return FALSE;//年、月、日检查
 	}
 	if (bLeapMonth)
 	{
 		if (LunarGetLeapMonth(sLunarYear)!=wLunarMonth)
 		{
 			return FALSE;//闰月检查
 		}
 	}
 	if (wLunarDay>LunarGetDaysofMonth(sLunarYear,wLunarMonth,bLeapMonth))
 	{
 		return FALSE;//大小月检查
 	}
 	LunarExpandDX(sLunarYear,DaysOfLunarMonth);//大小月表（农历每月天数）
 	LunarNewYear=LunarGetNewYearOrdinal(sLunarYear);//找到正月初一的公历年内序数
 	iLeapMonth=LunarGetLeapMonth(sLunarYear);//找出农历年的闰月
 	remain_days+=LunarNewYear;//加上正月初一的序数
 	if (iLeapMonthPre>10)
 	{
 		i=3;//今年正月在“DaysOfLunarMonth”中的索引，上一年十一月或十二月有闰月
 	}else
 	{
 		i=2;//上一年十一月和十二月无闰月时，今年正月的索引
 	}
 	if (iLeapMonth==0)
 	{
 		if (iLeapMonthPre>10)
 		{
 			for (;i<wLunarMonth+2;i++)
 			{
 				remain_days+=DaysOfLunarMonth[i];//年内序数累加
 			}
 		}else
 		{
 			for (;i<wLunarMonth+1;i++)
 			{
 				remain_days+=DaysOfLunarMonth[i];//年内序数累加
 			}
 		}
 	}else
 	{
 		if (wLunarMonth<iLeapMonth||(bLeapMonth==FALSE&&wLunarMonth==iLeapMonth))//在闰月之前
 		{
 			for (;i<wLunarMonth+1;i++)
 			{
 				remain_days+=DaysOfLunarMonth[i];
 			}
 		}else
 		{
 			for (;i<wLunarMonth+2;i++)
 			{
 				remain_days+=DaysOfLunarMonth[i];
 			}
 		}
 	}
 	remain_days+=wLunarDay-1;//减1是因为日名转到序数
 	GetDayOrdinal(sLunarYear,12,31,&iDaysofYear);//获取公历年总天数
 	iDaysofYear++;//从序数转到天数
 	*sYear=sLunarYear;
 	if ((remain_days+1)>iDaysofYear)
 	{
 		remain_days-=iDaysofYear;
 		*sYear+=1;//下一年
 	}
 	GetDateFromOrdinal(*sYear,remain_days,wMonth,wDay);
	return TRUE;
}



unsigned short GetDayOfWeek(short sYear,unsigned short wMonth,unsigned short wDay)
{
	unsigned int DayofWeek=0;
	int uDayOrd=0;
	if (GetDayOrdinal(sYear,wMonth,wDay,&uDayOrd)==0)
	{
		return 0;
	}
	uDayOrd++;//一年中的第几天，因为GetDayOrdinal所得到的是索引，因此要加一
	sYear--;
	DayofWeek=(sYear+sYear/4-sYear/100+sYear/400+uDayOrd)%7;//这个只是算星期的通用公式
	return DayofWeek;
}

unsigned short GetDaysOfMonth(short sYear,unsigned short wMonth)
{
	int days1=0,days2=0;
	int ret=0;
	if (wMonth==12)
	{
		return 31;//这里为了简便，判断12月就直接返回
	}
	ret=GetDayOrdinal(sYear,wMonth,1,&days1);//本月1日在年内的序数
	if (ret==0)
	{
		return ret; 
	}
	wMonth++;
	ret=GetDayOrdinal(sYear,wMonth,1,&days2);//下个月1日的年内序数
	if (ret==0)
	{
		return ret; 
	}
	ret=days2-days1;//下个月1日的序数减本月1日的序数
	return ret;
}

bool GetExtremeSeason(short sYear,short* sYijiu,unsigned short* wChuFu,unsigned short* wMoFu)
{
	unsigned short wjq[2]={0};
	int ET_index=sYear-START_YEAR;//数九、梅雨及三伏的年份索引
	if (sYear<START_YEAR||sYear>=END_YEAR)
	{
		return FALSE;
	}
	if (sYear==START_YEAR)
	{
		*sYijiu=cPreDongzhiOrder;
	}else
	{
		//GetJieQi(sYear-1,12,wjq);
		*sYijiu=wjq[1]-32;
	}
	*wChuFu=((wExtermSeason[ET_index]&0x3E)>>1)+180;
	*wMoFu=(*wChuFu)+((wExtermSeason[ET_index]&0x01)==1?30:20);
	return TRUE;
}

bool GetMeiYu(short sYear,unsigned short* wRuMeiOrd,unsigned short* wChuMeiOrd)
{
	int ET_index=sYear-START_YEAR;//数九、梅雨及三伏的年份索引
	if (sYear<START_YEAR||sYear>=END_YEAR)
	{
		return FALSE;
	}
	*wRuMeiOrd=((wExtermSeason[ET_index]&0xF800)>>11)+150;
	*wChuMeiOrd=((wExtermSeason[ET_index]&0x07C0)>>6)+180;
	return TRUE;
}

bool ShowCalendar(short sYear,unsigned short wMonth,short iDay)
{
	//要显示一个月的月历，有以下几个要点：
	//1.该月1日的星期
	//2.该月的总天数
	//3.该月1日对应的农历以及农历月大小，有时甚至需要知道下个月甚至下下个月的大小
	int iDayofweek_1st=0,top = 160;//该月1日星期
	short sNYear=0;
	unsigned short wNMonth=0,wNDay=0;//农历年、月、日
	bool bNLeapMonth=FALSE;//闰月标志
	int iDaysofmonth=0;//公历月总天数
	int iNDaysofmonth=0;//农历月总天数
	int iPos=0,iGDayIdx=1,iNDayIdx=0,iNindex=1;//输出位置、公历日，农历日，农历输出天数（同步iDaysofmonth）
	int iFillFlag=0;//填充标志，0为公历星期填充，1为农历星期填充，2为公历填充，3为农历填充
	iDayofweek_1st=GetDayOfWeek(sYear,wMonth,1);//取得1日的星期

	if (iDayofweek_1st==-1)
	{
		return FALSE;//输入年月有误
	}
	iDaysofmonth=GetDaysOfMonth(sYear,wMonth);//得到本月总天数
	Gongli2Nongli(sYear,wMonth,1,&sNYear,&wNMonth,&wNDay,&bNLeapMonth);//得到公历1日的农历
	iNDaysofmonth=LunarGetDaysofMonth(sNYear,wNMonth,bNLeapMonth);//得到农历月总天数
	iNDayIdx=wNDay;//取出农历日
	if (iNDaysofmonth==0)
	{
		return FALSE;
	}
	printf("\t\t%d年%d月 农历 ",sYear,wMonth);
	if (bNLeapMonth==1)
	{
		printf("闰");
	}
	if (iNDaysofmonth==29)
	{
		printf("%s（小）\n\n",Yueming[wNMonth-1]);
	}else if (iNDaysofmonth==30)
	{
		printf("%s（大）\n\n",Yueming[wNMonth-1]);
	}
	for (iPos=0;iPos<7;iPos++)
	{
		printf("%s\t",Xingqi[iPos]);
    epd_drv1.DrawUTF((iPos)*32+16 , top, Xingqi[iPos], 1); 
	}
  printf("Calendar_day = ",iDay);
	printf("\n_______________________________________________________\n\n");
  //epd_drv.Buf_DrawLine(0, 140, 140, 140);
	iPos=0;
	while (iNindex<=iDaysofmonth)
	{
		if (iFillFlag==0||iFillFlag==2)
		{
			if (iFillFlag==0)
			{
				while(iPos<iDayofweek_1st)//直到星期填充完
				{
					printf("\t");
					iPos++;//位置增加
				}
			}
       if (iGDayIdx == iDay) {
          user_area1.top =top+10 ;
          user_area1.left = (iPos)*32;
          user_area1.width = 48;
          user_area1.height = 48;
          epd_drv1.User_Img_Tran(48,48,gImage_pic2,S1D13541_LD_IMG_4BPP,&user_area1,0); //显示圈圈gImage_circle1
            
        }
			if (G_HolidayShow(sYear,wMonth,iGDayIdx,(iPos)*32+12 , top+20)==0)
			{
				printf("  1_%2d\t",iGDayIdx);//输出农历
        epd_drv1.EPD_SetFount(FONT16);
        
        if (iGDayIdx < 10) {
          epd_drv1.DrawUTF((iPos)*32+20 , top+20, String(iGDayIdx), 1);
        }
        else {
          epd_drv1.DrawUTF((iPos)*32+16 , top+20, String(iGDayIdx), 1);
        }
			}
			iPos++;
			iGDayIdx++;
			if (iGDayIdx>iDaysofmonth)
			{
				printf("\n");
        top = top + 35;
				iPos=0;//位置重新回到开始
				iFillFlag=3;
			}else
			{
				if (iPos==7)//输出要换行了
				{
					printf("\n");
          top = top + 35;
					iPos=0;//位置重新回到开始
					if (iFillFlag==0)
					{
						iFillFlag=1;//切换到农历星期填充
					}else
					{
						iFillFlag=3;
					}
				}
			}
		}else if (iFillFlag==1||iFillFlag==3)
		{
			if (iFillFlag==1)
			{
				while(iPos<iDayofweek_1st)//直到星期填充完
				{
					printf("\t");
					iPos++;//位置增加
				}
			}
			
			if (iNDayIdx<=iNDaysofmonth)
			{
				if (L_HolidayShow(sNYear,wNMonth,iNDayIdx,bNLeapMonth,(iPos)*32+12 , top)==0)
				{
					if (iNDayIdx==1)
					{
						printf(" 2_%s",Yueming[wNMonth-1]);//公历月首为农历月首
            epd_drv1.EPD_SetFount(FONT12);
            epd_drv1.DrawUTF((iPos)*32+12 , top+20, Yueming[wNMonth-1], 1); 
						if (iNDaysofmonth==30)
						{
							printf("大\t");
						}else
						{
							printf("小\t");
						}
					}else
					{
						printf(" 3_%s\t",Riming[iNDayIdx-1]);//没有节日、节气等杂项输出时，输出农历日名
            //printf("top=%d",top);
            epd_drv1.EPD_SetFount(FONT12);
            epd_drv1.DrawUTF((iPos)*32+12 , top, Riming[iNDayIdx-1], 1);
					}
				}
			}else
			{
				Gongli2Nongli(sYear,wMonth,iNindex,&sNYear,&wNMonth,&wNDay,&bNLeapMonth);//算出农历下一个月
				iNDaysofmonth=LunarGetDaysofMonth(sNYear,wNMonth,bNLeapMonth);//重新得到农历月总天数
				iNDayIdx=wNDay;
				if (L_HolidayShow(sNYear,wNMonth,iNDayIdx,bNLeapMonth,(iPos)*32+12 , top)==0)
				{
					//没有节日、节气等杂项输出时，输出农历月名
					if (bNLeapMonth==1)
					{
						printf("闰");
					}
					printf("4_%s",Yueming[wNMonth-1]);
          epd_drv1.DrawUTF((iPos)*32+12 , top, Yueming[wNMonth-1], 1);
					if (iNDaysofmonth==29)
					{
						printf("小");
					}else if (iNDaysofmonth==30)
					{
						printf("大");
					}
					if (bNLeapMonth==0)
					{
						printf("\t");
					}
				}
			}
			iPos++;
			iNDayIdx++;//农历日增加
			iNindex++;//农历已填充天数增加
			if (iPos==7)
			{
				if (!(iGDayIdx>=iDaysofmonth&&(iDaysofmonth+iDayofweek_1st)%7==0))
				{
					printf("\n\n");
				}
				iPos=0;
				iFillFlag=2;
			}
		}
	}
  
	printf("\n_______________________________________________________\n\n");
	return TRUE;
}

unsigned short G_HolidayShow(short sYear,unsigned short wMonth,unsigned short wDay,short left,short top)
{
	unsigned short wJQ_date[2]={0};//节气编号
	int iDayOrdial=0;//日期的年内序数
	int ET_index=sYear-START_YEAR;//数九、梅雨及三伏的年份索引
	int iDayofweek_1st=0;//月首星期
	int iDongzhi=0;//冬至
	unsigned short wrumei=0,wchumei=0,wchufu=0,wmofu=0;
	short sshujiu=0;
	int i=0;
	epd_drv1.EPD_SetFount(FONT12);
/*	if (GetJieQi(sYear,wMonth,wJQ_date)==1)
	{
		if (wJQ_date[0]==wDay)
		{
			printf(" %s\t",Jieqi[((wMonth<<1)+20)%24]);//该月第一个节气
      epd_drv1.DrawUTF(left , top,Jieqi[((wMonth<<1)+20)%24], 1);
			return 1;
		}else if (wJQ_date[1]==wDay)
		{
			printf(" %s\t",Jieqi[((wMonth<<1)+21)%24]);//该月第二个节气
      epd_drv1.DrawUTF(left , top,Jieqi[((wMonth<<1)+21)%24], 1);
			return 1;
		}
	}*/
	GetDayOrdinal(sYear,12,wJQ_date[1],&iDongzhi);//转换到年内序数
	if (GetDayOrdinal(sYear,wMonth,wDay,&iDayOrdial))
	{
		/*if (iDayOrdial==wExtermSeason[ET_index+5])
		{
			printf("[%s]\t",Shujiu[0]);//一九（即冬至，不需要判断了，上面冬至已经优先输出了）
			return 1;
		}else */
		GetExtremeSeason(sYear,&sshujiu,&wchufu,&wmofu);
		GetMeiYu(sYear,&wrumei,&wchumei);
		if (iDayOrdial==iDongzhi+9)
		{
			printf("[%s]\t",Shujiu[1]);//二九
      epd_drv1.DrawUTF(left , top,Shujiu[1], 1);
			return 2;
		}
		if (iDayOrdial>=(sshujiu+9)&&iDayOrdial<sshujiu+81)
		{
			for (i=0;i<8;i++)
			{
				if (iDayOrdial==sshujiu+(i+1)*9)
				{
					printf("[%s]\t",Shujiu[i+1]);//三九至九九
          epd_drv1.DrawUTF(left , top,Shujiu[i+1], 1);
					return 1;
				}
			}
		}else if (iDayOrdial==wrumei)
		{
			printf("[%s]\t",Meiyu[0]);//入梅
      epd_drv1.DrawUTF(left , top,Meiyu[0], 1);
			return 1;
		}else if (iDayOrdial==wchumei)
		{
			printf("[%s]\t",Meiyu[1]);//出梅
      epd_drv1.DrawUTF(left , top,Meiyu[1], 1);
			return 1;
		}else if (iDayOrdial==wchufu)
		{
			printf("[%s]\t",Sanfu[0]);//初伏
      epd_drv1.DrawUTF(left , top,Sanfu[0], 1);
			return 1;
		}else if (iDayOrdial==wchufu+10)
		{
			printf("[%s]\t",Sanfu[1]);//中伏
      epd_drv1.DrawUTF(left , top,Sanfu[1], 1);
			return 1;
		}else if (iDayOrdial==wmofu)
		{	printf("[%s]\t",Sanfu[2]);//末伏
      epd_drv1.DrawUTF(left , top,Sanfu[2], 1);
			return 1;
		}
	}
	if (wMonth==1)
	{
		if (wDay==1)
		{
			printf(" 元旦\t");
      epd_drv1.DrawUTF(left , top,"元旦", 1);
			return 1;
		}
	}else if (wMonth==2)
	{
		if (wDay==14)
		{
			printf("情人节\t");
      epd_drv1.DrawUTF(left , top, "情人节", 1);
			return 1;
		}
	}else if (wMonth==3)
	{
		switch (wDay)
		{
		case 5:
			if (sYear>=1963)
			{
				printf("学雷锋日");
        epd_drv1.DrawUTF(left , top,"学雷锋日", 1);
				return 1;
			}
			break;
		case 8:
			if (sYear>=1900)
			{
				printf("妇女节\t");
        epd_drv1.DrawUTF(left , top,"妇女节", 1);
				return 1;
			}
			break;
		case 12:
			if (sYear>=1928)
			{
				printf("植树节\t");
        epd_drv1.DrawUTF(left , top,"植树节", 1);
				return 1;
			}
			break;
		case 15:
			if (sYear>=1983)
			{
				printf("消权日\t");
        epd_drv1.DrawUTF(left , top,"消权日", 1);
				return 1;
			}
			break;
		default:
			break;
		}
	}else if (wMonth==4)
	{
		if (wDay==1)
		{
			printf("愚人节\t");
      epd_drv1.DrawUTF(left , top,"愚人节", 1);
			return 1;
		}
	}else if (wMonth==5)
	{
		if (sYear>=1872)
		{
			iDayofweek_1st=GetDayOfWeek(sYear,wMonth,1);//取月首星期
			if ((wDay+iDayofweek_1st-1)%7==0&&((wDay+iDayofweek_1st-1+(iDayofweek_1st==0?7:0))/7)==2)//5月第二个周日母亲节
			{
				printf("母亲节\t");
        epd_drv1.DrawUTF(left , top,"母亲节", 1);
				return 1;
			}
		}
		switch (wDay)
		{
		case 1:
			if (sYear>=1890)
			{
				printf("劳动节\t");
        epd_drv1.DrawUTF(left , top,"劳动节", 1);
				return 1;
			}
			break;
		case 4:
			if (sYear>=1919)
			{
				printf("青年节\t");
        epd_drv1.DrawUTF(left , top,"青年节", 1);
				return 1;
			}
			break;
		case 12:
			if (sYear>=1912)
			{
				printf("护士节\t");
        epd_drv1.DrawUTF(left , top,"护士节", 1);
				return 1;
			}
			break;
		case 31:
			if (sYear>=1989)
			{
				printf("无烟日\t");
        epd_drv1.DrawUTF(left , top,"无烟日", 1);
				return 1;
			}
			break;
		default:
			break;
		}
	}else if (wMonth==6)
	{
		if (sYear>=1910)
		{
			iDayofweek_1st=GetDayOfWeek(sYear,wMonth,1);//取月首星期
			if ((wDay+iDayofweek_1st-1)%7==0&&((wDay+iDayofweek_1st-1+(iDayofweek_1st==0?7:0))/7)==3)//6月第三个周日父亲节
			{
				printf("父亲节\t");
        epd_drv1.DrawUTF(left , top,"父亲节", 1);
				return 1;
			}
		}
		switch (wDay)
		{
		case 1:
			if (sYear>1949)
			{
				printf("儿童节\t");
        epd_drv1.DrawUTF(left , top,"儿童节", 1);
				return 1;
			}
			break;
		case 5:
			if (sYear>=1972)
			{
				printf("环境日\t");
        epd_drv1.DrawUTF(left , top,"环境日", 1);
				return 1;
			}
			break;
		case 26:
			if (sYear>=1987)
			{
				printf("禁毒日\t");
        epd_drv1.DrawUTF(left , top,"禁毒日", 1);
				return 1;
			}
			break;
		default:
			break;
		}
	}else if (wMonth==7)
	{
		switch (wDay)
		{
		case 1:
			if (sYear>=1921)
			{
				printf("中共诞辰");
        epd_drv1.DrawUTF(left , top,"党的生日", 1);
				return 1;
			}
			break;
		case 7:
			if (sYear>=1937)
			{
				printf("抗战纪念");
        epd_drv1.DrawUTF(left , top,"抗战纪念", 1);
				return 1;
			}
			break;
		default:
			break;
		}
	}else if (wMonth==8)
	{
		if (wDay==1)
		{
			if (sYear>=1933)
			{
				printf("建军节\t");
        epd_drv1.DrawUTF(left , top,"建军节", 1);
				return 1;
			}
		}
	}else if (wMonth==9)
	{
		switch (wDay)
		{
		case 3:
			if (sYear>=1945)
			{
				printf("抗战胜利");
        epd_drv1.DrawUTF(left , top,"抗战胜利", 1);
				return 1;
			}
			break;
		case 10:
			if (sYear>=1985)
			{
				printf("教师节\t");
        epd_drv1.DrawUTF(left , top,"教师节", 1);
				return 1;
			}
			break;
		default:
			break;
		}
	}else if (wMonth==10)
	{
		if (wDay==1)
		{
			if (sYear>=1949)
			{
				printf(" 国庆\t");
        epd_drv1.DrawUTF(left , top,"国庆", 1);
				return 1;
			}
		}
	}else if (wMonth==11)
	{
		if (sYear>=1941)
		{
			iDayofweek_1st=GetDayOfWeek(sYear,wMonth,1);//取月首星期
			if ((wDay+iDayofweek_1st-1)%7==4&&((wDay+iDayofweek_1st-1+(iDayofweek_1st==4?7:0))/7)==4)//11月第四个周四感恩节
			{
				printf("感恩节\t");
        epd_drv1.DrawUTF(left , top,"感恩节", 1);
				return 1;
			}
		}
		if (wDay==1)
		{
			printf("万圣节\t");
      epd_drv1.DrawUTF(left , top,"万圣节", 1);
			return 1;
		}
	}else if (wMonth==12)
	{
		switch (wDay)
		{
		case 10:
			if (sYear>=1948)
			{
				printf("人权日\t");
        epd_drv1.DrawUTF(left , top,"人权日", 1);
				return 1;
			}
			break;
		case 12:
			if (sYear>=1936)
			{
				printf("西安事变");
        epd_drv1.DrawUTF(left , top,"西安事变", 1);
				return 1;
			}
			break;
		case 24:
			printf("平安夜\t");
      epd_drv1.DrawUTF(left , top,"平安夜", 1);
			return 1;
		case 25:
			printf("圣诞节\t");
      epd_drv1.DrawUTF(left , top,"圣诞节", 1);
			return 1;
		default:
			break;
		}
	}
	return 0;
}

bool L_HolidayShow(int sLYear,int iLMonth,int iLDay,int iLeapMonth,short left,short top)
{
	int LeapMonth=LunarGetLeapMonth(sLYear);//取该年农历闰月
	int DayofLM_12=0;//农历十二月的总天数
  epd_drv1.EPD_SetFount(FONT12);
	if (iLeapMonth==0)
	{
		if (iLMonth==1)
		{
			switch (iLDay)
			{
			case 1:
				printf(" 春节\t");
        epd_drv1.DrawUTF(left , top,"春节", 1);
				return TRUE;
			case 15:
				printf("元宵节\t");
        epd_drv1.DrawUTF(left , top,"元宵", 1);
				return TRUE;
			default:
				break;
			}
		}else if (iLMonth==2)
		{
			if (iLDay==2)
			{
				printf("春龙节\t");
        epd_drv1.DrawUTF(left , top,"龙抬头", 1);
				return TRUE;
			}
		}else if (iLMonth==5)
		{
			if (iLDay==5)
			{
				printf("端午节\t");
        epd_drv1.DrawUTF(left , top,"端午", 1);
				return TRUE;
			}
		}else if (iLMonth==7)
		{
			if (iLDay==7)
			{
				printf(" 七夕\t");
        epd_drv1.DrawUTF(left , top,"七夕", 1);
				return TRUE;
			}else if (iLDay==15)
			{
				printf("中元节\t");
        epd_drv1.DrawUTF(left , top,"中元", 1);
				return TRUE;
			}
		}else if (iLMonth==8)
		{
			if (iLDay==15)
			{
				printf("中秋节\t");
        epd_drv1.DrawUTF(left , top,"中秋", 1);
				return TRUE;
			}
		}else if (iLMonth==9)
		{
			if (iLDay==9)
			{
				printf("重阳节\t");
        epd_drv1.DrawUTF(left , top,"重阳", 1);
				return TRUE;
			}
		}else if (iLMonth==10)
		{
			if (iLDay==1)
			{
				printf("祭祖节\t");
        epd_drv1.DrawUTF(left , top,"祭祖", 1);
				return TRUE;
			}else if (iLDay==15)
			{
				printf("下元节\t");
        epd_drv1.DrawUTF(left , top,"下元", 1);
				return TRUE;
			}
		}else if (iLMonth==12)
		{
			if (LeapMonth!=12)
			{
				DayofLM_12=LunarGetDaysofMonth(sLYear,12,0);//非闰十二月
				if (iLDay==8)
				{
					printf("腊八节\t");
          epd_drv1.DrawUTF(left , top,"腊八节", 1);
					return TRUE;
				}else if (iLDay==23)
				{
					printf(" 小年\t");
          epd_drv1.DrawUTF(left , top,"小年", 1);
					return TRUE;
				}else if (iLDay==DayofLM_12)//农历十二月的最后一天是除夕
				{
					printf(" 除夕\t");
          epd_drv1.DrawUTF(left , top,"除夕", 1);
					return TRUE;
				}
			}
		}
	}else
	{
		if (iLeapMonth==LeapMonth)
		{
			if (iLeapMonth==12)
			{
				DayofLM_12=LunarGetDaysofMonth(sLYear,12,1);//闰十二月
				if (iLDay==8)
				{
					printf("腊八节\t");
          epd_drv1.DrawUTF(left , top,"腊八节", 1);
					return TRUE;
				}else if (iLDay==23)
				{
					printf(" 小年\t");
          epd_drv1.DrawUTF(left , top,"小年", 1);
					return TRUE;
				}else if (iLDay==DayofLM_12)//农历十二月的最后一天是除夕
				{
					printf(" 除夕\t");
          epd_drv1.DrawUTF(left , top,"除夕", 1);
					return TRUE;
				}
			}
		}
	}
	return FALSE;//没有节日等
}

void ShowMenu()
{
	printf("**************************************\n");
	printf("*                                    *\n");
	printf("*             万年历查询             *\n");
	printf("*                                    *\n");
	printf("*    %d年至%d年（不含%d年）    *\n",START_YEAR,END_YEAR,END_YEAR);
	printf("*                                    *\n");
	printf("**************************************\n");
	printf("\n\n");
	printf("◆1. 月历显示\n◆2. 公历转农历\n◆3. 农历转公历\n◆4. 节气查询\n\n\n");
	printf("请选择：");
}

void GL2NL(int sYear,int wMonth,int wDay)
{
	//unsigned short wMonth=0,wDay=0,iLMonth=0,iLDay=0,iDayofweek=0;
  unsigned short iLMonth=0,iLDay=0,iDayofweek=0;
	//short sYear=0,sLYear=0;
  short sLYear=0;
	bool bLLeapMonth=FALSE;
	int ret=0;
		printf("◆2. 公历转农历\n\n");
	do 
	{
		printf("请输入公历日期，年 月 日：>");
		//scanf_s("%d %d %d",&sYear,&wMonth,&wDay);
		iDayofweek=GetDayOfWeek(sYear,wMonth,wDay);
		ret=Gongli2Nongli(sYear,wMonth,wDay,&sLYear,&iLMonth,&iLDay,&bLLeapMonth);
		if (ret==0)
		{
			printf("\n您的输入有误，");
		}
	} while (ret==0);
	if (bLLeapMonth)
	{
		printf("%d年%d月%d日转为农历为：%s%s（%s）年 闰%s %s",sYear,wMonth,wDay,Tiangan[(sLYear - 4)%10],Dizhi[(sLYear - 4)%12],Shengxiao[(sLYear - 4)%12],Yueming[iLMonth-1],Riming[iLDay-1]);
	}else
	{
		printf("%d年%d月%d日转为农历为：%s%s（%s）年 %s %s",sYear,wMonth,wDay,Tiangan[(sLYear - 4)%10],Dizhi[(sLYear - 4)%12],Shengxiao[(sLYear - 4)%12],Yueming[iLMonth-1],Riming[iLDay-1]);
	}
	printf("  %s\n\n",Xingqi[iDayofweek]);
	
	
}

void NL2GL(int sLYear,int iLMonth,int iLDay,int bLLeapMonth)
{
	//unsigned int wMonth=0,wDay=0,iLMonth=0,iLDay=0,iDayofweek=0;
  unsigned int wMonth=0,wDay=0,iDayofweek=0;
	int sYear=0;
	int ret=0;
	printf("◆3. 农历转公历\n\n");
	do 
	{
		printf("请输入农历日期以及是否闰月（闰月用1表示，非闰月用0表示）。\n年 月 日 是否闰月：>");
		//scanf_s("%d %d %d %d",&sLYear,&iLMonth,&iLDay,&bLLeapMonth);
		ret=Nongli2Gongli(sLYear,iLMonth,iLDay,bLLeapMonth,(short *)&sYear,(unsigned short *)&wMonth,(unsigned short *)&wDay);
		if (ret==0)
		{
			printf("\n您的输入有误，");
		}
	} while (ret==0);
	if (bLLeapMonth)
	{
		printf("农历 %d年 闰%s %s 转为公历为 %d年%d月%d日\n\n",sLYear,Yueming[iLMonth-1],Riming[iLDay-1],sYear,wMonth,wDay);
	}else
	{
		printf("农历 %d年 %s %s 转为公历为 %d年%d月%d日\n\n",sLYear,Yueming[iLMonth-1],Riming[iLDay-1],sYear,wMonth,wDay);
	}
	
	
}

void RL(short iYear,short iMonth,short iDay)
{
	//int iYear=0;
	//int iMonth=0;
	bool ret=FALSE;
	
	printf("◆1. 月历显示\n\n");
	do 
	{
		//scanf_s("%d %d",&iYear,&iMonth);
		ret=ShowCalendar(iYear,iMonth,iDay);
		ret=TRUE;
		if (ret==FALSE)
		{
			printf("\n您的输入有误，");
		}
	} while (ret==FALSE);
	
	
}
