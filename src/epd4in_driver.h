#ifndef _EPD4IN_DRIVER_H_
#define _EPD4IN_DRIVER_H_

#include <string.h>

#include <Arduino.h>
#include <SPI.h>

#include "FS.h"
#include "PIC.h"

#define XDOT  240
#define YDOT  400



#define S1D13541_STATUS_HRDY            (1 << 13)

#define S1D13524_PLLCFG0                0x340F
#define S1D13524_PLLCFG1                0x0300
#define S1D13524_PLLCFG2                0x1680
#define S1D13524_PLLCFG3                0x1880
#define S1D13541_PROD_CODE                0x0053
//#define S1D13541_STATUS_HRDY          (1 << 13)
#define S1D13541_INTERNAL_CLOCK_ENABLE  (1 << 7)
#define S1D13541_I2C_CLOCK_DIV          7 /* 100 kHz */
#define S1D13541_PROT_KEY_1             0x5678 /* ToDo: add to s1d135xx_data */
#define S1D13541_PROT_KEY_2             0x1234
#define S1D13541_TEMP_SENSOR_CONTROL    (1 << 14)
#define S1D13541_TEMP_SENSOR_EXTERNAL   (1 << 6)
#define S1D13541_AUTO_TEMP_JUDGE_EN     (1 << 2)
#define S1D13541_GENERIC_TEMP_EN        (1 << 15)
#define S1D13541_GENERIC_TEMP_JUDGE_EN  (1 << 14)
#define S1D13541_GENERIC_TEMP_MASK      0x01FF
#define S1D13541_INT_RAW_WF_UPDATE      (1 << 14)
#define S1D13541_INT_RAW_OUT_OF_RANGE   (1 << 10)
#define S1D13541_LD_IMG_1BPP            (0 << 4)
#define S1D13541_LD_IMG_2BPP            (1 << 4)
#define S1D13541_LD_IMG_4BPP            (2 << 4)
#define S1D13541_LD_IMG_8BPP            (3 << 4)
#define S1D13541_WF_ADDR                0x00080000L

#define S1D13541_PROM_STATUS             0x0500
#define S1D13541_PROM_CTRL               0x0502
#define S1D13541_PROM_ADR_PGR_DATA       0x0504
#define S1D13541_PROM_READ_DATA          0x0506

#define S1D13541_PROM_STATUS_IDLE               0x0
#define S1D13541_PROM_STATUS_READ_BUSY          (1 << 8)
#define S1D13541_PROM_STATUS_PGM_BUSY           (1 << 9)
#define S1D13541_PROM_STATUS_ERASE_BUSY (1 << 10)
#define S1D13541_PROM_STATUS_READ_MODE          (1 << 12)
#define S1D13541_PROM_STATUS_PGM_MODE           (1 << 13)
#define S1D13541_PROM_STATUS_ERASE_ALL_MODE     (1 << 14)

#define S1D13541_PROM_READ_START                (1 << 0)
#define S1D13541_PROM_READ_STOP                 (1 << 1)
#define S1D13541_PROM_VCOM_READ                 (1 << 2)
#define S1D13541_PROM_PGM_MODE_START            (1 << 4)
#define S1D13541_PROM_PGM_OP_START                     (1 << 5)
#define S1D13541_PROM_PGM_OP_STOP               (1 << 6)
#define S1D13541_PROM_PGM_MODE_STOP             (1 << 7)
#define S1D13541_PROM_ERASE_ALL_MODE_START      (1 << 8)
#define S1D13541_PROM_ERASE_ALL_OP_START        (1 << 9)
#define S1D13541_PROM_ERASE_ALL_OP_STOP (1 << 10)
#define S1D13541_PROM_ERASE_ALL_MODE_STOP       (1 << 11)



/* Set to 1 to enable verbose update and EPD power on/off log messages */
#define VERBOSE 0

#define DATA_BUFFER_LENGTH              2048 // must be above maximum xres value for any supported display

#define S1D135XX_WF_MODE(_wf)           (((_wf) << 8) & 0x0F00)
#define S1D135XX_XMASK                  0x0FFF
#define S1D135XX_YMASK                  0x0FFF
#define S1D135XX_INIT_CODE_CHECKSUM_OK  (1 << 15)
#define S1D135XX_PWR_CTRL_UP            0x8001
#define S1D135XX_PWR_CTRL_DOWN          0x8002
#define S1D135XX_PWR_CTRL_BUSY          0x0080
#define S1D135XX_PWR_CTRL_CHECK_ON      0x2200

#define SPICS_Pin   15
#define EN3P3_Pin   32
#define NRST_Pin    4
#define WKUP_Pin    17
#define SHDN_Pin    5
//#define HVEN_Pin    5
#define HDC_Pin     2
#define HIRQ_Pin    33    //NOT USE

/* Set to 1 to enable verbose temperature log messages */
#define VERBOSE_TEMPERATURE                  0
#define S1D135XX_TEMP_MASK                   0x00FF


/* Set to 1 to enable stub EPDC implementation */
#define PL_EPDC_STUB 0

/* Use this macro to convert a 16-greyscale value to 8 bits */
#define PL_GL16(_g) ({      \
    uint8_t g16 = (_g) & 0xF; \
    g16 | g16 << 4;     \
  })

#define PL_WHITE PL_GL16(15)
#define PL_BLACK PL_GL16(0)

enum pl_epdc_power_state {
  PL_EPDC_RUN = 0,
  PL_EPDC_STANDBY,
  PL_EPDC_SLEEP,
  PL_EPDC_OFF,
};

enum pl_epdc_temp_mode {
  PL_EPDC_TEMP_MANUAL,
  PL_EPDC_TEMP_EXTERNAL,
  PL_EPDC_TEMP_INTERNAL,
};

enum pl_update_mode {
  UPDATE_FULL        = 0, //0x33,
  UPDATE_FULL_AREA   = 1, //0x34,
  UPDATE_PARTIAL      = 2, //0x35,
  UPDATE_PARTIAL_AREA = 3, //0x36,
};
enum s1d13524_cmd {
  S1D13524_CMD_INIT_PLL           = 0x01,
  S1D13524_CMD_INIT_CTLR_MODE     = 0x0E,
  S1D13524_CMD_RD_WF_INFO         = 0x30,
};

enum s1d135xx_cmd {
  S1D135XX_CMD_INIT_SET            = 0x00, /* to load init code */
  S1D135XX_CMD_RUN                 = 0x02,
  S1D135XX_CMD_STBY                = 0x04,
  S1D135XX_CMD_SLEEP               = 0x05,
  S1D135XX_CMD_INIT_STBY           = 0x06, /* init then standby */
  S1D135XX_CMD_INIT_ROT_MODE       = 0x0B,
  S1D135XX_CMD_READ_REG            = 0x10,
  S1D135XX_CMD_WRITE_REG           = 0x11,
  S1D135XX_CMD_BST_RD_SDR          = 0x1C,
  S1D135XX_CMD_BST_WR_SDR          = 0x1D,
  S1D135XX_CMD_BST_END_SDR         = 0x1E,
  S1D135XX_CMD_LD_IMG              = 0x20,
  S1D135XX_CMD_LD_IMG_AREA         = 0x22,
  S1D135XX_CMD_LD_IMG_END          = 0x23,
  S1D135XX_CMD_WAIT_DSPE_TRG       = 0x28,
  S1D135XX_CMD_WAIT_DSPE_FREND     = 0x29,
  S1D135XX_CMD_UPD_INIT            = 0x32,
  S1D135XX_CMD_UPDATE_FULL         = 0x33,
  S1D135XX_CMD_UPDATE_FULL_AREA    = 0x34,
  S1D135XX_CMD_UPDATE_PARTIAL      = 0x35,
  S1D135XX_CMD_UPDATE_PARTIAL_AREA = 0x36,
  S1D135XX_CMD_EPD_GDRV_CLR        = 0x37,
};

enum s1d13541_reg {
  S1D13541_REG_CLOCK_CONFIG          = 0x0010,
  S1D13541_REG_PROT_KEY_1            = 0x042C,
  S1D13541_REG_PROT_KEY_2            = 0x042E,
  S1D13541_REG_FRAME_DATA_LENGTH     = 0x0400,
  S1D13541_REG_LINE_DATA_LENGTH      = 0x0406,
  S1D13541_REG_WF_DECODER_BYPASS     = 0x0420,
  S1D13541_REG_TEMP_SENSOR_VALUE     = 0x0576,
  S1D13541_REG_GENERIC_TEMP_CONFIG   = 0x057E,
};

enum s1d13541_cmd {
  S1D13541_CMD_RD_TEMP               = 0x12,
};

enum s1d135xx_reg {
  S1D135XX_REG_REV_CODE              = 0x0002,
  S1D135XX_REG_SOFTWARE_RESET        = 0x0008,
  S1D135XX_REG_SYSTEM_STATUS         = 0x000A,
  S1D135XX_REG_I2C_CLOCK             = 0x001A,
  S1D135XX_REG_PERIPH_CONFIG         = 0x0020,
  S1D135XX_REG_HOST_MEM_PORT         = 0x0154,
  S1D135XX_REG_I2C_TEMP_SENSOR_VALUE = 0x0216,
  S1D135XX_REG_I2C_STATUS            = 0x0218,
  S1D135XX_REG_PWR_CTRL              = 0x0230,
  S1D135XX_REG_SEQ_AUTOBOOT_CMD      = 0x02A8,
  S1D135XX_REG_DISPLAY_BUSY          = 0x0338,
  S1D135XX_REG_INT_RAW_STAT          = 0x033A,
  S1D135XX_REG_MEM_ACCESS            = 0x0140,
};

enum s1d135xx_rot_mode {
  S1D135XX_ROT_MODE_0   = 0,
  S1D135XX_ROT_MODE_90  = 1,
  S1D135XX_ROT_MODE_180 = 2,
  S1D135XX_ROT_MODE_270 = 3,
};

enum epson_epdc_ref {
  EPSON_EPDC_S1D13524,
  EPSON_EPDC_S1D13541,
};

struct s1d135xx_data {
  unsigned reset;
  unsigned cs0;
  unsigned hirq;
  unsigned hrdy;
  unsigned hdc;
  unsigned clk_en;
  unsigned vcc_en;
};

struct pl_area {
  unsigned left;
  unsigned top;
  unsigned width;
  unsigned height;
};

struct s1d135xx {
  const struct s1d135xx_data *data;
  struct pl_gpio *gpio;
  struct pl_interface *interface;
  uint16_t scrambling;
  uint16_t source_offset;
  uint16_t hrdy_mask;
  uint16_t hrdy_result;
  int measured_temp;
  unsigned xres;
  unsigned yres;
  struct {
    uint8_t needs_update: 1;
  } flags;
};


struct pl_wfid {
  int id_from;
  int id_to;
  //  const char *path;
  //  int id;
};

extern unsigned char UNICODEbuffer[1024];
extern String fontname;

enum FONT {
  FONT12 = 0,
  FONT8 = 1,
  FONT15 = 2,
  FONT16 = 3,
  FONT24 = 4,
  FONT32 = 5,
  FONT16_NUM = 6,
  FONT32_NUM = 7,
  FONT48_NUM = 8,
  FONT64_NUM = 9,
 
 

};
struct font_type {
  uint16_t font_lo;   //字库存储的位置  1 文件系统 0 flash直接存储 3编译存储
  int     Epd_Fontfont_addr; //
  char  *font_str_p;  //直接存储的指针 
  uint16_t cn_px;      //长宽默认一致的 汉字字符X
  uint16_t cn_py;      //字符Y
  uint16_t en_px;      //英文字符X
  uint16_t en_py;      //英文字符y
  uint16_t part_dx;    //行间距
  uint16_t Font_Index; //FOUNT
  String Fontname_En;
  String Fontname_Cn;
};


enum epd_type {
  EPD4IN = 1,
  WF29 = 2,
};

class EPD4INC {



  public:

    unsigned char  EPDbuffer[400 * 240 / 8];
    void EPD4INC_Port_init(void);
    void EPD4INC_Port_Reinit(void);
    int EPD4INC_Power_up( void);
    int EPD4INC_Power_ReUp();
    int EPD4INC_Power_down( void);
    int EPD4INC_Power_Sleep( void);
    int EPD4INC_Power_wakeup( void);
    int EPD4INC_HVEN( uint16_t vcom_ratio);
    int EPD4INC_HVDISEN( void);
    void EPD_SetFount( FONT font_index) ;
    int UTFtoUNICODE(unsigned char *code);

    uint16_t DrawUnicodeChar(int16_t x, int16_t y,  unsigned char *code, unsigned char isinv);
    int DrawUnicodeStr(int16_t x, int16_t y, uint16_t strle,  unsigned char *code, unsigned char isinv);
    int DrawUnicodeStr(int16_t x, int16_t y, uint16_t strle,  unsigned char *code, unsigned char isinv, int *drawLength);
    int UnicodeCharWidth(unsigned char *code);
    void DrawUTF(int16_t x, int16_t y, String code, unsigned char isinv);
    void DrawUTF(int16_t x, int16_t y, unsigned char *code, unsigned char isinv);
    void DrawUTF_lhb(int16_t x, int16_t y, String code, unsigned char isinv);
    int DrawUTF_hz(int16_t x, int16_t y, String code, unsigned char isinv);
    int DrawUTF_hz(int16_t x, int16_t y, String code, unsigned char isinv, int offset);
    void DrawTime(int16_t x, int16_t y, uint16_t hour, uint16_t minute, FONT font_index, unsigned char isinv);
    void DrawCalendarDay(int16_t x, int16_t y, uint16_t day, FONT font_index, unsigned char isinv);
    ////////////////////////////
    int EPD_Set_Rota(uint8_t turnrota);
    int epson_epdc_clear_init();
    int epson_epdc_wait_update_end();
    int epson_epdc_set_power(enum pl_epdc_power_state state);
    int epson_epdc_set_epd_power(int on);
    int epson_epdc_early_init();
    void s1d135xx_hard_reset();
    int s1d135xx_soft_reset();
    int s1d135xx_check_prod_code(uint16_t ref_code);
    int transfer_file(File *file);
    int s1d135xx_load_init_code();

    int s1d135xx_load_wflib(uint32_t size, uint32_t addr);
    int wflib_wr_file(File *file);
    int s1d135xx_wait_dspe_trig();
    int s1d135xx_init_gate_drv();
    int s1d135xx_clear_init();
    int s1d135xx_fill(uint16_t mode, unsigned bpp, const struct pl_area *a, uint8_t grey);
    int s1d135xx_pattern_check(uint16_t height, uint16_t width, uint16_t checker_size, uint16_t mode);
    int s1d135xx_update(int wfid, enum pl_update_mode mode,  const struct pl_area *area);
    int s1d135xx_wait_update_end();
    int s1d135xx_wait_idle();
    int s1d135xx_set_power_state(enum pl_epdc_power_state state);
    int s1d135xx_set_epd_power(int on);
    void s1d135xx_cmd(uint16_t cmd, uint16_t *params, size_t n);
    uint16_t s1d135xx_read_reg(uint16_t reg);
    void s1d135xx_write_reg(uint16_t reg, uint16_t val);
    int get_hrdy();
    int do_fill(const struct pl_area *area, unsigned bpp, uint8_t g);
    int wflib_wr(const uint8_t *data, size_t n);
    void transfer_data(const uint8_t *data, size_t n);
    void send_cmd_area(uint16_t cmd, uint16_t mode, const struct pl_area *area);
    void send_cmd_cs(uint16_t cmd);
    void send_cmd(uint16_t cmd);
    void send_params(uint16_t *params, size_t n);
    void send_param(uint16_t param);
    void set_cs(int state);
    void set_hdc(int state);
    int set_init_rot_mode();
    int s1d13541_load_wflib();
    int s1d13541_set_temp_mode(enum pl_epdc_temp_mode mode);
    int s1d13541_update_temp(enum pl_epdc_temp_mode temp_mode);
    int s1d13541_fill(const struct pl_area *area, uint8_t grey);
    int s1d13541_pattern_check(uint16_t size);
    int s1d13541_pattern_check_test_part(uint16_t size, const struct pl_area *area);
    int epson_epdc_early_init_s1d13541();
    int epson_epdc_init();
    int epson_epdc_init_s1d13541();
    int s1d13541_init_clocks();
    void update_temp(uint16_t reg);
    int update_temp_auto(uint16_t temp_reg);
    int s1d13541_read_prom(uint8_t * blob);
    int wait_for_ack (uint16_t status, uint16_t mask);
    int  pl_epdc_single_update(int wfid, enum pl_update_mode mode, const struct pl_area *area);
    void  transfer_data_inv(const uint8_t *data, size_t n);
    int User_Img_Tran(uint16_t height, uint16_t width, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv);
    int User_Font_Tran(uint16_t fontsize, const  uint8_t* p_img, uint16_t mode, const struct pl_area *area, uint8_t isinv);
    
    int EPD_UpdateUser(int wfid, enum pl_update_mode mode, const struct pl_area *area);
    int EPD_UpdateUserFast(int wfid, enum pl_update_mode mode, const struct pl_area *area) ;   
    int EPD_Fill(uint16_t mode, const struct pl_area *area, uint8_t grey);
    int EPD_Update_Full( uint16_t checker_size, uint16_t mode, const uint8_t *picimage);
    int EPD_Fill_White(const struct pl_area *area, uint8_t isinv);
    void EPD_CLK_EX(void);
    void EPD_CLK_STOP(void);
    void epd_read_reg(void);
    uint16_t EPD_Check_ISinit(void);
    void Buf_SetPix(int16_t x, int16_t y);
    void Buf_InverSePixel(int16_t x, int16_t y);
    void Buf_DrawLine(int xstart, int ystart, int xend, int yend);
    void Buf_Clear();
    void Buf_UpdateFull( uint16_t mode);
    void Buf_InverseArea(int xStart, int xEnd, int yStart, int yEnd);
    void Buf_DrawCircle(int x, int y, int r, bool fill);

    void Buf_Buf_DrawXline(int start, int end, int x);
    void Buf_Buf_DrawYline(int start, int end,  int y);
    void Buf_DrawBox(byte x, int y, int w, int h);
    void Buf_DrawEmptyBox(int x, int y, int w, int h);
    void Buf_Update_PartArea(int x, int y, int w, int h, uint16_t mode);
    void Buf_drawXbm(int16_t xMove, int16_t yMove, int16_t width, int16_t height, unsigned char *xbm);
};



#endif
