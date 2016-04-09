/*
 * Oscylator_init.c
 *
 * Created: 2015-11-03 21:56:30
 *  Author: marek
 */ 
#include "Oscylator_init.h"

void init_osc32(void)
{
	
	OSC.CTRL     =    OSC_RC32MEN_bm;       // w³¹czenie oscylatora 32MHz
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // czekanie na ustabilizowanie siê generatora
	CPU_CCP      =    CCP_IOREG_gc;         // odblokowanie zmiany Ÿród³a sygna³u
	CLK.CTRL     =    CLK_SCLKSEL_RC32M_gc; // zmiana Ÿród³a sygna³u na RC 32MHz
	//PORTC_REMAP |= 0x16; //See page 152 in datasheet, remaps the USART0
}