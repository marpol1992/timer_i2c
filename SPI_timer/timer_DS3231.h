/*
 * timer_DS3231.h
 *
 * Created: 2016-04-08 22:58:13
 *  Author: marek
 */ 


#ifndef TIMER_DS3231_H_
#define TIMER_DS3231_H_

#include <stdint.h>
/*Adres timera TWI */
#define DS3231_ADDR 0xD0
#define internal_adress_1HZ		 0x0e
#define internal_adress_temp	 0x11
#define time_adress				 0x00
#define date_adress				 0x03


typedef union {
	uint8_t bytes[7];
	struct {
		uint8_t ss;
		uint8_t mm;
		uint8_t hh;
		uint8_t dayofwek;
		uint8_t day;
		uint8_t month;
		uint8_t year;
	};
} TDATETIME;


typedef struct {
	int8_t cel;
	uint8_t fract;
} TTEMP;



void DS2321_init(void);
uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void DS3231_get_datetime( TDATETIME * dt );
void show_time( TDATETIME * dt );
void show_date( TDATETIME * dt );
void DS3231_set_time( uint8_t hh, uint8_t mm, uint8_t ss );
void DS3231_set_date( uint8_t year, uint8_t month, uint8_t day, uint8_t dayofweek );
void DS3231_get_temp( TTEMP * tmp );
void show_temperature( TTEMP * tmp );



#endif /* TIMER_DS3231_H_ */