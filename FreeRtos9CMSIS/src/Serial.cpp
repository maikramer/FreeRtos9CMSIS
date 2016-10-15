//=================================================================================//
//	Arquivo : Serial.cpp
//	Projeto : IntTeste
//	Autor : Maikeu Locatelli
//	Copyright : Locatelli Engenharia
//
//	Descricão: 
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
#include "Serial.h"

Serial::Serial() {
	init();
}

Serial::~Serial() {
	// TODO Auto-generated destructor stub
}

void Serial::init() {
	/* USART configuration structure for USART1 */
	USART_InitTypeDef usart1_init_struct;
	/* Bit configuration structure for GPIOA PIN9 and PIN10 */
	GPIO_InitTypeDef gpioa_init_struct;

	/* Enalbe clock for USART1, AFIO and GPIOA */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO |
	RCC_APB2Periph_GPIOA, ENABLE);

	/* GPIOA PIN9 alternative function Tx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpioa_init_struct);
	/* GPIOA PIN10 alternative function Rx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpioa_init_struct);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
	/* Baud rate 9600, 8-bit data, One stop bit
	 * No parity, Do both Rx and Tx, No HW flow control
	 */
	usart1_init_struct.USART_BaudRate = 115200;
	usart1_init_struct.USART_WordLength = USART_WordLength_8b;
	usart1_init_struct.USART_StopBits = USART_StopBits_1;
	usart1_init_struct.USART_Parity = USART_Parity_No;
	usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart1_init_struct.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	/* Configure USART1 */
	USART_Init(USART1, &usart1_init_struct);

#ifdef USE_IT_TO_READ
	/* Enable RXNE interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable USART1 global interrupt */
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0x1);
#endif
}

void Serial::print(const char * str) {
	while (*str != 0) {
		USART_SendData(USART1, *str);
		str++;
		// Aguarda até o dado ter sido enviado
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			;
	}
}
void Serial::printf(const char *format, ...) {
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 256, format, args);
	print(buffer);
	va_end(args);
}

bool Serial::available(void){
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
		return true;
	}
	else {
		return false;
	}
}

std::string Serial::read(void) {
	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
		std::string buffer;
		int timeout = 500;
		while (timeout > 0) {
			if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
				buffer += (char) USART_ReceiveData(USART1);
				if (timeout < 50000) {
					timeout += 100;
				}
			} else {
				timeout--;
			}
		}
		return (buffer);
	}
	else return ('\0');
}

#ifdef USE_IT_TO_READ
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"

extern "C" void USART1_IRQHandler(void) {
	// Dado Recebido
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		serialBuffer = "";
		uint16_t timeout = 500;
		while (timeout > 0) {
			if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) {
				serialBuffer += (char) USART_ReceiveData(USART1);
				if (timeout < 50000) {
					timeout = timeout + 100;
				}
			}
			else {
				timeout--;
			}
		}
		logMessage(serialBuffer);
	}
}

#pragma GCC diagnostic pop
#endif
