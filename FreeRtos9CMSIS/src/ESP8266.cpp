//=================================================================================//
//	Arquivo : ESP8266.cpp
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

#include "ESP8266.h"

//Configurações de conexão
const char * WIFI_RSID = "Netvirtua apt 1004";
const char * WIFI_PASSWORD = "27857";

ESP8266::ESP8266(Serial& serial) :
		_serial(serial) {
}

ESP8266::~ESP8266() {
	// TODO Auto-generated destructor stub
}

BaseType_t ESP8266::begin() {
	BaseType_t xReturn = pdFAIL;
	if (cmd("AT", "Verifica Status", 3) == pdFAIL) {
		return xReturn;
	}
	// Desliga o Echo
	if (cmd("ATE0", "Desliga Echo", 3) == pdPASS) {
		xReturn = pdPASS;
	}
	logMessage("ESP8266 Inicializado");
	return xReturn;
}

BaseType_t ESP8266::isConnected() {
	BaseType_t xReturn = pdTRUE;
	_serial.print("AT+CIPSTATUS\r\n");
	if (cmd("AT+CIPSTATUS", "Verifica Status da Coneccao", 3)) {
		if (strstr(_cRxBuffer, "STATUS:2") == nullptr) {
			xReturn = pdFALSE;
		}
	}
	return xReturn;
}

BaseType_t ESP8266::connect() {
	BaseType_t xReturn = pdFAIL;

	//Verifica se já Conectado
	if (isConnected() == pdTRUE) {
		logMessage("Wifi Conectado");
		return (pdPASS);
	}

	//Seta Modo para Station e ativa DHCP
	cmd("AT+CWDHCP_DEF=1,1", "Seta modo para Station e ativa DHCP", 3);
	_serial.printf("AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", WIFI_RSID, WIFI_PASSWORD);

	//Verifica se conectou
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "CONNECTED") == nullptr) {
		if (strstr(_cRxBuffer, "DISCONNECT") != nullptr) {
			_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
			if (strstr(_cRxBuffer, "CONNECTED") == nullptr) {
				logMessage("Falha ao conectar");
				return xReturn;
			}
		}
	}

	// Verifica se conseguiu IP
	logMessage("Conectado, adquirindo endereco de IP");
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "GOT") == nullptr) {
		logMessage("Falha ao conseguir endereco de IP");
		return xReturn;
	}
	logMessage("Adquirido endereco de IP");

	//Verifica se comando Finalizado com sucesso
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "OK") == nullptr) {
		return xReturn;
	}

	//Verifica se conectado
	if (isConnected() == pdTRUE) {
		xReturn = pdPASS;
		logMessage("Conectado!!");
	}

	return xReturn;
}

BaseType_t ESP8266::cmd(const char * command, const char *logMsg,
		unsigned char retries) {
	BaseType_t xReturn = pdFAIL;
	static char logMsgF[100];
	while (retries > 0) {
		sprintf(logMsgF, "%s %d Tentativas Restantes", logMsg, retries);
		logMessage(logMsgF);
		_serial.printf("%s\r\n", command);
		_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
		if (strstr(_cRxBuffer, "OK") != nullptr) {
			return pdPASS;
		}
		retries--;
		if (retries != 0) {
			vTaskDelay(500 / portTICK_RATE_MS);
		}
	}
	logMessage("Falha ao enviar Comando");
	return xReturn;
}

BaseType_t ESP8266::startTelnet(void) {
	BaseType_t xReturn = pdFAIL;
	static char ip[30];
	if (cmd("AT+CIPMUX=1", "Ativa multiplas conexoes", 3) == pdFAIL) {
		return xReturn;
	}
	if (cmd("AT+CIPSERVER=1,2222", "Estabelece servidor na porta 2222",
			3)== pdFAIL) {
		return xReturn;
	}
	//Exibe endereço de ip
	if (cmd("AT+CIFSR", "Endereco de IP Atual", 3) == pdFAIL) {
		return xReturn;
	}
	char * pcBegin = strchr(_cRxBuffer, '\"') + 1;
	char * pcEnd = strchr(pcBegin + 1, '\"');
	strncpy(ip, pcBegin, pcEnd - pcBegin);
	trace_printf("LOG : Conectado no endereco %s", ip);
	return (pdPASS);
}

BaseType_t ESP8266::sendTelnet(const char * pcStr) {
	BaseType_t xReturn = pdFAIL;
	size_t sSize = strlen(pcStr);
	_serial.printf("AT+CIPSEND=0,%d\r\n", sSize);
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strchr(_cRxBuffer, '>') == nullptr) {
		return xReturn;
	}
	_serial.print(pcStr);
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "OK") != nullptr) {
		return pdPASS;
	}
	return xReturn;
}
