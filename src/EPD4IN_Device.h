#ifndef _EPD4IN_DEVICE_H_
#define _EPD4IN_DEVICE_H_
#include <I2C_BM8563.h>
#include "SHTSensor.h"
#include "SPIFFS.h"
#include <string.h>
#include <Wire.h>
#include <Arduino.h>
#include "EPD4IN_driver.h"
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "ico_6464.h"
#include "EPD_Http.h"
#include "qrcode.h"
#include "PIC1.h"

class EPD4IN_Device {


  public :
    uint8_t IIC_Device_Dected(uint8_t *device_id);      //读取设备的I2C总线设备
    uint8_t  SHT_Init(void);
    uint8_t  RTC_Init(void);
    void EPD_Device_Init(void);
    void Start_Init_EPD(void);
    void SEt_EPD_NeedConfig(void);
    void SEt_EPD_WIFICONFIG(void);
    void Start_EPD_Show(void);

    uint16_t DrawFullScreen(void);
    uint16_t Screen_PowerUp(void);
    uint16_t DrawFuntionArea(uint16_t showtag);


    uint16_t DrawTimeUpdata(void);
    void TestRtc(void);
    void SetShtBuf(char *shtbuf);
    void TestSht(void);
    uint16_t  SetRtcBuf(char *rtcbuf);
    uint16_t  GetRtcMonth(void);
    uint16_t  GetRtcDay(void);
    uint16_t  GetRtcSenconds(void);
    uint16_t  GetRtcMinute(void);
    uint16_t  GetRtcHour(void);
    uint16_t  DrawStartScreen(void);
    void Rtc_Set(uint16_t rtc_year, uint8_t rtc_month, uint8_t rtc_date, uint8_t rtc_weekDay, uint8_t rtc_hours, uint8_t rtc_minutes, uint8_t rtc_seconds);
    float  GetBatVol(void);
    int SearchWeatherIco(uint8_t *ico_buf, int weathertype);

    void SetWDT(void);
    void  FeedWDT(void);
};

#endif
