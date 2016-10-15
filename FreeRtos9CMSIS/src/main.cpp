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

int main(void) {
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	led.init();
	xTaskCreate(LEDBlinkTask, "Blink", 256, NULL, 2, NULL);
	vTaskStartScheduler();
	while (1)
		;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
