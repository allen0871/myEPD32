#include "EPD4IN_driver.h"
#include "PIC.h"
#include "SPIFFS.h"

#include <esp32-hal-dac.h>

String waveformbin = "/waveform.bin";
String ecodebin = "/ecode.bin";
String fontname;
SPIClass *hspi = NULL;
static const int spiClk = 15000000; // 15 MHz
unsigned char UNICODEbuffer[1024];
struct pl_area epd_area;
struct font_type Epd_Font;
//配置显示字体
//
File f ;
void EPD4INC::EPD_SetFount( FONT font_index)
{
 
 
  switch (font_index)
  {
    case 0: Epd_Font.Fontname_En = "/fonten12.bin";       Epd_Font.Fontname_Cn = "/fontcn12.bin";  Epd_Font.Epd_Fontfont_addr=0x590000;           Epd_Font.cn_px = 12; Epd_Font.cn_py = 12; Epd_Font.en_px = 6; Epd_Font.en_py = 12; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 0; break;
    case 1: Epd_Font.Fontname_En = "/fonten8.bin";        Epd_Font.Fontname_Cn = "/fontcn8.bin";   Epd_Font.Epd_Fontfont_addr=0x710000;           Epd_Font.cn_px = 8; Epd_Font.cn_py = 8; Epd_Font.en_px = 4; Epd_Font.en_py = 8; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 1; break;
//  case 2: Epd_Font.Fontname_En = "/fonten15.bin";       Epd_Font.Fontname_Cn = "\0";                                                            Epd_Font.cn_px = 15; Epd_Font.cn_py = 15; Epd_Font.en_px = 8; Epd_Font.en_py = 15; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 2; break;
    case 3: Epd_Font.Fontname_En = "/fonten16.bin";       Epd_Font.Fontname_Cn = "/fontcn16.bin";  Epd_Font.Epd_Fontfont_addr=0x390000;           Epd_Font.cn_px = 16; Epd_Font.cn_py = 16; Epd_Font.en_px = 8; Epd_Font.en_py = 16; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 3; break;
//  case 4: Epd_Font.Fontname_En = "/fonten24.bin";       Epd_Font.Fontname_Cn = "\0";              f = SPIFFS.open(Epd_Font.Fontname_En, "r");   Epd_Font.cn_px = 24; Epd_Font.cn_py = 24; Epd_Font.en_px = 24; Epd_Font.en_py = 24; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 4; break;
//  case 5: Epd_Font.Fontname_En = "/fonten32.bin";       Epd_Font.Fontname_Cn = "\0";              f = SPIFFS.open(Epd_Font.Fontname_En, "r");   Epd_Font.cn_px = 32; Epd_Font.cn_py = 32; Epd_Font.en_px = 32; Epd_Font.en_py = 32; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 5; break;
 // case 6: Epd_Font.Fontname_En = "/fontnum16.bin";      Epd_Font.Fontname_Cn = "\0";              f = SPIFFS.open(Epd_Font.Fontname_En, "r");   Epd_Font.cn_px = 8; Epd_Font.cn_py = 16;   Epd_Font.en_px = 8; Epd_Font.en_py = 16; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 6; break;
//  case 7: Epd_Font.Fontname_En = "/fontnum32.bin";      Epd_Font.Fontname_Cn = "\0";              f = SPIFFS.open(Epd_Font.Fontname_En, "r");   Epd_Font.cn_px = 16; Epd_Font.cn_py = 32;   Epd_Font.en_px = 16; Epd_Font.en_py = 32; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 7; break;
    case 8: Epd_Font.Fontname_En = "/fontnum48.bin";      Epd_Font.Fontname_Cn = "\0";              Epd_Font.Epd_Fontfont_addr=0x790000;           Epd_Font.cn_px = 24; Epd_Font.cn_py = 48;   Epd_Font.en_px = 24; Epd_Font.en_py = 48; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 8; break;
    case 9: Epd_Font.Fontname_En = "/fontnum64.bin";      Epd_Font.Fontname_Cn = "\0";              Epd_Font.Epd_Fontfont_addr=0x791000;           Epd_Font.cn_px = 32; Epd_Font.cn_py = 64;   Epd_Font.en_px = 32; Epd_Font.en_py = 64; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 9; break;
//    case 10: Epd_Font.Fontname_En = "\0";                 Epd_Font.Fontname_Cn = "\0";             Epd_Font.Epd_Fontfont_addr=0x7A0000;           Epd_Font.cn_px = 128; Epd_Font.cn_py = 128; Epd_Font.en_px = 64; Epd_Font.en_py = 128; Epd_Font.part_dx = 1; Epd_Font.Font_Index = 10; break;
 
  }

}
  int SPIstatus = 0;      //如果休眠 ，变量会复位位0.表示需要重新初始化
//初始化端口 
void  EPD4INC::EPD4INC_Port_init(void)
{
  SPIstatus=1;;
  hspi = new SPIClass(HSPI);
  //initialise hspi with default pins
  //SCLK = 14, MISO = 12, MOSI = 13, SS = 15
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  hspi->begin();
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  Serial.println("HSPI Init OK");
  //alternatively route through GPIO pins
  //hspi->begin(25, 26, 27, 32); //SCLK, MISO, MOSI, SS
  pinMode(SPICS_Pin, OUTPUT); //HSPI SS
  pinMode(EN3P3_Pin, OUTPUT); //HSPI SS
  pinMode(NRST_Pin, OUTPUT); //HSPI SS
 
  pinMode(SHDN_Pin, OUTPUT); //HSPI SS
  digitalWrite(EN3P3_Pin, HIGH);
  digitalWrite(NRST_Pin, HIGH);
  digitalWrite(SHDN_Pin, LOW);

  digitalWrite(SPICS_Pin, 1);

}


void  EPD4INC::EPD4INC_Port_Reinit(void)
{
  
  if(SPIstatus==0){ 
  hspi = new SPIClass(HSPI);
  //initialise hspi with default pins
  //SCLK = 14, MISO = 12, MOSI = 13, SS = 15
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  hspi->begin();
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  
  Serial.println("HSPI ReInit OK");

  
 
  pinMode(SPICS_Pin, OUTPUT); //HSPI SS
  pinMode(EN3P3_Pin, OUTPUT); //HSPI SS
 
  pinMode(SHDN_Pin, OUTPUT); //HSPI SS

  pinMode(HDC_Pin, OUTPUT); //HSPI SS


  digitalWrite(EN3P3_Pin, HIGH);
  pinMode(NRST_Pin, OUTPUT); //HSPI SS
  digitalWrite(NRST_Pin, HIGH);
    
  digitalWrite(SHDN_Pin, LOW);

  digitalWrite(SPICS_Pin, 1);
  }

  
}

//启动3V3供电 长供电，无需使用本函数
int EPD4INC::EPD4INC_Power_up( void)
{
  dacWrite(25,145);
 
  digitalWrite(EN3P3_Pin, LOW);
  digitalWrite(NRST_Pin, LOW);
  //  digitalWrite(WKUP_Pin, HIGH);   //屏蔽即表示 停止时钟
  digitalWrite(SHDN_Pin, HIGH);
 }
//恢复3V3供电，长供电无需使用本函数
 int EPD4INC::EPD4INC_Power_ReUp( void)
{
  dacWrite(25,150);
  digitalWrite(EN3P3_Pin, LOW);
  digitalWrite(SHDN_Pin, HIGH);
}
//3V3掉电
int EPD4INC::EPD4INC_Power_down( void)
{
  digitalWrite(EN3P3_Pin, HIGH);
  digitalWrite(NRST_Pin, LOW);
  //  digitalWrite(WKUP_Pin, LOW);
  digitalWrite(SHDN_Pin, LOW);
 
}
//sleep 高压断电
int EPD4INC::EPD4INC_Power_Sleep( void)
{
//  digitalWrite(EN3P3_Pin, LOW);
//  digitalWrite(NRST_Pin, LOW);
//  digitalWrite(WKUP_Pin, LOW);
//  digitalWrite(SHDN_Pin, LOW);
  

}
//
int EPD4INC::EPD4INC_Power_wakeup( void)
{
// digitalWrite(SHDN_Pin, HIGH);
  digitalWrite(EN3P3_Pin, LOW);
 
}
int EPD4INC::EPD4INC_HVEN( uint16_t vcom_ratio)
{
  
  if(vcom_ratio<2)
      vcom_ratio=12;    //如果小于0 则为15 适配大部分屏幕
  vcom_ratio=vcom_ratio*10;
   
  pinMode(SHDN_Pin, OUTPUT); 
  dacWrite(25,vcom_ratio);      //
  digitalWrite(SHDN_Pin, HIGH);
}
int EPD4INC::EPD4INC_HVDISEN( void)
{
 digitalWrite(SHDN_Pin, LOW);
}

int EPD4INC::UTFtoUNICODE(unsigned char *code)
{
  int i = 0; int charcount = 0;
  while (code[i] != '\0')
  {
    //Serial.println("current codei");
    //Serial.println(code[i],HEX);
    //Serial.println(code[i]&0xf0,HEX);
    if (code[i] <= 0x7f) //ascii
    {

      UNICODEbuffer[charcount * 2] = 0x00;
      UNICODEbuffer[charcount * 2 + 1] = code[i];
      //Serial.println("english or number");
      //Serial.println(UNICODEbuffer[charcount*2],HEX);
      //Serial.println(UNICODEbuffer[charcount*2+1],HEX);
      i++; charcount++;

    }
    else if ((code[i] & 0xe0) == 0xc0)
    {

      UNICODEbuffer[charcount * 2 + 1] = (code[i] << 6) + (code[i + 1] & 0x3f);
      UNICODEbuffer[charcount * 2] = (code[i] >> 2) & 0x07;
      i += 2; charcount++;
      //Serial.println("two bits utf-8");
    }
    else if ((code[i] & 0xf0) == 0xe0)
    {

      UNICODEbuffer[charcount * 2 + 1] = (code[i + 1] << 6) + (code[i + 2] & 0x7f);
      UNICODEbuffer[charcount * 2] = (code[i] << 4) + ((code[i + 1] >> 2) & 0x0f);

      //Serial.println("three bits utf-8");
      //Serial.println(UNICODEbuffer[charcount*2],HEX);
      //Serial.println(UNICODEbuffer[charcount*2+1],HEX);
      i += 3; charcount++;
    }
    else
    {
      //Serial.println("return unexpect\n");
      //return 0;
      i++;
    }
  }
  UNICODEbuffer[charcount * 2] = '\0';
  return charcount * 2;
}

#include "ESP.h"
//在指定位置显示unicode
//参数 X Y 坐标   字符数据（2字节） 是否反相
uint16_t EPD4INC::DrawUnicodeChar(int16_t x, int16_t y,  unsigned char *code, unsigned char isinv)
{
  int charcode, offset;
  int sizeofsinglechar;
  String fontname;
  epd_area.top = y;
  epd_area.left = x;
  charcode = (code[0] * 0x100 + code[1]);

  epd_area.height = Epd_Font.cn_py;
  epd_area.width =  Epd_Font.cn_px;   //宽度 px

  if ( epd_area.height % 8 == 0)              
    sizeofsinglechar = ( epd_area.height / 8) * epd_area.width;
  else
    sizeofsinglechar = ( epd_area.height / 8 + 1) * epd_area.width ;
    
  offset = charcode * sizeofsinglechar;
  fontname = Epd_Font.Fontname_Cn;
//注意是否需要4字节对齐

  uint32_t  flashreadbuf[sizeofsinglechar/4];    //直接定义成int 4字节对齐，避免出错
  char *fontbuf=(char*)flashreadbuf;
  ESP.flashRead(offset+Epd_Font.Epd_Fontfont_addr,flashreadbuf,(size_t )sizeofsinglechar);
 
 
  User_Font_Tran((uint16_t)sizeofsinglechar, (uint8_t*)fontbuf, S1D13541_LD_IMG_1BPP, &epd_area, isinv);
 
  if ((charcode < 0x80) && (Epd_Font.Font_Index < 6)) //字库种英文字符为方块，此处更新  汉字字库种 ，英文字符也是方块，但是只占用前半部分，故每次直接覆盖后半部分，实现英文字符变为汉字的一半
  {
    epd_area.height = Epd_Font.en_py;
    epd_area.width =  Epd_Font.en_px;
  }
  return epd_area.width;  //返回操作数据宽度，便于下一个字符处理
}

int EPD4INC::UnicodeCharWidth(unsigned char *code)
{
  int charwidth = 0;
  int charcode = (code[0] * 0x100 + code[1]);
  charwidth =  Epd_Font.cn_px;   //宽度 px
 
  if ((charcode < 0x80) && (Epd_Font.Font_Index < 6)) //字库种英文字符为方块，此处更新  汉字字库种 ，英文字符也是方块，但是只占用前半部分，故每次直接覆盖后半部分，实现英文字符变为汉字的一半
  {
      charwidth =  Epd_Font.en_px;
  }
  return charwidth;  //返回操作数据宽度，便于下一个字符处理
}

//在指定位置显示unicode
//参数 X Y 坐标   字符数据（2字节） 是否反相
int EPD4INC::DrawUnicodeStr(int16_t x, int16_t y, uint16_t strle,  unsigned char *code, unsigned char isinv)
{
  int i = 0;
  int locat = 0, locatreturn;
  for (i = 0; i < strle / 2; i++)
  {
    locatreturn = DrawUnicodeChar(x + locat, y, code, isinv);
    locat += locatreturn;
    code += 2;
  }
  return locat;
}

int EPD4INC::DrawUnicodeStr(int16_t x, int16_t y, uint16_t strle,  unsigned char *code, unsigned char isinv, int *drawLength)
{
  const int maxlengh = 240;
  int i = 0;
  int locat = x, locatreturn;
  if(!drawLength) {
    return 0;
  }
  *drawLength = 0;
  for (i = 0; i < strle / 2; i++)
  {
    int width = UnicodeCharWidth(code);
    if((locat + width)<= maxlengh) {
      locatreturn = DrawUnicodeChar(x + locat, y, code, isinv);
      locat += locatreturn;
      code += 2;
      *drawLength += 2;
    }
  }
  return locat;
}

void EPD4INC::DrawUTF(int16_t x, int16_t y, String code, unsigned char isinv)
{
  char buffer[512];
  code.toCharArray(buffer, 512);
  DrawUTF(x, y, (unsigned char *)buffer, isinv);
}
void EPD4INC::DrawUTF(int16_t x, int16_t y, unsigned char *code, unsigned char isinv)
{
  int charcount;
  charcount = UTFtoUNICODE((unsigned char*)code);
  //Serial.printf("\n DrawUTF charcount is%d\n",charcount);   //字符长度
  DrawUnicodeStr(x, y, charcount, (unsigned char *)UNICODEbuffer, isinv);

}

void EPD4INC::DrawUTF_lhb(int16_t x, int16_t y, String code, unsigned char isinv)
{
  char buffer[512]; 
  int charcount;
  int i;
  int line;
  code.toCharArray(buffer, 512); 
  charcount = UTFtoUNICODE((unsigned char*)buffer);
  line = (charcount/60+1);
  Serial.printf("\n DrawUTF charcount is%d\n",charcount);   //字符长度
  for(i=0;i<line;i++)
  {
    DrawUTF(x, y+(18*i), (unsigned char *)(&buffer[i*30]), isinv);
  }
 
}

int EPD4INC::DrawUTF_hz(int16_t x, int16_t y, String code, unsigned char isinv, int offset) {
  char buffer[512]; 
  int charcount;
  int line = 0;
  int index = 0;
  code.toCharArray(buffer, 512); 
  charcount = UTFtoUNICODE((unsigned char*)buffer);

  Serial.printf("\n DrawUTF hz charcount is%d\n",charcount);   //字符长度
  
  while(charcount>0)
  {
    int drawLength;
    int xoffset = x;
    if(line>0) {
      xoffset = x + offset; 
    }
    int locat = DrawUnicodeStr(xoffset, y+(18*line), charcount, (unsigned char *)(UNICODEbuffer+index), isinv, &drawLength);
    Serial.printf("x=%d loc=%d drawLength=%d\n", x, locat, drawLength);
    charcount -= drawLength;
    index += drawLength;
    line++;
    if(line>7) {
      break;
    }
  }
  Serial.println(buffer);
  return line;
}

int EPD4INC::DrawUTF_hz(int16_t x, int16_t y, String code, unsigned char isinv)
{
  DrawUTF_hz(x,y,code,isinv, 0);
}

void EPD4INC::DrawTime(int16_t x, int16_t y, uint16_t hour, uint16_t minute, FONT font_index, unsigned char isinv)
{ //12:12
  FONT font_back = font_index;
  uint16_t widthback;
  unsigned char timebuf[10];  //

  timebuf[0] = 0;
  timebuf[1] = hour / 10;
  timebuf[2] = 0;
  timebuf[3] = hour % 10;
  timebuf[4] = 0;
  timebuf[5] = 10;
  timebuf[6] = 0;
  timebuf[7] = minute / 10;
  timebuf[8] = 0;
  timebuf[9] = minute % 10;
  EPD_SetFount( font_index);    //设置字体 设置数码管字体
  DrawUnicodeStr( x, y, 10, timebuf, isinv);
  EPD_SetFount( font_back);     //恢复字体
}
void EPD4INC::DrawCalendarDay(int16_t x, int16_t y, uint16_t day, FONT font_index, unsigned char isinv)
{ //12:12
  FONT font_back = font_index;
  uint16_t widthback;
  unsigned char timebuf[4];  //

  timebuf[0] = 0;
  timebuf[1] = day / 10;
  timebuf[2] = 0;
  timebuf[3] = day % 10;
  timebuf[4] = 0;
 
  EPD_SetFount( font_index);    //设置字体 设置数码管字体
  DrawUnicodeStr( x, y, 5, timebuf, isinv);
  EPD_SetFount( font_back);     //恢复字体
}


//private
int EPD4INC:: epson_epdc_clear_init()
{
  return s1d135xx_clear_init();
}
int EPD4INC::epson_epdc_wait_update_end()
{
  return s1d135xx_wait_update_end();
}
int EPD4INC::epson_epdc_set_power(enum pl_epdc_power_state state)
{
  if (s1d135xx_set_power_state(state))
    return -1;
  return 0;
}
int EPD4INC::epson_epdc_set_epd_power(int on)
{
  return s1d135xx_set_epd_power(on);
}
int EPD4INC::epson_epdc_init()
{

  if (epson_epdc_init_s1d13541())
    return -1;

  if (s1d13541_load_wflib())
    return -1;
  return 0;
}
int EPD4INC::epson_epdc_early_init()
{
  return epson_epdc_early_init_s1d13541();
}



void EPD4INC::s1d135xx_hard_reset()
{
  digitalWrite(NRST_Pin, LOW);
  delay(4);
  digitalWrite(NRST_Pin, HIGH);
  delay(10);
}

int EPD4INC::s1d135xx_soft_reset()
{
  s1d135xx_write_reg(S1D135XX_REG_SOFTWARE_RESET, 0xFF);
  return 0;
}

int EPD4INC::s1d135xx_check_prod_code(uint16_t ref_code)
{
  uint16_t prod_code;

  prod_code = s1d135xx_read_reg(S1D135XX_REG_REV_CODE);

  do {
    prod_code = s1d135xx_read_reg(S1D135XX_REG_REV_CODE);
    Serial.println("prod_code:0x");
    Serial.print(prod_code, HEX);
  } while (prod_code != ref_code);


  if (prod_code != ref_code) {
    return -1;
  }

  return 0;
}

int EPD4INC:: transfer_file(File *file)
{
  char databuf[DATA_BUFFER_LENGTH];
  int filesize = file->size();
  if (filesize < 0)
    return -1;
  for (int i = 0; i < (filesize / DATA_BUFFER_LENGTH); i++)
  {
    file->readBytes(databuf, DATA_BUFFER_LENGTH);
    transfer_data((uint8_t*)databuf, DATA_BUFFER_LENGTH);
  }
  file->readBytes(databuf, filesize % DATA_BUFFER_LENGTH);
  transfer_data((uint8_t*)databuf, filesize % DATA_BUFFER_LENGTH);

  return 0;
}

int EPD4INC::s1d135xx_load_init_code()
{
  uint16_t checksum;
  int stat;
  SPIFFS.begin();
  Serial.print("\nload ecobin data star \n");
  File f = SPIFFS.open(ecodebin, "r");
  if (s1d135xx_wait_idle())
    return -1;
  set_cs(0);
  send_cmd(S1D135XX_CMD_INIT_SET);
  stat = transfer_file(&f);
  //  transfer_data(initdata1, sizeof(initdata1));
  set_cs(1);
  f.close();
  Serial.print("\nload ecobin data over \n");
  if (s1d135xx_wait_idle())
    return -1;
  set_cs(0);
  send_cmd(S1D135XX_CMD_INIT_STBY);
  send_param(0x0500);
  set_cs(1);
  delay(10);
  if (s1d135xx_wait_idle())
    return -1;
  checksum = s1d135xx_read_reg(S1D135XX_REG_SEQ_AUTOBOOT_CMD);

  if (!(checksum & (uint16_t)S1D135XX_INIT_CODE_CHECKSUM_OK)) {
    return -1;
  }

  return 0;
}

int EPD4INC::s1d135xx_load_wflib(uint32_t size, uint32_t addr)
{
  uint16_t params[4];
  SPIFFS.begin();
  Serial.print("\nload waveformbin data star \n");
  File f = SPIFFS.open(waveformbin, "r");

  uint32_t size2 = f.size() / 2;
  if (s1d135xx_wait_idle())
    return -1;
  params[0] = addr & 0xFFFF;
  params[1] = (addr >> 16) & 0xFFFF;
  params[2] = size2 & 0xFFFF;
  params[3] = (size2 >> 16) & 0xFFFF;
  set_cs(0);
  send_cmd(S1D135XX_CMD_BST_WR_SDR);
  send_params(params, 4);
  set_cs(1);

  // wflib_wr(waveform, sizeof(waveform));
  wflib_wr_file(&f);

  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_BST_END_SDR);
  f.close();

  Serial.print("\nload waveformbin data over \n");
  return s1d135xx_wait_idle();
}

int EPD4INC::s1d135xx_init_gate_drv()
{
  send_cmd_cs(S1D135XX_CMD_EPD_GDRV_CLR);

  return s1d135xx_wait_idle();
}

int EPD4INC::s1d135xx_wait_dspe_trig()
{
  send_cmd_cs(S1D135XX_CMD_WAIT_DSPE_TRG);

  return 0;
}

int  EPD4INC::s1d135xx_clear_init()
{
  send_cmd_cs(S1D135XX_CMD_UPD_INIT);

  return s1d135xx_wait_dspe_trig();
}

int EPD4INC::s1d135xx_fill(uint16_t mode, unsigned bpp, const struct pl_area *a, uint8_t grey)
{
  struct pl_area full_area;
  const struct pl_area *fill_area;

  set_cs(0);

  if (a != NULL) {
    send_cmd_area(S1D135XX_CMD_LD_IMG_AREA, mode, a);
    fill_area = a;
  } else {
    send_cmd(S1D135XX_CMD_LD_IMG);
    send_param(mode);
    full_area.top = 0;
    full_area.left = 0;
    full_area.width = 399;
    full_area.height = 239;
    fill_area = &full_area;
  }

  set_cs(1);

  return do_fill(fill_area, bpp, grey);
}

int EPD4INC::s1d135xx_pattern_check(uint16_t height, uint16_t width, uint16_t checker_size, uint16_t mode)
{
  uint16_t i = 0, j = 0, k = 0;
  uint16_t val = 0;

  set_cs(0);
  send_cmd(S1D135XX_CMD_LD_IMG);
  send_param(mode);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);

 // transfer_data(gImage_PIC, 48000);

  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);

  return 0;
}


int EPD4INC::s1d135xx_update(int wfid, enum pl_update_mode mode,  const struct pl_area *area)
{
  //  struct pl_area area_scrambled;
  uint8_t command = S1D135XX_CMD_UPDATE_FULL + mode;
  set_cs(0);

  /* wfid = S1D135XX_WF_MODE(wfid); */

  if (area != NULL) {
    if (!(command % 2 == 0))
      command++;

    send_cmd_area(command, S1D135XX_WF_MODE(wfid), area);

  } else {
    send_cmd(command);
    send_param(S1D135XX_WF_MODE(wfid));
  }

  set_cs(1);

  if (s1d135xx_wait_idle())
    return -1;

  return s1d135xx_wait_dspe_trig();
}

int EPD4INC::EPD4INC::s1d135xx_wait_update_end()
{
  send_cmd_cs(S1D135XX_CMD_WAIT_DSPE_FREND);

  return s1d135xx_wait_idle();
}

int EPD4INC::s1d135xx_wait_idle()
{
  unsigned long timeout = 100000;

  while (!get_hrdy() && --timeout);

  if (!timeout) {

    return -1;
  }

  return 0;
}


int EPD4INC::s1d135xx_set_power_state(enum pl_epdc_power_state state)
{
  int stat;

  set_cs(1);
  set_hdc(1);
  //  HAL_GPIO_WritePin(GPIOB, EN3P3_Pin, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(GPIOB, WKUP_Pin, GPIO_PIN_SET);

  switch (state) {
    case PL_EPDC_RUN:
      send_cmd_cs(S1D135XX_CMD_RUN);
      stat = s1d135xx_wait_idle();
      break;

    case PL_EPDC_STANDBY:
      send_cmd_cs(S1D135XX_CMD_STBY);
      stat = s1d135xx_wait_idle();
      break;

    case PL_EPDC_SLEEP:
      send_cmd_cs(S1D135XX_CMD_STBY);
      stat = s1d135xx_wait_idle();

      break;

    case PL_EPDC_OFF:
      send_cmd_cs(S1D135XX_CMD_SLEEP);
      stat = s1d135xx_wait_idle();

      set_hdc(0);
      set_cs(0);
      break;
  }

  return stat;
}

int EPD4INC::s1d135xx_set_epd_power(int on)
{
  uint16_t arg = on ? S1D135XX_PWR_CTRL_UP : S1D135XX_PWR_CTRL_DOWN;
  uint16_t tmp;

  s1d135xx_write_reg(S1D135XX_REG_PWR_CTRL, arg);

  do {
    tmp = s1d135xx_read_reg(S1D135XX_REG_PWR_CTRL);
  } while (tmp & S1D135XX_PWR_CTRL_BUSY);

  if (on && ((tmp & S1D135XX_PWR_CTRL_CHECK_ON) !=
             S1D135XX_PWR_CTRL_CHECK_ON)) {
    return -1;
  }
  return 0;
}

void EPD4INC::s1d135xx_cmd(uint16_t cmd, uint16_t *params, size_t n)
{
  set_cs(0);
  send_cmd(cmd);
  send_params(params, n);
  set_cs(1);
}

uint16_t EPD4INC::s1d135xx_read_reg(uint16_t reg)
{
  uint16_t val;

  set_cs(0);
  send_cmd(S1D135XX_CMD_READ_REG);
  send_param(reg);


  val = hspi->transfer16(0xffff);
  val = hspi->transfer16(0xffff);
  set_cs(1);

  return val;  // swap bytes after read
  //  return ((val<<8)&0xFF00) | ((val>>8)&0xFF);
}

void EPD4INC::s1d135xx_write_reg(uint16_t reg, uint16_t val)
{
  uint16_t params[] = { reg, val };

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_params(params, 2);
  set_cs(1);
}


/* ----------------------------------------------------------------------------
   private functions
*/
#define S1D13541_STATUS_HRDY            (1 << 13)

int EPD4INC::get_hrdy()
{
  uint16_t status;
  status = s1d135xx_read_reg(S1D135XX_REG_SYSTEM_STATUS);

  return ((status & S1D13541_STATUS_HRDY) == S1D13541_STATUS_HRDY);
}

int EPD4INC::do_fill(const struct pl_area *area, unsigned bpp, uint8_t g)
{
  uint16_t val16;
  uint16_t lines;
  uint16_t pixels;

  /* Only 16-bit transfers for now... */
  //  assert(!(area->width % 2));

  switch (bpp) {
    case 1:
    case 2:
      //    LOG("Unsupported bpp");
      return -1;
    case 4:
      val16 = g & 0xF0;
      val16 |= val16 >> 4;
      val16 |= val16 << 8;
      pixels = area->width / 4;
      break;
    case 8:
      val16 = g | (g << 8);
      pixels = area->width / 2;
      break;
    default:;
      //    assert_fail("Invalid bpp");
  }

  lines = area->height;

  if (s1d135xx_wait_idle())
    return -1;

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);

  while (lines--) {
    uint16_t x = pixels;

    while (x--)
      send_param(val16);
  }

  set_cs(1);

  if (s1d135xx_wait_idle())
    return -1;

  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);

  return s1d135xx_wait_idle();
}

int EPD4INC::wflib_wr(const uint8_t *data, size_t n)
{
  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  transfer_data(data, n);
  set_cs(1);

  return 0;
}
//增加文件读取  写入SPI
int EPD4INC::wflib_wr_file(File *file)
{
  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  transfer_file(file);

  set_cs(1);
  return 0;
}

void EPD4INC:: transfer_data(const uint8_t *data, size_t n)
{
  uint16_t *data16 = (uint16_t *)data;

  n /= 2;

  while (n--)
    send_param(*data16++);
}

void EPD4INC::send_cmd_area(uint16_t cmd, uint16_t mode,
                            const struct pl_area *area)
{
  uint16_t args[] = {
    mode,
    (area->left & S1D135XX_XMASK),
    (area->top & S1D135XX_YMASK),
    (area->width & S1D135XX_XMASK),
    (area->height & S1D135XX_YMASK),
  };
#if VERBOSE
  LOG("%s: Command: 0x%04X", __func__, cmd);
#endif
  send_cmd(cmd);
  send_params(args, 5);
}

void  EPD4INC::send_cmd_cs(uint16_t cmd)
{
  set_cs(0);
  send_cmd(cmd);
  set_cs(1);
}

void EPD4INC::send_cmd(uint16_t cmd)
{
  //  uint16_t cmd2 = ((cmd<<8)&0xFF00) | ((cmd>>8)&0xFF);
  //  cmd = htobe16(cmd); // swap bytes before writing
  set_hdc(0);
  //  p->interface->write((uint8_t *)&cmd, sizeof(uint16_t));
  //  HAL_SPI_Transmit(&hspi1, (uint8_t*)&cmd, 1, 0xff);
  //  hspi->transfer((char)(cmd>>8));
  //  hspi->transfer((char)(cmd&0xff00));

  hspi->transfer16(cmd);
  set_hdc(1);
}

void EPD4INC::send_params(uint16_t *params, size_t n)
{
  size_t i;

  for (i = 0; i < n; ++i)
    send_param(params[i]);
}

void EPD4INC::send_param(uint16_t param)
{


  hspi->transfer16(param);
  //  HAL_SPI_Transmit(&hspi1, (uint8_t*)&param, 1, 0xff);
}
void EPD4INC::set_cs(int state)
{
  if (state)  digitalWrite(SPICS_Pin, HIGH);
  if (!state) digitalWrite(SPICS_Pin, LOW);

}

void EPD4INC::set_hdc(int state)
{
  if (state) digitalWrite(HDC_Pin, HIGH);
  if (!state)digitalWrite(HDC_Pin, LOW);

}

int EPD4INC::set_init_rot_mode()
{
  set_cs(0);
  send_cmd(S1D135XX_CMD_INIT_ROT_MODE);
  send_param(0x0400);
  set_cs(1);
  delay(10);
  return s1d135xx_wait_idle();
}


int EPD4INC::s1d13541_load_wflib()
{
  //  struct s1d135xx *p = epdc->data;

  return s1d135xx_load_wflib(2, S1D13541_WF_ADDR);
}

int EPD4INC::s1d13541_set_temp_mode(enum pl_epdc_temp_mode mode)
{
  //  struct s1d135xx *p = epdc->data;
  uint16_t reg;

  //  if (mode == epdc->temp_mode)
  //    return 0;

  reg = s1d135xx_read_reg(S1D135XX_REG_PERIPH_CONFIG);
  /* ToDo: when do we set this bit back? */
  reg &= S1D13541_TEMP_SENSOR_CONTROL;

  switch (mode) {
    case PL_EPDC_TEMP_MANUAL:
      break;
    case PL_EPDC_TEMP_EXTERNAL:
      reg &= ~S1D13541_TEMP_SENSOR_EXTERNAL;
      break;
    case PL_EPDC_TEMP_INTERNAL:
      reg |= S1D13541_TEMP_SENSOR_EXTERNAL;
      break;
    default:;
      //    assert_fail("Invalid temperature mode");
  }

  s1d135xx_write_reg(S1D135XX_REG_PERIPH_CONFIG, reg);

  /* Configure the controller to automatically update the waveform table
     after each temperature measurement.  */
  reg = s1d135xx_read_reg(S1D13541_REG_WF_DECODER_BYPASS);
  reg |= S1D13541_AUTO_TEMP_JUDGE_EN;
  s1d135xx_write_reg(reg, S1D13541_REG_WF_DECODER_BYPASS);

  //  epdc->temp_mode = mode;

  return 0;
}

int EPD4INC::s1d13541_update_temp(enum pl_epdc_temp_mode temp_mode)
{
  //  struct s1d135xx *p = epdc->data;
  int stat;

  switch (temp_mode) {
    case PL_EPDC_TEMP_MANUAL:
      //    stat = update_temp_manual(epdc->manual_temp);
      break;
    case PL_EPDC_TEMP_EXTERNAL:
      stat = update_temp_auto(S1D135XX_REG_I2C_TEMP_SENSOR_VALUE);
      break;
    case PL_EPDC_TEMP_INTERNAL:
      stat = update_temp_auto(S1D13541_REG_TEMP_SENSOR_VALUE);
      break;
  }

  if (stat)
    return -1;


  return 0;
}

int EPD4INC::s1d13541_fill(const struct pl_area *area, uint8_t grey)
{
  return s1d135xx_fill(S1D13541_LD_IMG_8BPP, 8, area, grey);
}

int EPD4INC::s1d13541_pattern_check(uint16_t size)
{
  //return s1d135xx_pattern_check(240, 400, size, S1D13541_LD_IMG_8BPP);
  return s1d135xx_pattern_check(240, 400, size, S1D13541_LD_IMG_4BPP);
}
int EPD4INC::s1d13541_pattern_check_test_part(uint16_t size, const struct pl_area *area)
{

}
/* -- initialisation -- */
int EPD4INC::epson_epdc_early_init_s1d13541()
{
  int16_t measured_temp = -127;
  s1d135xx_hard_reset();

  if (s1d135xx_soft_reset())
    return -1;
  Serial.println("Check EPD ... ");
  if (s1d135xx_check_prod_code(S1D13541_PROD_CODE))
    return -1;

  return s1d13541_init_clocks();
}

int EPD4INC::epson_epdc_init_s1d13541()
{
  //  struct s1d135xx *p = epdc->data;

  if (epson_epdc_early_init_s1d13541())
    return -1;

  if (s1d135xx_load_init_code()) {
    return -1;
  }
  
  EPD_Set_Rota(3);      //增加旋转设置

  // mg033 & mg034
  //s1d135xx_write_reg(p, 0x0140, 0);

  s1d135xx_write_reg(S1D13541_REG_PROT_KEY_1, S1D13541_PROT_KEY_1);
  s1d135xx_write_reg(S1D13541_REG_PROT_KEY_2, S1D13541_PROT_KEY_2);

  if (s1d135xx_wait_idle())
    return -1;

  if (s1d135xx_set_power_state(PL_EPDC_RUN))
    return -1;

  if (s1d135xx_init_gate_drv())
    return -1;

  if (s1d135xx_wait_dspe_trig())
    return -1;


  return s1d13541_set_temp_mode(PL_EPDC_TEMP_INTERNAL);
}

/* ----------------------------------------------------------------------------
   private functions
*/

int EPD4INC::s1d13541_init_clocks()
{
  s1d135xx_write_reg(S1D135XX_REG_I2C_CLOCK, S1D13541_I2C_CLOCK_DIV);

  //此处配置晶振 如果为20Mhz时钟输入

  s1d135xx_write_reg(0x0014, 0x0080);
  s1d135xx_write_reg(0x0012, 0x9000);



  if (s1d135xx_wait_idle())
    return -1;



  s1d135xx_write_reg(S1D13541_REG_CLOCK_CONFIG, S1D13541_INTERNAL_CLOCK_ENABLE);
  return s1d135xx_wait_idle();
}
void EPD4INC::update_temp(uint16_t reg)
{
  uint16_t regval;

  regval = s1d135xx_read_reg(S1D135XX_REG_INT_RAW_STAT);
  //  p->flags.needs_update = (regval & S1D13541_INT_RAW_WF_UPDATE) ? 1 : 0;
  s1d135xx_write_reg(S1D135XX_REG_INT_RAW_STAT,
                     (S1D13541_INT_RAW_WF_UPDATE |
                      S1D13541_INT_RAW_OUT_OF_RANGE));
  regval = s1d135xx_read_reg(reg) & S1D135XX_TEMP_MASK;

}


int EPD4INC::update_temp_auto(uint16_t temp_reg)
{
  if (s1d135xx_set_power_state(PL_EPDC_STANDBY))
    return -1;

  s1d135xx_cmd(S1D13541_CMD_RD_TEMP, NULL, 0);

  if (s1d135xx_wait_idle())
    return -1;

  if (s1d135xx_set_power_state( PL_EPDC_RUN))
    return -1;

  update_temp(temp_reg);

  return 0;
}

int EPD4INC::s1d13541_read_prom(uint8_t * blob)
{
  int i = 0, j = 0;
  uint16_t data = 0;
  uint16_t addr_ = 0;

  // wait for status: idle
  if (wait_for_ack(S1D13541_PROM_STATUS_IDLE, 0xffff))
    return -1;

  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 2; j++)
    {
      // set read address
      addr_ = ((i * 2 + j) << 8) & 0x0f00;
      s1d135xx_write_reg(S1D13541_PROM_ADR_PGR_DATA, addr_);

      // set read operation start trigger
      s1d135xx_write_reg(S1D13541_PROM_CTRL, S1D13541_PROM_READ_START);

      //wait for status: read mode start
      if (wait_for_ack(S1D13541_PROM_STATUS_READ_MODE, S1D13541_PROM_STATUS_READ_MODE))
        return -1;

      //wait for status: read operation finished
      if (wait_for_ack(0x0000, S1D13541_PROM_STATUS_READ_BUSY))
        return -1;

      // set read operation start trigger
      data = s1d135xx_read_reg(S1D13541_PROM_READ_DATA);
      if (j)
        blob[i] |= data & 0x0f;
      else
        blob[i] = data << 4 & 0xf0;

    }
  }

  // set read mode stop trigger
  s1d135xx_write_reg(S1D13541_PROM_CTRL, S1D13541_PROM_READ_STOP);

  //wait for status: read mode stop
  if (wait_for_ack(0x0000, S1D13541_PROM_STATUS_READ_MODE))
    return -1;

  return 0;
}


int EPD4INC::wait_for_ack (uint16_t status, uint16_t mask)
{
  unsigned long timeout = 50000;
  uint16_t v;
  while ((v = s1d135xx_read_reg(S1D13541_PROM_STATUS) & mask) != status) {
    --timeout;
    v = v;
    if (timeout == 0) {
      //                     LOG("PROM acknowledge timeout");
      return -1;
    }
  }

  return 0;
}


int EPD4INC:: pl_epdc_single_update(int wfid, enum pl_update_mode mode, const struct pl_area *area)
{
  if (s1d13541_update_temp(PL_EPDC_TEMP_INTERNAL))
    return -1;

  s1d135xx_set_epd_power(1);
  //  if (psu->on(psu))
  //    return -1;

  if (s1d135xx_update(wfid, mode, area))
    return -1;

  if (s1d135xx_wait_update_end())
    return -1;

  s1d135xx_set_epd_power(0);
  //  return psu->off(psu);
  return 0;
}
//////////////////////////////////////////////////////////////
//
void  EPD4INC::transfer_data_inv(const uint8_t *data, size_t n)
{
  uint16_t *data16 = (uint16_t *)data;
  n /= 2;
  while (n--)
    send_param(~(*data16++));
}
//高 宽  图片地址  模式 S1D13541_LD_IMG_4BPP 等
//区域结构体
//最后一个参数，是否反色 ，部分显示需要反色的。
//用户图形发送，  高度 宽度  指针  模式  区域 是否反相显示
int EPD4INC::User_Img_Tran(uint16_t height, uint16_t width, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv)
{
  uint16_t i = 0, j = 0, k = 0;
  uint16_t val = 0;

  set_cs(0);
  send_cmd_area( S1D135XX_CMD_LD_IMG_AREA, mode, area);
  send_cmd(S1D135XX_CMD_LD_IMG);
  send_param(mode);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  if (isinv == 1)
    transfer_data_inv(p_img, height * width);
  else
    transfer_data(p_img, height * width);

  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);

  return 0;
}
//高 宽  图片地址  模式 S1D13541_LD_IMG_4BPP 等
//区域结构体
//最后一个参数，是否反色 ，部分显示需要反色的。
//用户图形发送，  高度 宽度  指针  模式  区域 是否反相显示
int EPD4INC::User_Font_Tran(uint16_t fontsize, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv)
{
  uint16_t i = 0, j = 0, k = 0;
  uint16_t val = 0;
  uint8_t charbuf[fontsize];
  if (Epd_Font.Font_Index == 0) //12#
  {
    for (i = 0; i < 6; i++)
    {
      charbuf[0 + i * 3] = p_img[0 + i * 4];
      charbuf[1 + i * 3] = (p_img[2 + i * 4] << 4) + (p_img[1 + i * 4] & 0x0F);
      charbuf[2 + i * 3] = (p_img[3 + i * 4] << 4) + (p_img[2 + i * 4] >> 4);
    }
    p_img = charbuf;
    fontsize = 18;
  }
  set_cs(0);
  send_cmd_area( S1D135XX_CMD_LD_IMG_AREA, mode, area);
  send_cmd(S1D135XX_CMD_LD_IMG);
  send_param(mode);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  if (isinv == 1)
    transfer_data_inv(p_img, fontsize);
  else
    transfer_data(p_img, fontsize);

  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);

  return 0;
}

//    pl_epdc_updateuser(2, UPDATE_PARTIAL_AREA, NULL);
//wfid 刷新方式 0 1 2 3
//mode  UPDATE_PARTIAL_AREA  FULL
//区域  可以NULL 因为在上传图片时已经指定了区域
//注意，多区域刷新可能引起重叠，解决方案是指定刷新区域，统一或者分别刷新
int EPD4INC::EPD_UpdateUser(int wfid, enum pl_update_mode mode, const struct pl_area *area)
{
  if (s1d13541_update_temp(PL_EPDC_TEMP_INTERNAL))
    return -1;
  s1d135xx_set_epd_power(1);
  //  if (psu->on(psu))
  //    return -1;
  if (s1d135xx_update(wfid, mode, area))
    return -1;
  return 0;
}
int EPD4INC::EPD_UpdateUserFast(int wfid, enum pl_update_mode mode, const struct pl_area *area)
{
 
  if (s1d135xx_update(wfid, mode, area))
    return -1;
  return 0;
}
int EPD4INC::EPD_Fill(uint16_t mode, const struct pl_area *area, uint8_t grey)
{
  return s1d135xx_fill(mode, 8, area, grey);

}
//
//
////(240, 400, size, S1D13541_LD_IMG_4BPP);
//全屏幕传送 注意 mode 和size需要 匹配，1bpp 1200  2bpp2400  4bpp4800
//
int EPD4INC::EPD_Update_Full( uint16_t checker_size, uint16_t mode, const uint8_t *picimage)
{
  uint16_t i = 0, j = 0, k = 0;
  uint16_t val = 0;
  set_cs(0);
  send_cmd(S1D135XX_CMD_LD_IMG);
  send_param(mode);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  transfer_data(picimage, checker_size);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);
  return 0;
}

//测试 发送空白数据
//区域定义，是否反相
//只操作缓存，不进行显示更新
int EPD4INC::EPD_Fill_White(const struct pl_area *area, uint8_t isinv)
{
  uint16_t i = 0, j = 0, k = 0;
  uint16_t val = 0;
  uint16_t n = ((area->width) * (area->height)) / 16;
  set_cs(0);
  send_cmd_area( S1D135XX_CMD_LD_IMG_AREA, S1D13541_LD_IMG_1BPP, area);
  send_cmd(S1D135XX_CMD_LD_IMG);
  send_param(S1D13541_LD_IMG_1BPP);
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;

  set_cs(0);
  send_cmd(S1D135XX_CMD_WRITE_REG);
  send_param(S1D135XX_REG_HOST_MEM_PORT);
  if (isinv == 1)
  {

    while (n--)
      send_param(0xFFFF);
  }
  else
  {
    while (n--)
      send_param(0);
  }
  set_cs(1);
  if (s1d135xx_wait_idle())
    return -1;
  send_cmd_cs(S1D135XX_CMD_LD_IMG_END);
  return 0;
}
void EPD4INC::EPD_CLK_EX(void)
{
  ledcSetup(0, 80000000, 1);
  ledcAttachPin(17, 0);  //GPIO 17 配置为20MHz输出
  ledcWrite(0, 1);
}
void EPD4INC::EPD_CLK_STOP(void)
{
    ledcDetachPin(17);
}
void EPD4INC::epd_read_reg(void)
{
 
  Serial.println("read reg 0x0010  12 14 16 is ");
  int regdata;
  regdata = s1d135xx_read_reg(0x0016);
  Serial.println(regdata);
  delay(20);
  regdata = s1d135xx_read_reg(0x0014);
  Serial.println(regdata);
  delay(20);
  regdata = s1d135xx_read_reg(0x0012);
  Serial.println(regdata);
  delay(20);
  regdata = s1d135xx_read_reg(0x0010);
  Serial.println(regdata);

}
uint16_t EPD4INC::EPD_Check_ISinit(void)
{
   uint16_t regdata;
   regdata= s1d135xx_read_reg(S1D135XX_REG_MEM_ACCESS);
   Serial.println(regdata);   
   return regdata;
}

//屏幕旋转角度  00  0   01  90   02 180  03 270
int EPD4INC::EPD_Set_Rota(uint8_t turnrota)
{

  uint16_t  regdata = s1d135xx_read_reg(S1D135XX_REG_MEM_ACCESS);
   Serial.println("Set Screen ROTA IS 90°");
  Serial.println((regdata&0xFCFF)|(turnrota<<8));

  s1d135xx_write_reg(S1D135XX_REG_MEM_ACCESS, (regdata&0xFCFF)|(turnrota<<8)); 
 
  return 0;
}

//以上为对屏幕DRAM操作。以下为对CPU内RAM操作。
void EPD4INC::Buf_SetPix(int16_t x, int16_t y)
{

  //    int16_t temp=x;
  //   x=y;y=YDOT-1-temp;
  if (x < XDOT && y < YDOT)  EPDbuffer[x / 8 + y * XDOT / 8] &= ~(0x01 << x % 8) ;


}
void EPD4INC::Buf_InverSePixel(int16_t x, int16_t y)
{
  //    int16_t temp=x;
  //   x=y;y=YDOT-1-temp;
  if (x < XDOT && y < YDOT) EPDbuffer[x / 8 + y * XDOT / 8] ^= (0x01 << x % 8) ;

}

//注意X和Y反转 ，适应屏幕需求
void EPD4INC::Buf_DrawLine(int ystart, int xstart, int yend, int xend)
{
  int dx = abs(xend - xstart); int dy = abs(yend - ystart);
  if (dx >= dy)
  {
    if (xend > xstart)
    {
      for (int i = xstart; i <= xend; i++)
      {
        Buf_SetPix(i, round((i - xstart) * (yend - ystart) / (xend - xstart)) + ystart);
      }
    }
    else
    {
      for (int i = xstart; i >= xend; i--)
      {
        Buf_SetPix(i, round((i - xstart) * (yend - ystart) / (xend - xstart)) + ystart);
      }

    }
  }

  else
  {
    if (yend > ystart)
    {
      for (int i = ystart; i <= yend; i++)
      {
        Buf_SetPix(round((i - ystart) * (xend - xstart) / (yend - ystart)) + xstart, i);
      }
    }
    else
    {
      for (int i = ystart; i >= yend; i--)
      {
        Buf_SetPix(round((i - ystart) * (xend - xstart) / (yend - ystart)) + xstart, i);
      }

    }
  }
}
void EPD4INC::Buf_Clear()
{
  for (int i = 0; i < (XDOT * YDOT / 8); i++) EPDbuffer[i] = 0xff;
}

void EPD4INC::Buf_InverseArea(int xStart, int xEnd, int yStart, int yEnd)
{
  for (int i = 0; i < (xEnd - xStart); i++)
  {
    for (int j = 0; j < (yEnd - yStart); j++)
    {
      Buf_InverSePixel(xStart + i, yStart + j);
    }

  }
}
void EPD4INC::Buf_DrawCircle(int x, int y, int r, bool fill)
{
  if (fill == 0)
  {
    for (int i = 0; i < 360; i++)
    {
      Buf_SetPix(round(cos(i)*r + x), round(sin(i)*r) + y);

    }
  }
  else
  {
    for (int j = 0; j < r; j++)
    {

      for (int i = 0; i < 360; i++)
      {
        Buf_SetPix(round(cos(i)*j + x), round(sin(i)*j) + y);

      }
    }
  }
}
void EPD4INC::Buf_Buf_DrawXline(int start, int end, int x)
{
  for (int i = start; i <= end; i++)
  {
    Buf_SetPix(i, x);
  }
}
void EPD4INC::Buf_Buf_DrawYline(int start, int end,  int y)
{
  for (int i = start; i <= end; i++)
  {
    Buf_SetPix(y, i);
  }
}
void EPD4INC::Buf_DrawBox(byte x, int y, int w, int h)
{

  for (int i = x; i < x + h; i++)
  {
    Buf_Buf_DrawXline( y, y + w - 1,  i);
  }

}
void EPD4INC::Buf_DrawEmptyBox(int x, int y, int w, int h)
{
  Buf_Buf_DrawXline(y, y + w, x);
  Buf_Buf_DrawXline(y, y + w, x + h);
  Buf_Buf_DrawYline(x, x + w, y);
  Buf_Buf_DrawYline(x, x + w, y + w);
}
void EPD4INC::Buf_UpdateFull( uint16_t mode)
{
  //int EPD4INC::EPD_Update_Full( uint16_t checker_size, uint16_t mode, const uint8_t *picimage)

  EPD_Update_Full((XDOT * YDOT / 8), S1D13541_LD_IMG_1BPP, EPDbuffer);
 // EPD_UpdateUser(mode, UPDATE_FULL, NULL);
}
//更新缓存区域数据到显示屏并更新
//注意，X Y 需要是8的整数倍地址 否则无效
void EPD4INC::Buf_Update_PartArea(int x, int y, int w, int h, uint16_t mode)
{
  unsigned char partbuf[(w / 8 + 1) * (h / 8 + 1)];
  int xtemp, ytemp, i = 0;
  if ((x % 8) || (y % 8))
    return ;
  for (ytemp = 0; ytemp < (h / 8 + 1); ytemp++)
  {
  }

}
//测试有问题 
void EPD4INC::Buf_drawXbm(int16_t xMove, int16_t yMove, int16_t width, int16_t height, unsigned char *xbm) {
  int16_t heightInXbm = (height + 7) / 8;
  uint8_t Data;
  //外部变量
  int16_t fontscale = 1;
  int16_t CurrentCursor = 0;
  //
  //  for(int16_t x = 0; x < width; x++) {
  //    for(int16_t y = 0; y < height; y++ ) {
  //      if (y & 7) {
  //        Data <<= 1; // Move a bit
  //      } else {  // Read new Data every 8 bit
  //        Data = xbm[(y / 8) + x * heightInXbm];
  //      }
  //      // if there is a bit draw it
  //      if (((Data & 0x80)>>7)) {
  //       if(fontscale==1) {Buf_SetPix(xMove + y, yMove + x);CurrentCursor=x;}
  //       if(fontscale==2) {Buf_SetPix(xMove + y*2, yMove + x*2);Buf_SetPix(xMove + y*2+1, yMove + x*2);Buf_SetPix(xMove + y*2, yMove + x*2+1);Buf_SetPix(xMove + y*2+1, yMove + x*2+1);CurrentCursor=x*2;}
  //       //if(fontscale==2) {SetPixel(xMove + y*2, yMove + x*2);CurrentCursor=x*2;}
  //      }
  //    }
  //  }
  for (int16_t y = 0; y < height; y++) {
    
    for (int16_t x = 0; x < width; x++)
    {

      if(((xbm[(y*width+x)/8])>>((y*width+x)%8))==0x01)
          Buf_SetPix(x+xMove,yMove+y);

 

    }
  }

}
