#include "EPD4IN_Device.h"
#include "ClosedCube_SHT31D.h"
#include "Helper.h"
//
//
EPD4INC epd_drv;
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);
RTC_DATA_ATTR     I2C_BM8563_DateTypeDef dateStruct;
RTC_DATA_ATTR     I2C_BM8563_TimeTypeDef timeStruct;
SHTSensor sht;
ClosedCube_SHT31D sht3xd; 

extern RTC_DATA_ATTR EPD_Device_Struct epd_rtc_data;
char buff[128];

const char softver[] = "V2.12YM";
struct pl_area;
struct pl_area user_area;


//QRCode qrcode;

const int wdtTimeout = 30000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}


uint8_t EPD4IN_Device:: IIC_Device_Dected(uint8_t *device_id)
{
  Serial.println ("I2C scanner. Scanning ...");
  uint8_t count = 0;
  Wire.begin();
  for (uint8_t i = 8; i < 120; i++) {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0) {
      Serial.print ("Found address: ");
      device_id[count] = i;
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1); // maybe unneeded?
    } // end of good response
  } // end of for loop
  return count;
}
uint8_t EPD4IN_Device:: SHT_Init(void)
{
  Wire1.begin(21, 22);
  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }

}
uint8_t EPD4IN_Device:: RTC_Init(void)
{

  Wire1.begin(21, 22);
  rtc.begin();
}
//初始化EPD
void EPD4IN_Device::EPD_Device_Init(void)
{
  Serial.println("----------EPD_Device_Init()-----------");

  Serial.println("Init Epd Port");
  epd_drv.EPD4INC_Port_init();
  epd_drv.EPD_CLK_EX();
  Serial.println("Epd Power Up");

  delay(10);
  epd_drv.s1d135xx_soft_reset();
  delay(10);
  epd_drv.epson_epdc_init();
  delay(5);
  epd_drv.s1d135xx_set_epd_power(1);
  delay(10);
  Serial.println("----------EPD_Device_Init()Return -----------");
}
//初始化EPD
//初始化页面，完成开机页面显示
void EPD4IN_Device::Start_Init_EPD(void)
{
  EPD_Device_Init();

  user_area.top = 180;
  user_area.left = 35;
  user_area.width = 120;
  user_area.height = 16;

  //刷新背景
  //  epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_InitPage);
  //刷新背景
 epd_drv.EPD_Update_Full(48000, S1D13541_LD_IMG_4BPP, imagetest);


  //写入ID
  //  sprintf(buff, "Device ID=%llx", ESP.getEfuseMac());
  //  epd_drv.EPD_SetFount(FONT16);
  //  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);
  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  Serial.println("04 Epd updat background Over");
  delay(900);

  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();


  Serial.println("----------Start_Init_EPD()Return -----------");
}
void EPD4IN_Device::SEt_EPD_NeedConfig(void)
{
  Serial.println("----------SEt_EPD_NeedConfig()-----------");
  Serial.println("DrawStartScreen");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟


  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
  //通过BUF处理图层
  epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, gImage_needinit);
  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);

  user_area.top = 270;
  user_area.left = 30;
  user_area.width = 120;
  user_area.height = 16;

  sprintf(buff, "Devide ID=%llx", ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  Serial.println("04 Epd updat background Over");
  delay(900);

  Serial.println("05 shuban delay over");
  Serial.println("06 Epd Go To sleep");
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();


  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  Serial.println("----------SEt_EPD_NeedConfig()-----------");
}


//wifisetimg

void EPD4IN_Device::SEt_EPD_WIFICONFIG(void)
{
  Serial.println("DrawStartScreen");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟

  epd_drv.s1d135xx_set_epd_power(1);
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
  //通过BUF处理图层
  epd_drv.EPD_Update_Full(12000, S1D13541_LD_IMG_1BPP, wifisetimg);
  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);

  user_area.top = 350;
  user_area.left = 30;
  user_area.width = 120;
  user_area.height = 16;

  sprintf(buff, "Devide ID=%llx", ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  delay(10);
  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);
  Serial.println("04 Epd updat background Over");
  delay(900);

  Serial.println("05 shuban delay over");
  Serial.println("06 Epd Go To sleep");
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();


  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();


}
//初始化EPD
void EPD4IN_Device::Start_EPD_Show(void)
{
  DrawStartScreen();
}

uint16_t EPD4IN_Device::DrawStartScreen(void)
{
  uint16_t returntime;
  Serial.println("----------DrawStartScreen-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化

  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);


  //通过BUF处理图层
  epd_drv.Buf_Clear();
  epd_drv.Buf_DrawLine(140, 0, 140, 240);
  epd_drv.Buf_DrawLine(0, 140, 140, 140);



  epd_drv.Buf_UpdateFull(1);
  //ico_sunny
  //int EPD4INC::User_Img_Tran(uint16_t height, uint16_t width, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv)

  user_area.top = 40 ;
  user_area.left = 160;
  user_area.width = 64;
  user_area.height = 64;

  uint8_t  testicostr[512];

  SearchWeatherIco(testicostr, 508);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);
  user_area.top += 64;
  SearchWeatherIco(testicostr, 301);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);
  user_area.top += 64;
  SearchWeatherIco(testicostr, 302);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);
  user_area.top += 64;
  SearchWeatherIco(testicostr, 317);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);
  user_area.top += 64;
  SearchWeatherIco(testicostr, 901);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);

  user_area.top = 0;
  user_area.left = 0;
  user_area.width = 120;
  user_area.height = 16;

  // sprintf(buff, "2021年1月21日 星期四",bootcounter++);


  Serial.println("Set RTC Buf  ");
  SetRtcBuf(buff);

  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //日期显示

  user_area.top = 88;
  user_area.left = 16;

  Serial.println("Set SHT Buf  ");
  SetShtBuf(buff);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示
  epd_drv.DrawTime(10, 30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

  user_area.top = 160;
  user_area.left = 30;
  sprintf(buff, "Devide ID=%llx", ESP.getEfuseMac());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号

  user_area.top = 190;
  user_area.left = 30;
  sprintf(buff, "BAT VOL is=%2.1fV", GetBatVol());
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //显示设备序列号



  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);
  delay(2);

  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);

  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
  return   SetRtcBuf(buff);
}
//全屏幕刷新
//完成屏幕布局，刷新所有界面
uint16_t EPD4IN_Device::DrawFullScreen(void)
{
  uint16_t returntime;
  char buff[512];
  Serial.println("----------DrawFullScreen-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  //  delay(2);
  if ((epd_drv.EPD_Check_ISinit() != 0x300)&&(epd_drv.EPD_Check_ISinit() != 0x320)) //判断旋转寄存器是否是设定值，若不是，重启屏幕。
  {

    Serial.printf("\n\nDevice check error  System will  restart \n\n");

    ESP.restart();  //复位esp32
  }


  Serial.printf("In DrawFullScreen Check Rota Reg is 0x%x\n", epd_drv.EPD_Check_ISinit());

  //通过BUF处理图层
  epd_drv.Buf_Clear();
  epd_drv.Buf_DrawLine(135, 0, 135, 240);
  epd_drv.Buf_DrawLine(152, 0, 152, 240);

  epd_drv.Buf_DrawLine(0, 140, 140, 140);


  //  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  //  qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://mp.weixin.qq.com/a/~-2VsaMDvwQte31K6TOm05w~~");
  //      for (uint8_t y = 0; y < qrcode.size; y++)
  //      {
  //        for (uint8_t x = 0; x < qrcode.size; x++)
  //        {
  //            if(qrcode_getModule(&qrcode, x, y))
  //               {
  //
  //                epd_drv.Buf_SetPix(2*x+120,2*y+300);
  //                epd_drv.Buf_SetPix(2*x+1+120,2*y+300);
  //                epd_drv.Buf_SetPix(2*x+120,2*y+1+300);
  //                epd_drv.Buf_SetPix(2*x+1+120,2*y+1+300);
  //               }
  //
  //        }
  //      }
  //  qrcode_initText(&qrcode, qrcodeData, 3, 0, "1016f8bd9e7c");
  //      for (uint8_t y = 0; y < qrcode.size; y++)
  //      {
  //        for (uint8_t x = 0; x < qrcode.size; x++)
  //        {
  //            if(qrcode_getModule(&qrcode, x, y))
  //               {
  //
  //                epd_drv.Buf_SetPix(2*x+20,2*y+300);
  //                epd_drv.Buf_SetPix(2*x+1+20,2*y+300);
  //                epd_drv.Buf_SetPix(2*x+20,2*y+1+300);
  //                epd_drv.Buf_SetPix(2*x+1+20,2*y+1+300);
  //               }
  //
  //        }
  //      }

  epd_drv.Buf_UpdateFull(1);

  user_area.top = 32 ;
  user_area.left = 160;
  user_area.width = 64;
  user_area.height = 64;
  //天气图标
  uint8_t  testicostr[512];
  SearchWeatherIco(testicostr, epd_rtc_data.Weather_TypeCode);
  epd_drv.User_Img_Tran(64, 64, testicostr, S1D13541_LD_IMG_1BPP, &user_area, 1);
  //城市和实时温度
  user_area.top = 20;
  user_area.left = 160;
  user_area.width = 32;
  user_area.height = 16;
  memset(buff, 0, 128);
  sprintf(buff, "%s%d℃", epd_rtc_data.Weather_City, epd_rtc_data.Weather_Tmp_Real);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //城市名称
  //天气  温度范围
  user_area.top = 96;
  user_area.left = 148;
  user_area.width = 32;
  user_area.height = 16;
  memset(buff, 0, 128);
  sprintf(buff, "%s %d~%d℃", epd_rtc_data.Weather_Type, epd_rtc_data.Weather_Tmp_Low, epd_rtc_data.Weather_Tmp_High);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //

  //天气  风力

  if (epd_rtc_data.Weather_windpwr > 0)
  {
    user_area.top = 116;
    user_area.left = 160;
    user_area.width = 32;
    user_area.height = 16;
    memset(buff, 0, 128);
    sprintf(buff, "%s%d级", epd_rtc_data.Weather_winddir, epd_rtc_data.Weather_windpwr);
    epd_drv.EPD_SetFount(FONT16);
    epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //

  }


  //年月日 日期显示
  user_area.top = 0;
  user_area.left = 0;
  user_area.width = 120;
  user_area.height = 16;
  Serial.println("Set RTC Buf  ");
  SetRtcBuf(buff);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //日期显示
  //温湿度显示
  user_area.top = 78;
  user_area.left = 16;
  Serial.println("Set SHT Buf  ");
  SetShtBuf(buff);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示
  epd_drv.DrawTime(10, 30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);

  //阴历和节假日
  user_area.top = 98;
  if (epd_rtc_data.Calendar_holiday[0] == NULL)
    user_area.left = 40;
  else
    user_area.left = 12;
  memset(buff, 0, 128);
  sprintf(buff, "%s %s", epd_rtc_data.Calendar_lunar, epd_rtc_data.Calendar_holiday);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //
  //留言是否存在
  if (epd_rtc_data.Msg_result)
  {
    user_area.top = 116;
    user_area.left = 48;
    memset(buff, 0, 128);
    sprintf(buff, "有留言");
    epd_drv.EPD_SetFount(FONT16);
    epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //

  }



  //电池电压
  user_area.top = 0;
  user_area.left = 212;
  memset(buff, 0, 128);
  sprintf(buff, "%2.1fV", GetBatVol());
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //softver

  //softver
  user_area.top = 0;
  user_area.left = 100;
  memset(buff, 0, 128);
  sprintf(buff, "%s", softver);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //
  //设备名字
  user_area.top = 0;
  user_area.left = 150;
  memset(buff, 0, 128);
  sprintf(buff, "%s", epd_rtc_data.Device_Name);  //设备名字
  printf("设备名字%d", buff);
  epd_drv.EPD_SetFount(FONT12);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //

  //刷新时间
  TestRtc();
  epd_drv.DrawTime(10, 30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);
  //
  epd_drv.EPD_SetFount(FONT16);
  DrawFuntionArea(3);


  user_area.top = 162;
  user_area.left = 0;
  Serial.printf("\nScreenSlect=%d ", epd_rtc_data.ScreenSlect);
  //hot news 1
  if (epd_rtc_data.ScreenSlect == 1)
  {
    DrawFuntionArea(1) ;
    user_area.top = 162;
    user_area.left = 0;
    epd_drv.EPD_SetFount(FONT16);
    int totalLine = 0;
    for (int todoindx = 0; todoindx < epd_rtc_data.HotNews_count; todoindx++)
    {
      memset(buff, 0, 512);
      sprintf(buff, ">%s", epd_rtc_data.HotNews_List[todoindx]);
      //epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //
      int line = epd_drv.DrawUTF_hz(user_area.left , user_area.top, buff, 1, 4);
      user_area.top += 18*line;
      totalLine+=line;
      if(totalLine >= 12) {
        break;
      }
    }
  }
  //message
  if (epd_rtc_data.ScreenSlect == 2)
  {
    DrawFuntionArea(2) ;
    user_area.top = 162;
    user_area.left = 0;
    epd_drv.EPD_SetFount(FONT16);
    if (epd_rtc_data.Msg_result)
    {
      memset(buff, 0, 128);
      sprintf(buff, ">%s", epd_rtc_data.Msg_body);
      epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //
    }
  }
  //todo 3
  if (epd_rtc_data.ScreenSlect == 3)
  {
    DrawFuntionArea(3) ;
    user_area.top = 162;
    user_area.left = 0;
    epd_drv.EPD_SetFount(FONT16);
    for (int todoindx = 0; todoindx < epd_rtc_data.Todo_count; todoindx++)
    {
      memset(buff, 0, 128);
      sprintf(buff, ">%s", epd_rtc_data.Todo_New_List[todoindx]);
      epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //
      user_area.top += 18;
    }
  }
  //番茄时间 3
  if (epd_rtc_data.ScreenSlect == 4)
  {
    DrawFuntionArea(4) ;
    user_area.top = 162;
    user_area.left = 0;
    memset(buff, 0, 512);
    epd_drv.EPD_SetFount(FONT16);
     sprintf(buff, "%s", epd_rtc_data.Get_english);
     epd_drv.DrawUTF_lhb( user_area.left , user_area.top, buff, 1); 
     user_area.top += 96;   //
     memset(buff, 0, 512);
     sprintf(buff, "%s", epd_rtc_data.Get_china);  
     epd_drv.DrawUTF_hz( user_area.left , user_area.top, buff, 1); 
  }
  //日历 5
  if (epd_rtc_data.ScreenSlect == 5)
  {
    DrawFuntionArea(5) ;
  
    uint16_t rtcday = GetRtcDay();
    uint16_t rtcMonth = GetRtcMonth();
    Serial.printf("\nGetRtcDay%d ", rtcday);
    Serial.printf("\nrtcMonth%d ", rtcMonth);
    RL((short)dateStruct.year,(short)rtcMonth,(short)rtcday);
   // epd_drv.DrawCalendarDay(60, 180, rtcday, FONT64_NUM, 1); //写日历时间
  }

  ///////////////////////////////////////////////////////////////////////


  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);
  delay(5);

  epd_drv.EPD_UpdateUser(1, UPDATE_FULL, NULL);


  Serial.println("\n04 Epd updat  Over");
  delay(900);
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);

  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();


  return   SetRtcBuf(buff);
}
uint16_t EPD4IN_Device::Screen_PowerUp(void)
{

  Serial.println("----------Screen_PowerUp-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);
  delay(2);
}

//全屏幕刷新

uint16_t EPD4IN_Device::DrawFuntionArea(uint16_t showtag)
{
  epd_drv.EPD_SetFount(FONT16);
  user_area.top = 136;
  user_area.left = 0;
  user_area.height = 16;
  if (showtag == 1)
  {
    epd_drv.DrawUTF( user_area.left , user_area.top, " Todo ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 留言 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 热点 ", 0);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 日历 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 双语 ", 1);    //

  }
  if (showtag == 2)
  {

    epd_drv.DrawUTF( user_area.left , user_area.top, " 双语 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " Todo ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 留言 ", 0);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 热点 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 日历 ", 1);    //


  }
  if (showtag == 3)
  {
    epd_drv.DrawUTF( user_area.left , user_area.top, " 日历 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 双语 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " Todo ", 0);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 留言 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 热点 ", 1);    //
    user_area.left += 48;
  }
  if (showtag == 4)
  {
    epd_drv.DrawUTF( user_area.left , user_area.top, " 热点 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 日历 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 双语 ", 0);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " Todo ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 留言 ", 1);    //
    user_area.left += 48;
  }
  if (showtag == 5)
  {
    epd_drv.DrawUTF( user_area.left , user_area.top, " 留言 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 热点 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 日历 ", 0);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " 双语 ", 1);    //
    user_area.left += 48;
    epd_drv.DrawUTF( user_area.left , user_area.top, " Todo ", 1);    //
    user_area.left += 48;

  }

  user_area.top = 136;
  user_area.left = 0;
  user_area.height = 16;
  user_area.width = 240;
  // epd_drv.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area);
}
// 局部刷新时间
uint16_t EPD4IN_Device::DrawTimeUpdata(void)
{
  Serial.println("----------DrawTimeUpdata-----------");
  epd_drv.EPD4INC_Port_Reinit();      //SPI初始化
  epd_drv.EPD_CLK_EX();               //其中时钟
  epd_drv.s1d135xx_set_epd_power(1);
  delay(2);
  Serial.printf("In DrawTimeUpdata Check Rota Reg is 0x%x\n", epd_drv.EPD_Check_ISinit());
  user_area.top = 30;
  user_area.left = 10;
  user_area.width = 120;
  user_area.height = 48;
  TestRtc();  //更新时间
  epd_drv.DrawTime(10, 30, timeStruct.hours, timeStruct.minutes, FONT48_NUM, 1);
  epd_drv.EPD4INC_HVEN(epd_rtc_data.screenratio);

  //温湿度显示
  user_area.top = 78;
  user_area.left = 16;

  Serial.println("Set SHT Buf  ");
  SetShtBuf(buff);
  epd_drv.EPD_SetFount(FONT16);
  epd_drv.DrawUTF( user_area.left , user_area.top, buff, 1);    //RTC显示

  user_area.top = 30;
  user_area.left = 0;
  user_area.width = 136;
  user_area.height = 64;

  Serial.println("\n Epd UPDATE_PARTIAL_AREA updat start");
  epd_drv.EPD_UpdateUser(2, UPDATE_PARTIAL_AREA, &user_area);
  Serial.println("\n Epd UPDATE_PARTIAL_AREA updat  Over");
  
  delay(300);
  Serial.println("\n Epd UPDATE_PARTIAL_AREA delay  Over");
  epd_drv.EPD4INC_HVDISEN();
  epd_drv.s1d135xx_set_power_state(PL_EPDC_SLEEP);
  epd_drv.EPD_CLK_STOP();
}

void EPD4IN_Device::TestSht(void)
{
  Wire.begin();

  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }
  delay(1);
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  delay(5);
  if (sht.readSample()) {
    Serial.print("SHT:\n");
    Serial.print("  RH: ");
    Serial.print(sht.getHumidity(), 2);
    Serial.print("\n");
    Serial.print("  T:  ");
    Serial.print(sht.getTemperature(), 2);
    Serial.print("\n");
  } else {
    Serial.print("Error in readSample()\n");
  }

}

void EPD4IN_Device::SetShtBuf(char *shtbuf)
{
  delay(1);
  Serial.print("init SHT\n");
  Wire.begin();
  delay(5);
  sht.init();
  delay(5);
  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }
  delay(1);
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  delay(5);
  sht.readSample();
  delay(5);
  if (sht.readSample()) {
    Serial.print("SHT:\n");
    Serial.print("  RH: ");
    Serial.print(sht.getHumidity(), 2);
    Serial.print("\n");
    Serial.print("  T:  ");
    Serial.print(sht.getTemperature(), 2);
    Serial.print("\n");
    sprintf(shtbuf, "%2.1f%cRH %2.1f℃",
            sht.getHumidity(),
            '%',
            sht.getTemperature()
           );



  } else {

  sht3xd.begin(0x44);
  if (sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ) != SHT3XD_NO_ERROR)
    Serial.println("[ERROR] Cannot start periodic mode");
  SHT31D result = sht3xd.periodicFetchData();  
  if (result.error == SHT3XD_NO_ERROR) {
    Serial.print("T=");
    Serial.print(result.t);
    Serial.print("C, RH=");
    Serial.print(result.rh);
    Serial.println("%");
    sprintf(shtbuf, "%2.1f%cRH %2.1f℃",
                result.rh,
                '%',
                result.t
           );
  } else {
    Serial.print(": [ERROR] Code #");
    Serial.println(result.error);
  }





   
  }

}
void  EPD4IN_Device::TestRtc(void)
{
  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds
               );
}
uint16_t  EPD4IN_Device::SetRtcBuf(char *rtcbuf)
{

  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);

  // Print RTC
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds
               );

  sprintf(rtcbuf, "%04d/%02d/%02d",
          dateStruct.year,
          dateStruct.month,
          dateStruct.date);

  return timeStruct.seconds;

}
uint16_t  EPD4IN_Device::GetRtcSenconds(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.seconds;
}
uint16_t  EPD4IN_Device::GetRtcMinute(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.minutes;
}
uint16_t  EPD4IN_Device::GetRtcHour(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getTime(&timeStruct);
  return timeStruct.hours;
}
uint16_t  EPD4IN_Device::GetRtcMonth(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  return dateStruct.month;
}
uint16_t  EPD4IN_Device::GetRtcDay(void)
{
  Wire1.begin(21, 22);
  rtc.begin();
  // Get RTC
  rtc.getDate(&dateStruct);
  return dateStruct.date;
}
void  EPD4IN_Device::Rtc_Set(uint16_t rtc_year, uint8_t rtc_month, uint8_t rtc_date, uint8_t rtc_weekDay, uint8_t rtc_hours, uint8_t rtc_minutes, uint8_t rtc_seconds)
{

  Wire1.begin(21, 22);
  rtc.begin();
  // Set RTC Date
  I2C_BM8563_DateTypeDef dateStruct;
  dateStruct.weekDay = rtc_weekDay;
  dateStruct.month = rtc_month;
  dateStruct.date = rtc_date;
  dateStruct.year = rtc_year;
  rtc.setDate(&dateStruct);

  // Set RTC Time
  I2C_BM8563_TimeTypeDef timeStruct;
  timeStruct.hours   = rtc_hours;
  timeStruct.minutes = rtc_minutes;
  timeStruct.seconds = rtc_seconds;
  rtc.setTime(&timeStruct);
}
float  EPD4IN_Device::GetBatVol(void)
{
  float analog_value = 0;
  pinMode(23, OUTPUT); //BAT EN H
  digitalWrite(23, HIGH);
  analogReadResolution(12);
  pinMode(35, INPUT);
  delay(20);
  analog_value = analogRead(35) / 586.1;
  Serial.printf("BAT VOL IS%f:\n", analog_value);
  digitalWrite(23, LOW);
  pinMode(23, INPUT); //BAT EN H
  return analog_value;
}
int EPD4IN_Device::SearchWeatherIco(uint8_t *ico_buf, int weathertype)
{
  for (int si = 0; si < 100; si++)
  {

    if ( Weather_ico[si].index == weathertype)
    {
      //    ico_buf=Weather_ico[si];
      memcpy(ico_buf, Weather_ico[si].model, 512);
      return 1;
    }

  }

}
void EPD4IN_Device::SetWDT(void)
{

  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt

}
void  EPD4IN_Device::FeedWDT(void)
{
  timerWrite(timer, 0); //reset timer (feed watchdog)

}
