//=================================================================================//
//	Arquivo : DigitalOut.cpp
//	Projeto : FreeRtos9CMSIS
//	Autor : Maikeu Locatelli
//	Copyright : Locatelli Engenharia
//
//	Descricão: Objeto para saídas GPIO do STM32F103
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

#include <DigitalOut.h>
#include <debug.h>

//==================================================================================//
//	Definição dos Métodos
//==================================================================================//

DigitalOut::DigitalOut(GPIO_TypeDef* port, uint16_t pin) :
		_port(port), _pin(pin), _init(0) {
	if (port == GPIOA)
		_clk = RCC_APB2Periph_GPIOA;
	else if (port == GPIOB)
		_clk = RCC_APB2Periph_GPIOB;
	else if (port == GPIOC)
		_clk = RCC_APB2Periph_GPIOC;
	else _clk = 0;
}

DigitalOut::~DigitalOut() {
	// TODO Auto-generated destructor stub
}

void DigitalOut::init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	// Habilita o Clock
	RCC_APB2PeriphClockCmd(_clk, ENABLE);
	GPIO_InitStructure.GPIO_Pin = _pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &GPIO_InitStructure);
	_init = 1;
}

void DigitalOut::setOutput(State state){
	if (state == State::HIGH){
		GPIO_SetBits(_port, _pin);
	}
	else {
		GPIO_ResetBits(_port, _pin);
	}
}

State DigitalOut::getOutput(){
	if (GPIO_ReadOutputDataBit(_port, _pin) == Bit_SET){
		return State::HIGH;
	}
	else {
		return State::LOW;
	}

}
