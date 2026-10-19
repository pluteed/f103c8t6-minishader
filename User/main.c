#include "stm32f10x.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Delay.h"


float angleX = 0.0f;
float angleY = 0.0f;

int main(void)
{
    OLED_Init();
    MPU6050_Init();

    while (1)
    {
        int16_t gx, gy;


        MPU6050_GetGyroXY(&gx, &gy);


        angleX += gx / 2000.0f;
        angleY += gy / 2000.0f;


        render_frame(angleX, angleY);

        //for(int t=0; t<200000; t++);
    }
}