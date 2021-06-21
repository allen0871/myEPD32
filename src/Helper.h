#ifndef _HELPER_H_
#define _HELPER_H_

void ShowMenu(void);//显示菜单
void GL2NL(int sYear,int wMonth,int wDay);//公历转农历
void NL2GL(int sLYear,int iLMonth,int iLDay,int bLLeapMonth);//农历转公历
void RL(short iYear,short iMonth,short iDay);//显示一个月的日历
//bool ShowCalendar(short sYear,unsigned short wMonth);
void JQ(int Year,int Index);//节气查询

#endif //_HELPER_H_
