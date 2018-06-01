/* Name: jprocess.c
 * Project: ARM fast JPEG-coder
 * Author: Dmitry Oparin aka Rst7/CBSIE (Modified by ILIASAM)
 * Creation Date: 1-Jun-2008
 * Copyright: (C)2008 by Rst7/CBSIE
 * License: GNU GPL v3 (see http://www.gnu.org/licenses/gpl-3.0.txt)
 */


//#include "stdafx.h"
//#include "params.h"
#include "jprocess.h"
#include <stdint.h>
//#include "adc_control.h"

#define LINE_WIDTH LINE_PIX_LENGTH

//COOCOX
//uint8_t outbytes0[20000] __attribute__ ((section(".ccm")));
//uint8_t outbytes1[20000] __attribute__ ((section(".ccm")));

#pragma location = ".ccmram"
uint8_t outbytes0[32000];
#pragma location = ".ccmram"
uint8_t outbytes1[32000];

uint8_t *write_pointer = (uint8_t*)&outbytes0;
uint8_t *read_pointer =  (uint8_t*)&outbytes1;

void switch_buffers(void)
{
	if (write_pointer == ((uint8_t*)&outbytes0))
	{
		write_pointer = outbytes1;
		read_pointer = outbytes0;
	}
	else
	{
		write_pointer = outbytes0;
		read_pointer = outbytes1;
	}
}

//extern volatile uint16_t frame_buffer[LINES_NUMBER][LINE_BUFFER_SIZE];//raw image


#define USE_OUTBYTES


#ifdef USE_OUTBYTES
//uint8_t outbytes[32768];
#else
//#define OUTSYM(VAR) {while(!UCSRA_UDRE);UDR=VAR;}
volatile UINT8 _SYMBOL;
#define OUTSYM(VAR) _SYMBOL=(VAR)
#endif


#define USE_MUL

#define abs(VAL) ((VAL)>=0?(VAL):-(VAL))
#define DIVIDE_(a,b)	((a)>=(b)?(a)/(b):0)
