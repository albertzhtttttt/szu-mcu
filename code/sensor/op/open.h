#ifndef __OPEN_H
#define __OPEN_H

#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"

GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus; /* ö�ٱ������������ʱ�ӵ���״̬ */
NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

/*******************************************************************************
 * Function Name  : RCC_Configuration
 * Description    : Configures System Clocks
 *******************************************************************************/

void RCC_Configuration(void)
{
  /* ������RCC�Ĵ�������ΪĬ��ֵ�����йؼĴ�����λ�����ú������ĶRCC_CR��HSITRIM[4:0]λ��Ҳ�����üĴ���RCC_BDCR�ͼĴ���RCC_CSR */
  RCC_DeInit();
  /* ʹ���ⲿHSE���پ��� */
  RCC_HSEConfig(RCC_HSE_ON);
  /* �ȴ�HSE���پ����ȶ��������ڳ�ʱ��������˳� */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  /* SUCCESS:HSE�����ȶ��Ҿ�����ERROR��HSE����δ���� */
  if (HSEStartUpStatus == SUCCESS)
  {
    /* ʹ��flashԤȡָ��������������RCCûֱ�ӹ�ϵ */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* ����FLASH�洢����ʱʱ����������2����Ը�Ƶʱ�ӵģ�
    FLASH_Latency_0��0��ʱ���ڣ�FLASH_Latency_1��1��ʱ����
    FLASH_Latency_2��2��ʱ���� */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK=SYSCLK ���ø�������ʱ��=ϵͳʱ�� */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    /* PCLK1=HCLK/2 ���õ�������1ʱ��=����ʱ�ӵĶ���Ƶ*/
    RCC_PCLK1Config(RCC_HCLK_Div2);
    /* PCLK2=HCLK ���õ�������2ʱ��=��������ʱ�� */
    RCC_PCLK2Config(RCC_HCLK_Div1);
    /* Set PLL clock output to 72MHz using HSE (8MHz) as entry clock */
    /* �������໷��HSE�ⲿ8MHz����9��Ƶ��72MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* Enable PLL��ʹ��PLL���໷ */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready���ȴ����໷����ȶ� */
    /* RCC_FLAG_HSIRDY��HSI���������RCC_FLAG_HSERDY��HSE�������
       RCC_FLAG_PLLRDY��PLL������RCC_FLAG_LSERDY��LSE�������
       RCC_FLAG_LSIRDY��LSI���������RCC_FLAG_PINRST�����Ÿ�λ
       RCC_FLAG_PORRST��POR/PDR��λ��RCC_FLAG_SFTRST��������λ
       RCC_FLAG_IWDGRST��IWDG��λ��RCC_FLAG_WWDGRST��WWDG��λ
       RCC_FLAG_LPWRRST���͹��ĸ�λ */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
      ;

    /* Select PLL as system clock source�������໷�������Ϊϵͳʱ�� */
    /* RCC_SYSCLKSource_HSI��ѡ��HSI��Ϊϵͳʱ��
       RCC_SYSCLKSource_HSE��ѡ��HSE��Ϊϵͳʱ��
       RCC_SYSCLKSource_PLLCLK��ѡ��PLL��Ϊϵͳʱ��*/
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* �ȴ�PLL��Ϊϵͳʱ�ӱ�־λ��λ */
    /* 0x00��HSI��Ϊϵͳʱ�ӣ�0x04��HSE��Ϊϵͳʱ��
       0x08��PLL��Ϊϵͳʱ�� */
    while (RCC_GetSYSCLKSource() != 0x08)
      ;
  }

  /* Enable GPIOA~E and AFIO clocks��ʹ����Χ�˿�����ʱ�ӡ�ע�������������������ͬоƬ�Ϳ�����ķ��䲻ͬ*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
  /* USART1 clock enable��USART1ʱ��ʹ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  /* TIM1 clock enable��TIM1ʱ��ʹ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  /* TIM2 clock enable��TIM2ʱ��ʹ��*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* ADC1 clock enable��ADC1ʱ��ʹ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

/*******************************************************************************
 * Function Name  : GPIO_Configuration
 * Description    : Configures GPIO Modes
 *******************************************************************************/

void GPIO_Configuration(void)
{
  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // ������������
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //	  /* Configure LEDs IO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ����Ϊ����
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // ����PE8/PE10/PE

  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12;
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����Ϊ���
  // GPIO_Init(GPIOE,&GPIO_InitStructure);

  // �����˶�
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ����Ϊ���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ����Ϊ���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Configure USART1 Rx (PA.10) as input floating */
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  //	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //   GPIO_Init(GPIOA, &GPIO_InitStructure);

  //	  /* Configure LEDs IO */
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  //	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //   GPIO_Init(GPIOB, &GPIO_InitStructure);

  //	  /* Configure Motors I/O */
  //	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  //    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //    GPIO_Init(GPIOC, &GPIO_InitStructure);

  //	  /* Configure KEY I/O PC8 to PC11 */
  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOC, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOC, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOC, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOC, &GPIO_InitStructure);
  //
  //		/* Configure infrared IO*/
  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOE, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOE, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  //		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOE, &GPIO_InitStructure);

  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  //		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOE, &GPIO_InitStructure);

  //		/* GPIOA Configuration: Channel 1, 2, 3 and 4 Output */
  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOA, &GPIO_InitStructure);
  //		GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

  //		/* GPIOB Configuration: Channel 1N, 2N and 3N Output */
  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOB, &GPIO_InitStructure);
  //
  //		/* GPIOB Configuration: BKIN pin */
  //		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  //		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
 * Function Name  : NVIC_Configuration
 * Description    : Configures NVIC Modes for T1
 *******************************************************************************/

void NVIC_Configuration(void)
{

#ifdef VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

  /* Configure the NVIC Preemption Priority Bits[�������ȼ���] */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* Configure INT IO  PC9 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure INT IO  PE4 enable exti4*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure INT IO  PE5 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Enable the TIM2 gloabal Interrupt [����TIM1ȫ���ж�] */
  NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Enable the TIM2 gloabal Interrupt [����TIM2ȫ���ж�] */
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

/*******************************************************************************
 * Function Name  : USART_Configuration
 * Description    : Configures the USART1.
 *******************************************************************************/

void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStructure);

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
  USART_ClockInit(USART1, &USART_ClockInitStructure);

  /* Enables the USART1 transmit interrupt */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(USART1, USART_FLAG_TC);
  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
 * Function Name  : TIM_Configuration
 * Description    : Configures the TIMx.
 *******************************************************************************/
void TIM_Configuration()
{
  /* TIM1��ʱ������ */
  u16 CCR1_Val = 1800; // ����TIM1ͨ��1���ռ�ձ�:50%
  u16 CCR2_Val = 900;  // ����TIM1ͨ��2���ռ�ձ�:25%
  u16 CCR3_Val = 450;  // ����TIM1ͨ��3���ռ�ձ�:12.5%
  /*��λTIM1��ʱ��*/
  TIM_DeInit(TIM1);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 3600 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1,2,3 and 4 Configuration in PWM mode */
  /* ѡ��ʱ������Ƚ�ΪPWMģʽ2�������ϼ���ʱ����TIM1_CNT<TIM1_CCR1ʱ��
     ͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�������¼���ʱ����TIM1_CNT>TIM1_CCR1ʱ��
     ͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�����ѡ��PWMģʽ1�����෴�� */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  // ѡ������Ƚ�״̬���Լ���������Ƚ�״̬
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;

  // ����ͨ��1����ȽϼĴ���������ֵ����ռ�ձ�Ϊ50%
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;

  // ������Ժͻ���������Ե���Ч��ƽΪ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;

  // ѡ�����״̬�µķǹ���״̬��MOE=0ʱ������TIM1����ȽϿ���״̬��
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  // ����ͨ��2����ȽϼĴ���������ֵ����ռ�ձ�Ϊ25%
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  // ����ͨ��3����ȽϼĴ���������ֵ����ռ�ձ�Ϊ12.5%
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration */
  // ����������ģʽ�·ǹ���״̬
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;

  // д��������������Ϊ1������д��TIM1_BDTR�Ĵ�����DTG/BKE/BKP/AOEλ��TIM1_CR2�Ĵ�����OISx/OISxNλ��ϵͳ��λ��ֻ��д��һ��LOCKλ�������ݶ���ֱ����λ��
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 0x75;          // �������������ʱ��1.625us
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable; // ɲ������ʹ��

  // ����ɲ������ֹ�������ź����ԣ��ߵ�ƽ��Ч�������Զ����
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  /* Clear TIM1 update pending flag�����TIM1����жϱ�־ */
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  /* Enable TIM1 Update interrupt��TIM1����ж����� */
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  /* TIM1 enable counter������TIM1���� */
  TIM_Cmd(TIM1, ENABLE);
  /* TIM1 Main Output Enable�����PWM */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  /* TIM2��ʱ������ */
  /*��λTIM2��ʱ��*/
  TIM_DeInit(TIM2);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 35999;
  TIM_TimeBaseStructure.TIM_Prescaler = 1999; // ��1999+1������72MHz��Ϊ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* Clear TIM2 update pending flag�����TIM2����жϱ�־ */
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  /* Enable TIM2 Update interrupt��TIM2����ж����� */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  /* TIM2 enable counter������TIM2���� */
  TIM_Cmd(TIM2, ENABLE);
}

/*******************************************************************************
 * Function Name  : fputc
 * Description    : Retargets the C library printf function to the USART.
 *******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (unsigned char)ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
    ; // waiting here

  return ch;
}

/*******************************************************************************
 * Function Name  : Open_Init
 * Description    : Configures RCC,GPIO,NVIC
 *******************************************************************************/

void Open_Init(void)
{
  RCC_Configuration();
  GPIO_Configuration();
  USART_Configuration();
  //	NVIC_Configuration();
  //  TIM_Configuration();
}

/*******************************************************************************
 * Function Name  : delay_nus
 * Description    : delay n us
 *******************************************************************************/
void delay_nus(unsigned long n) //??n us: n>=6,??????6us
{
  unsigned long j;
  while (n--) // ????:8M;PLL:9;8M*9=72MHz
  {
    j = 8; // ????,???????
    while (j--)
      ;
  }
}

/*******************************************************************************
 * Function Name  : delay_nms
 * Description    : delay n ms
 *******************************************************************************/
void delay_nms(unsigned long n) //??n ms
{
  while (n--)        // ????:8M;PLL:9;8M*9=72MHz
    delay_nus(1100); // 1ms????
}

#endif
