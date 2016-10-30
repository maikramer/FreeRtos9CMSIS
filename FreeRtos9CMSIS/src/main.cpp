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
#include <Button.h>
#include <Serial.h>
#include <Timer.h>
#include <debug.h>
#include <Stepper.h>

//==================================================================================//
//	Objetos
//==================================================================================//

Led led(GPIOC, GPIO_Pin_13, State::LOW);
Led led2(GPIOB, GPIO_Pin_15, State::HIGH);

Button botao(GPIOB, GPIO_Pin_0);

Serial serial(USART1);

Timer timer1(TIM1);

DigitalOut stepOut(GPIOB, GPIO_Pin_7);
DigitalOut directionOut(GPIOB, GPIO_Pin_6);
DigitalOut enableOut(GPIOB, GPIO_Pin_8);

Stepper step1(stepOut, directionOut, enableOut);

//==================================================================================//
//	Funções
//==================================================================================//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void LEDBlinkTask(void *pvParameters) {
	while (1) {
		// Delay 1 segundo e então alterna estado
		vTaskDelay(1000 / portTICK_RATE_MS);
		led.toggle();
	}
	//Recomendação, se conseguir quebrar o loop, deleta a tarefa
	vTaskDelete(NULL);
}

static void escreveSerialTask(void *pvParameters) {
	while (1) {
		vTaskDelay(6000 / portTICK_RATE_MS);
		serial.print("AT+GMR\r\n");
	}
	//Recomendação, se conseguir quebrar o loop, deleta a tarefa
	vTaskDelete(NULL);
}

static void leSerialTask(void *pvParameters) {
	char cStr[128];
	size_t cStrLen = sizeof(cStr) / sizeof(char);
	while (1) {
		serial.gets(cStr, cStrLen);
		logMessage(cStr);
	}
	//Recomendação, se conseguir quebrar o loop, deleta a tarefa
	vTaskDelete(NULL);
}

static void leBotao(void *pvParameters) {
	while (1) {
		if (botao.getState()) {
			led2.set();
		} else
			led2.clear();
	}
}

static void enviaStepsTask(void *pvParameters) {
	while (1) {
		step1.step(1000, Speed::FASTEST);
		vTaskDelay(2000 / portTICK_RATE_MS);
		step1.step(-200, Speed::SLOWEST);
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}
int main(void) {
	//Recomendação para FreeRTOS, 4 bits for pre-emption priority 0 bits for subpriority
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);
	//Inicializa os Leds
	led.init();
	led2.init();
	step1.init();
	//Inicializa serial
	serial.init(115200);
	//Inicializa o botao
	botao.init();
	//Cria as Tarefas
	if (xTaskCreate(LEDBlinkTask, "Blink", 256, NULL, 2,
	NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		errorMessage("Nao foi possivel alocar a memoria necessaria para Blink");
	} else {
		logMessage("Tarefa Blink Adcionada\n");
	}

	if (xTaskCreate(escreveSerialTask, "EscreveSerial", 256, NULL, 2,
	NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		errorMessage(
				"Nao foi possivel alocar a memoria necessaria para EscreveSerial");
	} else {
		logMessage("Tarefa EscreveSerial Adcionada\n");
	}

	if (xTaskCreate(leSerialTask, "LeSerial", 256, NULL, 2,
	NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		errorMessage("Nao foi possivel alocar a memoria necessaria");
	} else {
		logMessage("Tarefa LeSerial Adcionada\n");
	}

	if (xTaskCreate(leBotao, "LeBotao", 256, NULL, 2,
	NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		errorMessage(
				"Nao foi possivel alocar a memoria necessaria para LeBotao");
	} else {
		logMessage("Tarefa LeBotao Adcionada\n");
	}

	if (xTaskCreate(enviaStepsTask, "EnviaSteps", 256, NULL, 3,
	NULL) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		errorMessage(
				"Nao foi possivel alocar a memoria necessaria para EnviaSteps");
	} else {
		logMessage("Tarefa EnviaSteps Adcionada\n");
	}

	vTaskStartScheduler();
	errorMessage("Nao foi possivel alocar memoria para a tarefa Idle");
	while (1)
		;
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,
		signed char *pcTaskName) {
	/* This function will get called if a task overflows its stack.   If the
	 parameters are corrupt then inspect pxCurrentTCB to find which was the
	 offending task. */

	trace_printf("Stack Overflow na tarefa %s", pcTaskName);

	for (;;)
		;
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
