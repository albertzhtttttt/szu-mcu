#include "stm32f10x.h"
#include "open.h"
#include "adc.h"
#include "Util.h"
#include "OLED.h"
#include "Timer.h"
#include "MPU6050.h"

uint8_t flag = 0; 

int main(void)
{
	Open_Init();
	ADC1_Init();
	OLED_Init();
	Timer_Init();
	MPU6050_Init();

	OLED_Clear();
	while (1)
	{
		// 直走
		light('g');
		OLED_ShowString(1, 1, "No Obstacle");
		run(200, -200, 1);

		if (!testLeft())
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "Found Obstacle");

			light('r');
			beep(1, 100); // 鸣笛

			// 左转
			light('b');
			run(-200, -200, 90);
			// 直走
			light('y');
			run(200, -200, 50);
			// 右转
			light('c');
			run(200, 200, 90);
			// 直走
			light('m');
			run(200, -200, 100);
		}
	}
}
