//=================================================================================//
//	Arquivo : Serial.h
//	Projeto : FreeRTOS9CMSIS
//	Autor : Maikeu Locatelli
//	Copyright : Locatelli Engenharia
//
//	Descricão: Objeto para manipulação de Serial no STM32F103
//=================================================================================//
//	This file is part of IntTeste
//	IntTeste is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//==================================================================================//
//	Includes
//==================================================================================//

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

//==================================================================================//
//	Includes STM32
//==================================================================================//

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>

//==================================================================================//
//	Includes FreeRTOS
//==================================================================================//

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//==================================================================================//
//	Includes Locais
//==================================================================================//

#include <debug.h>

//==================================================================================//
//	Definições de configuração
//==================================================================================//

// Numero de portas seriais sendo utilizadas
#define serNUM_COM_PORTS				( 2 )

// Máximo de caracteres que a QUEUE para Transmissão pode conter
#define serTX_QUEUE_LEN					( 100 )

// Máximo de caracteres que a QUEUE para Recepção pode conter
#define serRX_QUEUE_LEN					( 100 )

// Tempo por caracter que a função lSerialPutString() pode aguardar por espaço na QUEUE
#define serPUT_STRING_CHAR_DELAY		( 5 / portTICK_PERIOD_MS )

#ifndef SERIAL_H_
#define SERIAL_H_

class Serial {
public:
	Serial(USART_TypeDef * xPort);
	virtual ~Serial();
	BaseType_t init(unsigned long ulWantedBaud);
	BaseType_t print(const char * str);
	BaseType_t printf(const char *format, ...);
	BaseType_t gets(char * pcStr, const size_t maxLen);
	BaseType_t getChar(char *pcRxedChar, TickType_t xBlockTime);
	BaseType_t putChar(unsigned char cOutChar, TickType_t xBlockTime);
private:
	unsigned long _ulPort;
};

#endif /* SERIAL_H_ */
