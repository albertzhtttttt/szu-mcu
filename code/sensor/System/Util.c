#include "stm32f10x.h"
#include "MPU6050.h"
#include "OLED.h"
#include "Delay.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

int base = 1700; // 基准延时，轮子不动

int16_t AX, GZ;	  // 定义用于存放各个数据的变量
float angleZ = 0; // 累计角度
float veloX;	  // 累计速度
extern uint8_t flag;

int testLeft(void)
{
	return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);
}

int testRight(void)
{
	return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6);
}

/**
 * @param num 发声次数（次数）
 * @param gap 时间间隔（毫秒）
 */
void beep(int num, unsigned int gap)
{
	int i;

	typedef struct
	{
		GPIO_TypeDef *port;
		uint16_t pin;
	} Buzzer;

	/*
		之所以会出现IO口不论高低蜂鸣器一直响的情况，是因为此蜂鸣器采用PNP型三极管，外接小车VCC为5V，
		而IO高电平为3.3V，电流从5.5V流向更低的3.3V，三极管导通，电流在蜂鸣器内形成回路，
		因此不论高电平（3.3V）还是低电平，蜂鸣器电路均会导通。
	*/
	Buzzer VCC = {GPIOD, GPIO_Pin_10}; // 模拟VCC
	Buzzer IO = {GPIOC, GPIO_Pin_10};  // IO口

	GPIO_SetBits(VCC.port, VCC.pin); // 模拟VCC

	for (i = 0; i < num; i++)
	{
		GPIO_ResetBits(IO.port, IO.pin); // 蜂鸣器低位有效
		Delay_ms(100);
		GPIO_SetBits(IO.port, IO.pin);
		if (i < num - 1)
			Delay_ms(gap);
	}
}

/**
 * @param color 颜色的英文小写首字母
 *
 * 红色（red）-> r     ||
 * 绿色（green）-> g   ||
 * 蓝色（blue）-> b    ||
 * 黄色（yellow）-> y  ||
 * 青色（cyan）-> c    ||
 * 洋红（magenta）-> m ||
 * 白色（white）-> w
 */
void light(char color)
{
	typedef struct
	{
		GPIO_TypeDef *port;
		uint16_t pin;
	} Color;

	// LED引脚顺序：红、VCC、绿、蓝
	Color red = {GPIOE, GPIO_Pin_6};
	Color green = {GPIOE, GPIO_Pin_2};
	Color blue = {GPIOC, GPIO_Pin_12};

	switch (color)
	{
	case 'r':
		GPIO_ResetBits(red.port, red.pin);
		GPIO_SetBits(green.port, green.pin);
		GPIO_SetBits(blue.port, blue.pin);
		break;
	case 'g':
		GPIO_ResetBits(green.port, green.pin);
		GPIO_SetBits(red.port, red.pin);
		GPIO_SetBits(blue.port, blue.pin);
		break;
	case 'b':
		GPIO_ResetBits(blue.port, blue.pin);
		GPIO_SetBits(red.port, red.pin);
		GPIO_SetBits(green.port, green.pin);
		break;
	case 'y':
		GPIO_ResetBits(red.port, red.pin);
		GPIO_ResetBits(green.port, green.pin);
		GPIO_SetBits(blue.port, blue.pin);
		break;
	case 'c':
		GPIO_ResetBits(green.port, green.pin);
		GPIO_ResetBits(blue.port, blue.pin);
		GPIO_SetBits(red.port, red.pin);
		break;
	case 'm':
		GPIO_ResetBits(red.port, red.pin);
		GPIO_ResetBits(blue.port, blue.pin);
		GPIO_SetBits(green.port, green.pin);
		break;
	case 'w':
		GPIO_ResetBits(red.port, red.pin);
		GPIO_ResetBits(green.port, green.pin);
		GPIO_ResetBits(blue.port, blue.pin);
		break;
	};
}

/**
 * @param offset 顺负逆正
 * @param angle 旋转角度，直走时为循环次数
 *
 * 当脉冲延时小于基准时，轮子顺时针转动
 * 当脉冲延时等于基准时，轮子停止转动
 * 当脉冲延时大于基准时，轮子逆时针转动
 *
 * 直走：左逆右顺 -> 左+ 右- ||
 * 左转：左某右顺 -> 左× 右- ||
 * 右转：左逆右某 -> 左+ 右×
 *
 * 注：某表示都可以，只是转法不同
 */
void run(int offset_left, int offset_right, int angle)
{
	int left = base + offset_left;
	int right = base + offset_right;

	// 处理直走情况
	if (offset_left + offset_right == 0)
	{
		flag = 2;
		for (int i = 0; i < angle; i++)
		{
			if (angleZ > 2.5)
			{
				while (angleZ > 0)
				{
					GPIO_SetBits(GPIOE, GPIO_Pin_4);
					Delay_us(base);
					GPIO_ResetBits(GPIOE, GPIO_Pin_4);

					GPIO_SetBits(GPIOE, GPIO_Pin_5);
					Delay_us(left);
					GPIO_ResetBits(GPIOE, GPIO_Pin_5);

				}

				continue;
			}

			if (angleZ < -2.5)
			{
				while (angleZ < 0)
				{
					GPIO_SetBits(GPIOE, GPIO_Pin_5);
					Delay_us(base);
					GPIO_ResetBits(GPIOE, GPIO_Pin_5);

					GPIO_SetBits(GPIOE, GPIO_Pin_4);
					Delay_us(right);
					GPIO_ResetBits(GPIOE, GPIO_Pin_4);

				}

				continue;
			}

			GPIO_SetBits(GPIOE, GPIO_Pin_5);
			Delay_us(left);
			GPIO_ResetBits(GPIOE, GPIO_Pin_5);

			GPIO_SetBits(GPIOE, GPIO_Pin_4);
			Delay_us(right);
			GPIO_ResetBits(GPIOE, GPIO_Pin_4);

			Delay_ms(10);
		}
		flag = 0;

		return;
	}

	angleZ = 0;

	while (1)
	{
		flag = 1;

		// 提前刹车
		if (ABS(angleZ + GZ / 32767.0 * 250 * 0.1) >= angle)
		{
			flag = 0;
			for (uint8_t i = 0; i < GZ / 5000; i++) // 校准参数
			{
				GPIO_SetBits(GPIOE, GPIO_Pin_5);
				Delay_us(base - offset_left);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);

				GPIO_SetBits(GPIOE, GPIO_Pin_4);
				Delay_us(base - offset_right);
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);

				Delay_ms(10);
			}

			angleZ = 0;

			break;
		}

		// // 防止惯性甩尾
		// if (ABS(GZ) > 10000)
		// {
		// 	continue;
		// }

		OLED_ShowSignedNum(2, 1, AX, 5);
		OLED_ShowSignedNum(2, 8, GZ, 5);

		// 执行控制逻辑
		GPIO_SetBits(GPIOE, GPIO_Pin_5);
		Delay_us(left - offset_left * GZ / 32767);
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);

		GPIO_SetBits(GPIOE, GPIO_Pin_4);
		Delay_us(right - offset_right * GZ / 32767);
		GPIO_ResetBits(GPIOE, GPIO_Pin_4);

		Delay_ms(10);
	}
	OLED_Clear();
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) // 判断是否是TIM2的更新事件触发的中断
	{
		if (flag == 1)
		{
			GZ = (MPU6050_ReadReg(0x47) << 8) | MPU6050_ReadReg(0x48);
			if (ABS(GZ) < 500)
				GZ = 0; // 消除噪声

			angleZ += GZ / 32767.0 * 250 * 0.1; // 计算累计角度
			OLED_ShowString(3, 1, "degree:");
			OLED_ShowSignedNum(3, 8, angleZ, 3);
			OLED_ShowSignedNum(2, 1, GZ, 5);
		}
		if (flag == 2)
		{
			AX = (MPU6050_ReadReg(0x3B) << 8) | MPU6050_ReadReg(0x3C);
			GZ = (MPU6050_ReadReg(0x47) << 8) | MPU6050_ReadReg(0x48);

			veloX = AX / 32767.0 * 2 * 9.8 * 0.1 * 100; // cm/s
			angleZ += GZ / 32767.0 * 250 * 0.1;			// 计算累计角度

			OLED_ShowString(3, 1, "velocity:");
			OLED_ShowSignedNum(3, 10, veloX, 3);
			OLED_ShowSignedNum(2, 1, AX, 5);
			OLED_ShowSignedNum(2, 8, GZ, 5);
			OLED_ShowString(4, 1, "degree:");
			OLED_ShowSignedNum(4, 8, angleZ, 5);
		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除TIM2更新事件的中断标志位
													// 中断标志位必须清除
													// 否则中断将连续不断地触发，导致主程序卡死
	}
}
