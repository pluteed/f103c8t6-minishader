
#include "stm32f10x.h"
#include "OLED.h"
#include "OLED_Font.h"
#include <math.h> 



static uint8_t framebuffer[FB_SIZE];




// 16棱柱 32个顶点（完整正确）
const Vec4 model_points[32] = {
    // 上底面 Y = 0.35f
    { 0.35f,  0.35f,  0.000f, 1.0f },
    { 0.33f,  0.35f,  0.12f, 1.0f },
    { 0.29f,  0.35f,  0.23f, 1.0f },
    { 0.22f,  0.35f,  0.30f, 1.0f },
    { 0.14f,  0.35f,  0.33f, 1.0f },
    { 0.05f,  0.35f,  0.33f, 1.0f },
    {-0.04f,  0.35f,  0.28f, 1.0f },
    {-0.12f,  0.35f,  0.20f, 1.0f },
    {-0.18f,  0.35f,  0.09f, 1.0f },
    {-0.20f,  0.35f, -0.03f, 1.0f },
    {-0.19f,  0.35f, -0.14f, 1.0f },
    {-0.14f,  0.35f, -0.23f, 1.0f },
    {-0.07f,  0.35f, -0.30f, 1.0f },
    { 0.02f,  0.35f, -0.33f, 1.0f },
    { 0.11f,  0.35f, -0.32f, 1.0f },
    { 0.19f,  0.35f, -0.27f, 1.0f },

    // 下底面 Y = -0.35f
    { 0.35f, -0.35f,  0.000f, 1.0f },
    { 0.33f, -0.35f,  0.12f, 1.0f },
    { 0.29f, -0.35f,  0.23f, 1.0f },
    { 0.22f, -0.35f,  0.30f, 1.0f },
    { 0.14f, -0.35f,  0.33f, 1.0f },
    { 0.05f, -0.35f,  0.33f, 1.0f },
    {-0.04f, -0.35f,  0.28f, 1.0f },
    {-0.12f, -0.35f,  0.20f, 1.0f },
    {-0.18f, -0.35f,  0.09f, 1.0f },
    {-0.20f, -0.35f, -0.03f, 1.0f },
    {-0.19f, -0.35f, -0.14f, 1.0f },
    {-0.14f, -0.35f, -0.23f, 1.0f },
    {-0.07f, -0.35f, -0.30f, 1.0f },
    { 0.02f, -0.35f, -0.33f, 1.0f },
    { 0.11f, -0.35f, -0.32f, 1.0f },
    { 0.19f, -0.35f, -0.27f, 1.0f }
};

// 16棱柱 边表（完整 48 条边）
const uint8_t edges[48][2] = {
    // 上底面 16 条边
    {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},
    {8,9},{9,10},{10,11},{11,12},{12,13},{13,14},{14,15},{15,0},

    // 下底面 16 条边
    {16,17},{17,18},{18,19},{19,20},{20,21},{21,22},{22,23},{23,24},
    {24,25},{25,26},{26,27},{27,28},{28,29},{29,30},{30,31},{31,16},

    // 竖边 16 条
    {0,16},{1,17},{2,18},{3,19},{4,20},{5,21},{6,22},{7,23},
    {8,24},{9,25},{10,26},{11,27},{12,28},{13,29},{14,30},{15,31}
};



/*引脚初始化*/
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}



/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}

void draw_pixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    uint16_t idx = page * SCREEN_W + x;
    
    if (idx >= FB_SIZE) return;
    
    if (color)
        framebuffer[idx] |= (1 << bit);
    else
        framebuffer[idx] &= ~(1 << bit);
}

void oled_refresh(void){
	for(uint8_t page=0;page<8;page++){
		OLED_SetCursor(page,0);
		for(uint8_t col=0;col<128;col++){
			OLED_WriteData(framebuffer[page*128+col]);
		}
	}
}


void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;
    
    while (1) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_triangle(Vec4 p0,Vec4 p1,Vec4 p2,uint8_t color){
	Vec4 v0=viewport(p0,SCREEN_W,SCREEN_H);
	Vec4 v1=viewport(p1,SCREEN_W,SCREEN_H);
	Vec4 v2=viewport(p2,SCREEN_W,SCREEN_H);
	
	draw_line((int16_t)v0.x, (int16_t)v0.y, (int16_t)v1.x, (int16_t)v1.y, color);
    draw_line((int16_t)v1.x, (int16_t)v1.y, (int16_t)v2.x, (int16_t)v2.y, color);
    draw_line((int16_t)v2.x, (int16_t)v2.y, (int16_t)v0.x, (int16_t)v0.y, color);
}

void clear_framebuffer(void){
	for(int i=0;i<FB_SIZE;i++){
		framebuffer[i]=0;
	}
}










void render_frame(float angleY, float angleX)
{
    clear_framebuffer();

    int edge_count = sizeof(edges) / sizeof(edges[0]);

    for (int i = 0; i < edge_count; i++)
    {
        int a = edges[i][0];
        int b = edges[i][1];

        Vec4 v0 = model_points[a];
        Vec4 v1 = model_points[b];


        v0 = RotateX(v0, angleX);
        v0 = RotateY(v0, angleY);
        v1 = RotateX(v1, angleX);
        v1 = RotateY(v1, angleY);

        // 2. 相机平移
        v0.z += 1.1f;
        v1.z += 1.1f;

        // 3. 透视投影
        v0 = perspective(v0, 90, 2, 0.1f, 10.0f);
        v1 = perspective(v1, 90, 2, 0.1f, 10.0f);

        // 4. 透视除法
        if (fabs(v0.w) > 0.1f) {
            v0.x /= v0.w;
            v0.y /= v0.w;
        }
        if (fabs(v1.w) > 0.1f) {
            v1.x /= v1.w;
            v1.y /= v1.w;
        }

       
        v0 = viewport(v0,SCREEN_W,SCREEN_H);
        v1 = viewport(v1,SCREEN_W,SCREEN_H);


        draw_line((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, 1);
    }

    oled_refresh();
}
