//=================================================================================//
//	Arquivo : Stepper.h
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
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

//==================================================================================//
//	Includes STM32
//==================================================================================//

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

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
#include <DigitalOut.h>
#include <Timer.h>

#ifndef STEPPER_H_
#define STEPPER_H_

class Stepper {
public:
	Stepper(DigitalOut& step, DigitalOut& direction, DigitalOut& enableOut,
			Timer& timer);
	virtual ~Stepper();
	void init();
	void update();
	void step(long ulSteps);
private:
	DigitalOut& _stepOut;
	DigitalOut& _directionOut;
	DigitalOut& _enableOut;
	Timer& _timer;
	long long _totalSteps;
};

#endif /* STEPPER_H_ */
