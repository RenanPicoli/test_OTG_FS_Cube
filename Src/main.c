/*
******************************************************************************
File:     main.cpp
Info:     Generated by Atollic TrueSTUDIO(R) 9.0.0   2018-06-05

The MIT License (MIT)
Copyright (c) 2018 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f7xx.h"
//#include "stm32f4_discovery.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

#ifdef __cplusplus
extern "C"{
#endif
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_video_core.h"
#include "math.h"
#include "jprocess.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
#ifdef __cplusplus
}
#endif
__IO uint8_t UserButtonPressed = 0;

uint8_t raw_image[IMG_HEIGHT][IMG_WIDTH];

uint16_t last_jpeg_frame_size = 0;
volatile uint8_t jpeg_encode_done = 0;//1 - encode stopped flag
volatile uint8_t jpeg_encode_enabled = 1;//1 - capture and encoding enabled

double circle_x = 0;
double circle_y = 0;
double angle = 0;
uint8_t color;

void draw_circle(int Hcenter, int Vcenter, int radius,uint8_t color);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
void Fail_Handler(void);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  int i = 0;

  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  /* TODO - Add your application code here */

  //configura��o do GPIOC (leds)
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_2 ;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  //Os LEDS dessa placa s�o ATIVADOS EM N�VEL BAIXO
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);//red off
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);//green off
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);//blue off

  USBD_Init(&USB_OTG_dev,
              USB_OTG_FS_CORE_ID,
              &USR_desc,
              &VIDEO_cb,
              &USR_cb);

  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);//red on
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);//green on
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);//blue on

  /* Infinite loop */
  while (1)
  {
	i++;
	  if (jpeg_encode_enabled == 1)
	    {
	      jpeg_encode_enabled = 0;
	      jpeg_encode_done = 0;
	      last_jpeg_frame_size = jprocess();//Data source (image) for jpeg encoder can be switched in "jprocess" function.

	      circle_x = 160 + sin(angle)*60;
	      circle_y = 120 + cos(angle)*60;
	      angle+= 0.05;
	      color+= 10;
	      draw_circle((int)circle_x, (int)circle_y, 15, color);

	      jpeg_encode_done = 1;//encoding ended
	    }

  }
}


void draw_circle(int Hcenter, int Vcenter, int radius,uint8_t color)
{
  int x = radius;
  int y = 0;
  int xChange = 1 - (radius << 1);
  int yChange = 0;
  int radiusError = 0;
  int i;
  //int p = 3 - 2 * radius;

  while (x >= y)
  {
    for (i = Hcenter - x; i <= Hcenter + x; i++)
    {
      raw_image[Vcenter + y][i] = color;
      raw_image[Vcenter - y][i] = color;
    }
    for (i = Hcenter - y; i <= Hcenter + y; i++)
    {
      raw_image[Vcenter + x][i] = color;
      raw_image[Vcenter - x][i] = color;
    }

    y++;
    radiusError += yChange;
    yChange += 2;
    if (((radiusError << 1) + xChange) > 0)
    {
      x--;
      radiusError += xChange;
      xChange += 2;
    }
  }
}



void delay_ms(uint32_t ms)
{
        volatile uint32_t nCount;
/*        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);*/
        nCount=(HAL_RCC_GetHCLKFreq()/10000)*ms;
        for (; nCount!=0; nCount--);
}
