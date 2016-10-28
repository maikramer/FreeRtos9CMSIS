//=================================================================================//
//	Arquivo : Timer.h
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//==================================================================================//
//	Includes STM32
//==================================================================================//

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>

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

#define MAX_FUNCTIONS 5

#ifndef TIMER_H_
#define TIMER_H_

typedef void (*pFunction)();

class Timer {
public:
	Timer(TIM_TypeDef * timer);
	virtual ~Timer();
	void init();
	void setPeriod(uint16_t periodOverflow);
	void configIT(uint16_t periodOverflow, pFunction func);
private:
	TIM_TypeDef * _timer;
	IRQn _timerIRQn;
	bool _init;
};

#endif /* TIMER_H_ */
