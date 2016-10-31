//=================================================================================//
//	Arquivo : ESP8266.h
//	Projeto : FreeRtos9CMSIS
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
#include <Serial.h>


#ifndef ESP8266_H_
#define ESP8266_H_

#define MAX_ESP_BUFFER 	128

class ESP8266 {
public:
	ESP8266(Serial& serial);
	virtual ~ESP8266();
	BaseType_t begin(void);
	BaseType_t isConnected();
	BaseType_t connect();
private:
	bool _waitForOk(void);
	Serial& _serial;
	char _cRxBuffer[MAX_ESP_BUFFER];
};

#endif /* ESP8266_H_ */
