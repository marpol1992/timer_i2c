/*
 * timer_DS3231.c
 *
 * Created: 2016-04-08 23:00:21
 *  Author: marek
 */ 
#include "timer_DS3231.h"
#include "usart.h"
#include "TWI_XMEGA.h"

char days[7][4] = {
	"pon", "wto", "sro", "czw", "pia", "sob", "nie"
};

// konwersja liczby dziesiêtnej na BCD
uint8_t dec2bcd(uint8_t dec) {
	return ((dec / 10)<<4) | (dec % 10);
}

// konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd) {
	return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}

void DS2321_init(void){
	uint8_t ctrl = 0;
	TWI_WritePacket(&TWIE,DS3231_ADDR,10,internal_adress_1HZ,1,&ctrl,1);
	
}

void DS3231_get_datetime( TDATETIME * dt ) {
	uint8_t i;
	uint8_t buf[7];
	TWI_ReadPacket(&TWIE,DS3231_ADDR,10,time_adress,1,buf,7);
	//TWI_read_buf( DS3231_ADDR, 0x00, 7, buf );
	for( i=0; i<7; i++ ) dt->bytes[i] = bcd2dec( buf[i] );
	//dt->ss = buf[0];
	//dt->mm = buf[1];
	//dt->hh = buf[2];
	//dt->dayofwek = buf[3];
	//dt->day = buf[4];
	//dt->month = buf[5];
	//dt->year = buf[6];
	
}

void DS3231_set_time( uint8_t hh, uint8_t mm, uint8_t ss ) {
	uint8_t buf[3];
	buf[0]=dec2bcd(ss);
	buf[1]=dec2bcd(mm);
	buf[2]=dec2bcd(hh);
	//TWI_write_buf( DS3231_ADDR, 0x00, 3, buf );
	TWI_WritePacket(&TWIE,DS3231_ADDR,10,time_adress,1,buf,3);
}

void DS3231_set_date( uint8_t year, uint8_t month, uint8_t day, uint8_t dayofweek ) {
	uint8_t buf[4];
	buf[0]=dayofweek;
	buf[1]=dec2bcd(day);
	buf[2]=dec2bcd(month);
	buf[3]=dec2bcd(year);
	//TWI_write_buf( DS3231_ADDR, 0x03, 4, buf );
	TWI_WritePacket(&TWIE,DS3231_ADDR,10,date_adress,1,buf,4);
}

void DS3231_get_temp( TTEMP * tmp ) {
	uint8_t buf[2];
	TWI_ReadPacket(&TWIE,DS3231_ADDR,10,internal_adress_temp,1,buf,2);
	//TWI_read_buf( DS3231_ADDR, 0x11, 2, buf );
	tmp->cel = buf[0] ;
	tmp->fract = buf[1]>>6;

	//TWI_read_buf( DS3231_ADDR, 0x0e, 1, buf );
	TWI_ReadPacket(&TWIE,DS3231_ADDR,10,internal_adress_1HZ,1,buf,1);
	buf[0] |= (1<<5);
	//TWI_write_buf( DS3231_ADDR, 0x0e, 1, buf );
	TWI_WritePacket(&TWIE,DS3231_ADDR,10,internal_adress_1HZ,1,buf,1);
}

void show_temperature( TTEMP * tmp ) {
	uint8_t fr[4] = {0,2,5,7};

	USART_send(&USARTF0,"\n\rTemperatura:\n\r");
	USART_sendint(&USARTF0,tmp->cel,10);
	USART_send(&USARTF0,".");
	if( tmp->cel >= 0 ) USART_sendint(&USARTF0,fr[ tmp->fract ] ,10);
	else USART_sendint(&USARTF0,fr[ 3 - tmp->fract] ,10);
	USART_send(&USARTF0,"*C");

}
void show_date( TDATETIME * dt ) {
	USART_send(&USARTF0,"\n\r");
	USART_send(&USARTF0,"20");
	if( dt->year < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->year,10);
	USART_send(&USARTF0,"-");
	if( dt->month < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->month,10);
	USART_send(&USARTF0,"-");
	if( dt->day < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->day,10);
	USART_send(&USARTF0,"  ");
	USART_send(&USARTF0,days[ dt->dayofwek - 1 ]);
	USART_send(&USARTF0,"\n\r");
}
void show_time( TDATETIME * dt ) {
	USART_send(&USARTF0,"Jest godzina:\n\r");
	if( dt->hh < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->hh,10);
	USART_send(&USARTF0,":");
	if( dt->mm < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->mm,10);
	USART_send(&USARTF0,":");
	if( dt->ss < 10 ) USART_send(&USARTF0,"0");
	USART_sendint(&USARTF0,dt->ss,10);
}