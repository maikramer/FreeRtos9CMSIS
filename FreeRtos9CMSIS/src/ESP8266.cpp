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
const char * WIFI_PASSWORD = "27857"s;

ESP8266::ESP8266(Serial& serial) :
		_serial(serial) {
}

ESP8266::~ESP8266() {
	// TODO Auto-generated destructor stub
}

BaseType_t ESP8266::begin() {
	BaseType_t xReturn = pdFAIL;
	_serial.print("AT\r\n");
	if (!_waitForOk()) {
		return xReturn;
	}
	// Desliga o Echo
	_serial.print("ATE0\r\n");
	if (_waitForOk()) {
		logMessage(" WIFI inicializado");
		xReturn = pdPASS;
	}
	return xReturn;
}

BaseType_t ESP8266::isConnected() {
	BaseType_t xReturn = pdFAIL;
	_serial.print("AT+CIPSTATUS\r\n");
	if (_waitForOk()) {
		if (strstr(_cRxBuffer, "STATUS:2")) {
			xReturn = pdPASS;
		}
	}
	return xReturn;
}

BaseType_t ESP8266::connect() {
	BaseType_t xReturn = pdFAIL;
	if (isConnected()){
		logMessage("Wifi Conectado");
		return (pdPASS);
	}
	_serial.print("AT+CWDHCP_DEF=1,1\r\n");
	if (!_waitForOk()) {
		return xReturn;
	}
	logMessage("Modo setado para Station e DHCP ativado");
	_serial.printf("AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", WIFI_RSID, WIFI_PASSWORD);
	// Limpa o Buffer
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
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
	logMessage("Conectado, adquirindo endereco de IP");
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "GOT") == nullptr) {
		logMessage("Falha ao conseguir endereco de IP");
		return xReturn;
	}
	logMessage("Adquirido endereco de IP");

	if (!_waitForOk()) {
		return xReturn;
	}
	if (isConnected() == pdPASS) {
		xReturn = pdPASS;
		logMessage("Conectado!!");
	}
	return xReturn;
}

bool ESP8266::_waitForOk(void) {
	_serial.gets(_cRxBuffer, MAX_ESP_BUFFER);
	if (strstr(_cRxBuffer, "OK") != nullptr) {
		return true;
	}
	return false;
}

