//=================================================================================//
//	Arquivo : Serial.cpp
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

#include <Serial.h>

//==================================================================================//
//	Váriaveis globais
//==================================================================================//

// Vetor de USARTs disponíveis
static USART_TypeDef * const xUARTS[serNUM_COM_PORTS] = {
		((USART_TypeDef *) USART1), ((USART_TypeDef *) USART2) };

// QUEUE usada para conter os caracteres aguardando para serem transmitidos - Um por porta
static QueueHandle_t xCharsForTx[serNUM_COM_PORTS] = { 0 };

// QUEUE usada para conter os caracteres recebidos - Um por porta
static QueueHandle_t xRxedChars[serNUM_COM_PORTS] = { 0 };

//==================================================================================//
//	Definição dos Métodos
//==================================================================================//

// Cria o objeto Serial especificando a porta utilizada
// Valores possíveis : USART1 e USART2
Serial::Serial(USART_TypeDef * xPort) {
	if (xPort == USART1) {
		_ulPort = 0;
	} else if (xPort == USART2) {
		_ulPort = 1;
	}

}

//Destrutor
Serial::~Serial() {
	//Somente um placehold
}

//Inicializa a serial com a velocidade selecionada
//Ex: 9600, 115200 ...
BaseType_t Serial::init(unsigned long ulWantedBaud) {
	BaseType_t xReturn = pdFAIL;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	if (_ulPort < serNUM_COM_PORTS) {
		/* The common (not port dependent) part of the initialisation. */
		USART_InitStructure.USART_BaudRate = ulWantedBaud;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
		configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		/* Init the buffer structures with the buffer for the COM port being
		 initialised, and perform any non-common initialisation necessary.  This
		 does not check to see if the COM port has already been initialised. */
		if (_ulPort == 0) {
			/* Create the queue of chars that are waiting to be sent to COM0. */
			xCharsForTx[0] = xQueueCreate(serTX_QUEUE_LEN, sizeof(char));

			/* Create the queue used to hold characters received from COM0. */
			xRxedChars[0] = xQueueCreate(serRX_QUEUE_LEN, sizeof(char));

			/* Enable COM0 clock - the ST libraries start numbering from UART1. */
			RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

			/* Configure USART1 Rx (PA10) as input floating */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init( GPIOA, &GPIO_InitStructure);

			/* Configure USART1 Tx (PA9) as alternate function push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init( GPIOA, &GPIO_InitStructure);

			USART_Init( USART1, &USART_InitStructure);
			USART_ITConfig( USART1, USART_IT_RXNE, ENABLE);

			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
			NVIC_Init(&NVIC_InitStructure);

			USART_DMACmd( USART1, ( USART_DMAReq_Tx | USART_DMAReq_Rx), ENABLE);
			USART_Cmd( USART1, ENABLE);

			/* Everything is ok. */
			xReturn = pdPASS;
		} else if (_ulPort == 1) {
			/* Create the queue of chars that are waiting to be sent to COM1. */
			xCharsForTx[1] = xQueueCreate(serTX_QUEUE_LEN, sizeof(char));

			/* Create the queue used to hold characters received from COM0. */
			xRxedChars[1] = xQueueCreate(serRX_QUEUE_LEN, sizeof(char));

			/* Enable COM0 clock - the ST libraries start numbering from 1. */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

			/* Configure USART2 Rx (PA3) as input floating */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init( GPIOA, &GPIO_InitStructure);

			/* Configure USART2 Tx (PA2) as alternate function push-pull */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init( GPIOA, &GPIO_InitStructure);

			USART_Init( USART2, &USART_InitStructure);
			USART_ITConfig( USART2, USART_IT_RXNE, ENABLE);

			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			NVIC_Init(&NVIC_InitStructure);

			USART_DMACmd( USART2, ( USART_DMAReq_Tx | USART_DMAReq_Rx), ENABLE);
			USART_Cmd( USART2, ENABLE);

			/* Everything is ok. */
			xReturn = pdPASS;
		} else {
			/* Nothing to do unless more than two ports are supported. */
		}
	}

	return xReturn;
}

//Imprime uma string sem formatação.
BaseType_t Serial::print(const char * str) {
	BaseType_t xReturn;
	unsigned long ulStringLength = strlen(str);

	if (_ulPort < serNUM_COM_PORTS) {
		xReturn = pdPASS;

		for (unsigned long ul = 0; ul < ulStringLength; ul++) {
			if ( xQueueSend(xCharsForTx[_ulPort], str+ul,
					serPUT_STRING_CHAR_DELAY) != pdPASS) {
				/* Cannot fit any more in the queue.  Try turning the Tx on to
				 clear some space. */
				USART_ITConfig(xUARTS[_ulPort], USART_IT_TXE, ENABLE);
				vTaskDelay( serPUT_STRING_CHAR_DELAY);

				/* Go back and try again. */
				continue;
			}
		}

		USART_ITConfig(xUARTS[_ulPort], USART_IT_TXE, ENABLE);
	} else {
		xReturn = pdFAIL;
	}

	return xReturn;
}

BaseType_t Serial::printf(const char *format, ...) {
	char buffer[128];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 128, format, args);
	va_end(args);
	return (print(buffer));
}

BaseType_t Serial::gets(char * pcStr, const size_t maxLen) {
	BaseType_t xReturn = pdFAIL;
	unsigned long ulCount = 0;
	if (this->getChar(pcStr, portMAX_DELAY) == pdFAIL) {
		return xReturn;
	}
	for (;;) {
		pcStr++;
		if ((++ulCount) == (maxLen - 1)) {
			*pcStr = '\0';
			xReturn = pdFAIL;
		}
		if (this->getChar(pcStr, 5 / portTICK_RATE_MS)) {
			continue;
		} else {
			*pcStr = '\0';
			xReturn = pdPASS;
			break;
		}
	}
	return xReturn;
}

BaseType_t Serial::getChar(char *pcRxedChar, TickType_t xBlockTime) {
	BaseType_t xReturn = pdFAIL;
	if (_ulPort < serNUM_COM_PORTS) {
		if ( xQueueReceive(xRxedChars[_ulPort], pcRxedChar,
				xBlockTime) == pdPASS) {
			xReturn = pdPASS;
		}
	}
	return xReturn;
}

BaseType_t Serial::putChar(unsigned char cOutChar, TickType_t xBlockTime) {
	BaseType_t xReturn;
	if ( xQueueSend( xCharsForTx[ _ulPort ], &cOutChar, xBlockTime ) == pdPASS) {
		xReturn = pdPASS;
		USART_ITConfig(xUARTS[_ulPort], USART_IT_TXE, ENABLE);
	} else {
		xReturn = pdFAIL;
	}

	return xReturn;
}

extern "C" void USART1_IRQHandler(void) {
	long xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	if (USART_GetITStatus( USART1, USART_IT_TXE) == SET) {
		/* The interrupt was caused by the THR becoming empty.  Are there any
		 more characters to transmit? */
		if (xQueueReceiveFromISR(xCharsForTx[0], &cChar,
				&xHigherPriorityTaskWoken)) {
			/* A character was retrieved from the buffer so can be sent to the
			 THR now. */
			USART_SendData( USART1, cChar);
		} else {
			USART_ITConfig( USART1, USART_IT_TXE, DISABLE);
		}
	}

	if (USART_GetITStatus( USART1, USART_IT_RXNE) == SET) {
		cChar = USART_ReceiveData( USART1);
		xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
/*-----------------------------------------------------------*/

extern "C" void USART2_IRQHandler(void) {
	long xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	if (USART_GetITStatus( USART2, USART_IT_TXE) == SET) {
		/* The interrupt was caused by the THR becoming empty.  Are there any
		 more characters to transmit? */
		if (xQueueReceiveFromISR(xCharsForTx[1], &cChar,
				&xHigherPriorityTaskWoken)) {
			/* A character was retrieved from the buffer so can be sent to the
			 THR now. */
			USART_SendData( USART2, cChar);
		} else {
			USART_ITConfig( USART2, USART_IT_TXE, DISABLE);
		}
	}

	if (USART_GetITStatus( USART2, USART_IT_RXNE) == SET) {
		cChar = USART_ReceiveData( USART2);
		xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
