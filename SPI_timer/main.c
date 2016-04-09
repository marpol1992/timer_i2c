/*
 * SPI_timer.c
 *
 * Created: 2016-03-23 20:37:20
 * Author : marek
 */ 

#include <avr/io.h>
#include "timer_DS3231.h"
#include "TWI_XMEGA.h"
#include "Oscylator_init.h"
#include "usart.h"

void USART_init();


TDATETIME datetime;
TTEMP temperature;



int main(void)
{
	 PORTD.DIR			=  PIN0_bm ;		
   //PORTC.DIR			=  PIN1_bm ;  
	 //PORTC.OUTCLR		=  PIN0_bm|PIN1_bm ;
	 PORTE.PIN0CTRL = PORT_OPC_WIREDAND_gc;
	 PORTE.PIN1CTRL = PORT_OPC_WIREDAND_gc;
	 //PORTCFG.MPCMASK	=  0x03;    // Configure several PINxCTRL registers.
    //PORTC.PIN0CTRL	=  (PORTC.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	////////////////przerwanie/////////////////
	PORTF.DIRCLR	=	PIN7_bm;
	PORTF.PIN7CTRL	=	PORT_ISC_RISING_gc;
	PORTF.INT0MASK  =   PIN7_bm;               // pin F1 ma generowaæ przerwania INT0
	PORTF.INTCTRL   =   PORT_INT0LVL_LO_gc;    //// poziom HI dla przerwania INT0
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; //0x07
	sei();
	init_osc32();
	TWI_Init(&TWIE,TWI_BAUD_FROM_FREQ(400000));
	DS2321_init();
	USART_init();
	//DS3231_set_time(20,22,0);
	//DS3231_set_date(16,4,7,4);
    while (1) 
    {   	
		
    }
}



void USART_init()
{
	PORTF_OUTSET=PIN3_bm;
	PORTF_DIRSET=PIN3_bm;
	
	PORTF_OUTCLR = PIN2_bm;
	PORTF_DIRCLR = PIN2_bm; //PC6 as RX
	

	// wlacz odbiornik oraz nadajnik USART
	USARTF0.CTRLB=USART_RXEN_bm | USART_TXEN_bm;
	// ramka 8 bitów, bez parzystoœci, 1 bit stopu
	USARTF0.CTRLC=USART_CHSIZE_8BIT_gc;
	usart_set_baudrate(&USARTF0, 115200, F_CPU);
	// odblokuj przerwania odbiornika
	USARTF0.CTRLA=USART_RXCINTLVL_HI_gc;
}
ISR(USARTF0_RXC_vect)
{
		uint8_t t		= USARTF0_DATA ;
		PORTD_OUTTGL	= PIN0_bm;
	
}
ISR(PORTF_INT0_vect) {
	PORTD_OUTTGL = PIN0_bm;
	DS3231_get_datetime(&datetime);
	show_time(&datetime);
	show_date(&datetime);
	DS3231_get_temp(&temperature);
	show_temperature(&temperature);
	
	USART_send(&USARTF0,"\n\r");
	USART_send(&USARTF0,"\n\r");
	
}