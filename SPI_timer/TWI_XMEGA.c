/*
 * TWI_XMEGA.c
 *
 * Created: 2016-03-23 21:22:14
 *  Author: marek
 */ 
#include "Common/Common.h"

#if (ARCH == ARCH_XMEGA)

#define  __INCLUDE_FROM_TWI_C
#include "TWI_XMEGA.h"

uint8_t TWI_StartTransmission(TWI_t* const TWI,
const uint8_t SlaveAddress,
const uint8_t TimeoutMS)
{
	uint16_t TimeoutRemaining;

	TWI->MASTER.ADDR = SlaveAddress;

	TimeoutRemaining = (TimeoutMS * 100);
	while (TimeoutRemaining)
	{
		uint8_t status = TWI->MASTER.STATUS;

		if ((status & (TWI_MASTER_WIF_bm | TWI_MASTER_ARBLOST_bm)) == (TWI_MASTER_WIF_bm | TWI_MASTER_ARBLOST_bm))
		{
			TWI->MASTER.ADDR = SlaveAddress;
		}
		else if ((status & (TWI_MASTER_WIF_bm | TWI_MASTER_RXACK_bm)) == (TWI_MASTER_WIF_bm | TWI_MASTER_RXACK_bm))
		{
			TWI_StopTransmission(TWI);
			return TWI_ERROR_SlaveResponseTimeout;
		}
		else if (status & (TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm))
		{
			return TWI_ERROR_NoError;
		}

		_delay_us(10);
		TimeoutRemaining--;
	}

	if (!(TimeoutRemaining)) {
		if (TWI->MASTER.STATUS & TWI_MASTER_CLKHOLD_bm) {
			TWI_StopTransmission(TWI);
		}
	}

	return TWI_ERROR_BusCaptureTimeout;
}

bool TWI_SendByte(TWI_t* const TWI,
const uint8_t Byte)
{
	TWI->MASTER.DATA = Byte;

	while (!(TWI->MASTER.STATUS & TWI_MASTER_WIF_bm));

	return (TWI->MASTER.STATUS & TWI_MASTER_WIF_bm) && !(TWI->MASTER.STATUS & TWI_MASTER_RXACK_bm);
}

bool TWI_ReceiveByte(TWI_t* const TWI,
uint8_t* const Byte,
const bool LastByte)
{
	if ((TWI->MASTER.STATUS & (TWI_MASTER_BUSERR_bm | TWI_MASTER_ARBLOST_bm)) == (TWI_MASTER_BUSERR_bm | TWI_MASTER_ARBLOST_bm)) {
		return false;
	}

	while (!(TWI->MASTER.STATUS & TWI_MASTER_RIF_bm));

	*Byte = TWI->MASTER.DATA;

	if (LastByte)
	TWI->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
	else
	TWI->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;

	return true;
}

uint8_t TWI_ReadPacket(TWI_t* const TWI,
const uint8_t SlaveAddress,
const uint8_t TimeoutMS,
const uint8_t InternalAddress,
uint8_t InternalAddressLen,
uint8_t* Buffer,
uint8_t Length)
{
	uint8_t ErrorCode;

	if ((ErrorCode = TWI_StartTransmission(TWI, (SlaveAddress & TWI_DEVICE_ADDRESS_MASK) | TWI_ADDRESS_WRITE,
	TimeoutMS)) == TWI_ERROR_NoError)
	{
		while (InternalAddressLen--)
		{
			if (!(TWI_SendByte(TWI, (InternalAddress))))
			{
				ErrorCode = TWI_ERROR_SlaveNAK;
				break;
			}
		}

		if ((ErrorCode = TWI_StartTransmission(TWI, (SlaveAddress & TWI_DEVICE_ADDRESS_MASK) | TWI_ADDRESS_READ,
		TimeoutMS)) == TWI_ERROR_NoError)
		{
			while (Length--)
			{
				if (!(TWI_ReceiveByte(TWI, Buffer++, (Length == 0))))
				{
					ErrorCode = TWI_ERROR_SlaveNAK;
					break;
				}
			}
		}

		TWI_StopTransmission(TWI);
	}

	return ErrorCode;
}

uint8_t TWI_WritePacket(TWI_t* const twi,
const uint8_t SlaveAddress,
const uint8_t TimeoutMS,
const uint8_t InternalAddress,
uint8_t InternalAddressLen,
const uint8_t* Buffer,
uint8_t Length)
{
	uint8_t ErrorCode;

	if ((ErrorCode = TWI_StartTransmission(twi, (SlaveAddress & TWI_DEVICE_ADDRESS_MASK) | TWI_ADDRESS_WRITE,
	TimeoutMS)) == TWI_ERROR_NoError)
	{
		while (InternalAddressLen--)
		{
			if (!(TWI_SendByte(twi, (InternalAddress))))
			{
				ErrorCode = TWI_ERROR_SlaveNAK;
				break;
			}
		}

		while (Length--)
		{
			if (!(TWI_SendByte(twi, *(Buffer++))))
			{
				ErrorCode = TWI_ERROR_SlaveNAK;
				break;
			}
		}

		TWI_StopTransmission(twi);
	}

	return ErrorCode;
}

#endif
