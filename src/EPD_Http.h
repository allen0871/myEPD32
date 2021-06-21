#ifndef _EPD_HTTP_H_
#define _EPD_HTTP_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <I2C_BM8563.h>
 
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "EPD4IN_Device.h" 
#include "time.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
 //////////////////////////////////////////////////////////
int Http_Post(String webp, String sendjson);


struct EPD_Device_Struct
{ 
  char devsn[16];           //addap时更新  获取的MAC地址
  
  bool is_init;             //是否初始化（获取到TOKEN认为初始化成功）
  uint16_t  ScreenSlect;         //显示选择，1 热点新闻 2留言  3todo   4 番茄  5 日历   默认为3 todo 
 
  char token_str[128];      //token字符串，reg后填写

  bool get_config;        //配置是否有效 
  uint8_t theme;         //主题编号
  uint8_t screenratio;    //屏幕对比度
  char Device_Name[16];    //设备名称   
  
  bool weather_result;      //天气预报
  char Weather_City[12];    //
  int Weather_Tmp_High;
  int Weather_Tmp_Low;
  int Weather_Tmp_Real;
  char Weather_Type[16];
  uint16_t  Weather_TypeCode;
  uint16_t Weather_hum;
  char Weather_winddir[16];
  uint16_t Weather_windpwr;
  char Weather_uptime[8];


  bool HotNews_result;         //   
  uint16_t  HotNews_count;
  uint16_t  HotNews_point;   //当前显示位置
  char HotNews_List[13][64];// 最大13条新闻
  char HotNews_Uptime[8];
  
  char Get_english[512];
  char Get_china[512];

  
  bool Todo_result;         //
  uint16_t  Todo_count;
  uint16_t  Todo_point;   //当前显示位置
  char Todo_New_List[5][64];//   
  char Todo_Uptime[16];

  bool Msg_result;         //  
  char Msg_body[128];
  char Msg_code[32];
  bool Msg_Read;
  
  bool Calendar_result;
  char Calendar_animalsYear[8];
  char Calendar_lunarYear[12];
  char Calendar_suit[256];
  char Calendar_avoid[256];
  char Calendar_lunar[16];
  char Calendar_holiday[16];
};

bool addap(void);

void setUpHttpClient();
bool AutoConfig();
int Auto_Web_Config(void);
bool WIFI_Disconnect(void);
void wifi_Config();
void RestoreWifi();

int WebHandle(void);

int Http_Set_Rtc(void);
int Http_Get_Token(void);
int Http_Get_Config(void);
int Http_Get_RealWeather(void);
int Http_Get_Calendar(void);
uint16_t  NewGetRtcMinute(void);
uint16_t  NewGetRtcHour(void);
int Http_Get_TodoList(void);
int Http_Get_HotNews(void);
int Http_Get_english(void);
int Http_Get_LeaveMsg(void);
int Http_Post(String webp, String sendjson);
int GetNews(void);
int Http_Get_double(void);
#endif
