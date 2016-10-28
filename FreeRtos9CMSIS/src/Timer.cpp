//=================================================================================//
//	Arquivo : Timer.cpp
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
#include <Timer.h>

pFunction execTimer1Array[MAX_FUNCTIONS] = { nullptr };
pFunction * execTimer1 = execTimer1Array;
pFunction execTimer2Array[MAX_FUNCTIONS] = { nullptr };
pFunction * execTimer2 = execTimer2Array;

Timer::Timer(TIM_TypeDef * timer) :
		_timer(timer), _init(0) {
	if (timer == TIM1) {
		_timerIRQn = TIM1_UP_IRQn;
	} else if (timer == TIM2) {
		_timerIRQn = TIM2_IRQn;
	}
}

Timer::~Timer() {
}

void Timer::init() {
	if (_init == 0) {
		//Define Struct de Timer
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
		//Configurações de timer
		TIM_TimeBaseInitStruct.TIM_Prescaler = (SystemCoreClock / 1000000) - 1;
		TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
		TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
		if (_timer == TIM1) {
			//Habilita o clock
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
		} else if (_timer == TIM2) {
			//Habilita o clock
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		}
		TIM_TimeBaseInit(_timer, &TIM_TimeBaseInitStruct);
		TIM_Cmd(_timer, ENABLE);
		_init = 1;
	}
}
void Timer::setPeriod(uint16_t periodOverflow) {
	TIM_SetCounter(_timer, 0);
	TIM_SetAutoreload(_timer, periodOverflow);
}

void Timer::configIT(uint16_t periodOverflow, pFunction func) {
	//Configura interrrupção
	TIM_SetAutoreload(_timer, periodOverflow);
	TIM_SetCounter(_timer, 0x0);
	TIM_ITConfig(_timer, TIM_IT_Update, ENABLE);
	if (_timer == TIM1) {
		//Seta a função a ser executada dentro da ISR
		if (*execTimer1 == nullptr) {
			*execTimer1 = func;
			(*execTimer1)++;
		}
	} else if (_timer == TIM2) {
		//Seta a função a ser executada dentro da ISR
		if (*execTimer2 == nullptr) {
			*execTimer2 = func;
			(*execTimer2)++;
		}
	}
	TIM_ClearITPendingBit(_timer, TIM_IT_Update);
	NVIC_SetPriority(_timerIRQn, configLIBRARY_KERNEL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(_timerIRQn);

}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"

extern "C" void TIM1_UP_IRQHandler() {
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
		for (int i = 0; i < MAX_FUNCTIONS; i++) {
			if (execTimer1Array[i] == nullptr) {
				break;
			}
			execTimer1Array[i]();
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

extern "C" void TIM2_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		for (int i = 0; i < MAX_FUNCTIONS; i++) {
			if (execTimer2Array[i] == nullptr) {
				break;
			}
			execTimer2Array[i]();
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

#pragma GCC diagnostic pop
