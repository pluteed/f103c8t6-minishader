#ifndef __OLED_H
#define __OLED_H
#include "My_Geometry.h"

/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

#define SCREEN_W 128
#define SCREEN_H 64
#define FB_PAGES 8              // 64行 ÷ 8 = 8页
#define FB_SIZE (SCREEN_W * FB_PAGES)   // 128 × 8 = 1024 字节
extern uint8_t framebuffer[FB_SIZE];

#define POINT_NUM 16
extern Vec4 shape_pts[POINT_NUM];
extern Vec4 shape_buf[POINT_NUM];



void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void draw_pixel(int16_t x,int16_t y,uint8_t color);
void oled_refresh(void);
void draw_triangle(Vec4 p0,Vec4 p1,Vec4 p2,uint8_t color);
void clear_framebuffer(void);
void render_frame(float angleY, float angleX);

#endif
