//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <Led.h>
#include <Serial.h>
#include <debug.h>
// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.

Led led(GPIOC, GPIO_Pin_13, State::LOW);
Serial serial;

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
