//=================================================================================//
//	Arquivo : Main.cpp
//	Projeto : FreeRtos9CMSIS
//	Autor : Maikeu Locatelli
//	Copyright : Locatelli Engenharia
//
//	Descricão: Configuração basica do FreeRTOS 9 com CMSIS no STM32F103
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

//==================================================================================//
//	Includes FreeRTOS
//==================================================================================//

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//==================================================================================//
//	Includes Locais
//==================================================================================//

#include <Led.h>
#include <Serial.h>
#include <debug.h>

//==================================================================================//
//	Objetos
//==================================================================================//

Led led(GPIOC, GPIO_Pin_13, State::LOW);
Serial serial;

//==================================================================================//
//	Funções
//==================================================================================//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void LEDBlinkTask(void *pvParameters) {
	while (1) {
		// Delay and turn on
		vTaskDelay(1000 / portTICK_RATE_MS);
		led.toggle();
	}
}

static void escreveSerialTask(void *pvParameters) {
	while (1) {
		vTaskDelay(5000 / portTICK_RATE_MS);
		serial.print("AT\r\n");
	}
}

static void leSerialTask(void *pvParameters) {
	while (1) {
		while (!serial.available())
			;
		logMessage(serial.read());
	}
}

int main(void) {
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
	led.init();
	xTaskCreate(LEDBlinkTask, "Blink", 256, NULL, 2, NULL);
	logMessage("Tarefa Blink Adcionada\n");
	xTaskCreate(escreveSerialTask, "EscreveSerial", 1024, NULL, 2, NULL);
	logMessage("Tarefa Escreve Serial Adcionada\n");
	xTaskCreate(leSerialTask, "LeSerial", 1024, NULL, 2, NULL);
	logMessage("Tarefa Le Serial Adcionada\n");
	vTaskStartScheduler();
	while (1)
		;
}

#pragma GCC diagnostic pop

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
		signed char *pcTaskName) {
	/* This function will get called if a task overflows its stack.   If the
	 parameters are corrupt then inspect pxCurrentTCB to find which was the
	 offending task. */

	trace_printf("Stack Overflow na tarefa %s", pcTaskName);

	for (;;)
		;
}

// ----------------------------------------------------------------------------
