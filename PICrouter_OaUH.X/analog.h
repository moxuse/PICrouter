/*
 * Copylight (C) 2012, Shunichi Yamamoto, tkrworks.net
 *
 * This file is part of PICrouter.
 *
 * PICrouter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option ) any later version.
 *
 * PICrouter is distributed in the hope that it will be useful,
 * but WITHIOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PICrouter. if not, see <http:/www.gnu.org/licenses/>.
 *
 * analog.h,v.0.51 2013/01/18
 */

#ifndef ANALOG_H
#define	ANALOG_H

#include <plib.h>
#include <GenericTypeDefs.h>
#include <math.h>

#include "osc.h"

typedef enum {
	MIDI_FADER     =  0,
	MIDI_VOLUME,
	MIDI_ORIGINAL,
	LONG_FADER,
	LONG_VOLUME,
	LONG_ORIGINAL
} TYPE_AN_VAL;

#define USE_ADC_NUM  14
#define FLTR_ADC_CNT 8

BOOL analogEnable[USE_ADC_NUM];
BOOL analogSendFlag[USE_ADC_NUM];
BYTE count[USE_ADC_NUM];
LONG analog[USE_ADC_NUM][FLTR_ADC_CNT];
LONG currentAnalog[USE_ADC_NUM];
LONG prevAnalog[USE_ADC_NUM];

void initAnalogVariables(void);
void resetAnalogFlag(BYTE port);
BOOL getAnalogFlag(BYTE port);
BYTE getAnalogByte(BYTE port, BYTE type);
WORD getAnalogWord(BYTE port, BYTE type);
void analogInHandle(BYTE port, LONG value);
void sendAdc();

#endif	/* ANALOG_H */

