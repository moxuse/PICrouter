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
 * analog.c,v.0.7.5 2013/07/10
 */

#include "analog.h"

static BOOL analogEnable[AN_NUM];
static BYTE analogType[AN_NUM];
static float analogCurve[AN_NUM];
static BOOL analogSendFlag[AN_NUM];
static BYTE count[AN_NUM];
static LONG analog[AN_NUM][FLTR_ADC_CNT];
static LONG currentAnalog[AN_NUM];
static LONG prevAnalog[AN_NUM];

// for Infinium or CF-X2
static int currentValue[AN_NUM];// = {0};
static int prevValue[AN_NUM];// = {0};
static int boundaryValue[AN_NUM];// = {0};
static int currentDirectionValue[AN_NUM];// = {0};
static int currentPosition;// = 0;
static int currentPosition1;// = 0;
static int prevPosition;// = 0;
static int boundaryPosition[AN_NUM];// = {0};
static int centerPosition;// = 0;
static int currentSection;// = 0;
static int prevSection;// = 0;
static int currentDirection;// = 0;
static int currentDirection1;// = 0;
static int prevDirection;// = 0;

/*******************************************************************************
  Function:
    void setAnalogEnable(BYTE port, BOOL flag)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    BYTE flag - a flag whether an analog port is enabled or not.

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void setAnalogEnable(BYTE port, BOOL flag)
{
    analogEnable[port] = flag;
}

/*******************************************************************************
  Function:
    BOOL getAnalogEnable(BYTE port)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)

  Return Values:
    TREU  - Enabled an analog input of the "port"
    FALSE - Disabled an analog input of the "port"

  Remarks:
    None
*******************************************************************************/
BOOL getAnalogEnable(BYTE port)
{
    return analogEnable[port];
}

/*******************************************************************************
  Function:
    void setAnalogType(BYTE port, BYTE type)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    BYTE type - an analog input type

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void setAnalogType(BYTE port, BYTE type)
{
    analogType[port] = type;
}

/*******************************************************************************
  Function:
    void getAnalogType(BYTE port)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)

  Return Values:
    MIDI_FADER    - 8bit value optimized for a slide volume.
    MIDI_VOLUME   - 8bit value optimized for a rotary volume.
    MIDI_ORIGINAL - 8bit value.
    LONG_FADER    - 10bit value optimized for a slide volume.
    LONG_VOLUME   - 10bit value optimized for a rotary volume.
    LONG_ORIGINAL - 10bit value.

  Remarks:
    None
*******************************************************************************/
BYTE getAnalogType(BYTE port)
{
    return analogType[port];
}

/*******************************************************************************
  Function:
    void setAnalogCurve(BYTE port, BYTE curve)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    BYTE curve - an analog input curve

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void setAnalogCurve(BYTE port, float curve)
{
    analogCurve[port] = curve;
}

/*******************************************************************************
  Function:
    void getAnalogCurve(BYTE port)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)

  Return Values:
    value(-4-+4) - float value

  Remarks:
    None
*******************************************************************************/
float getAnalogCurve(BYTE port)
{
    return analogCurve[port];
}

/*******************************************************************************
  Function:
    void initAnalogVariables(void)

  Precondition:


  Summary:
    The initialization of all variables for the analog input

  Description:


  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void initAnalogVariables(void)
{
    BYTE i, j;

    for(i = 0; i < AN_NUM; i++)
    {
        analogEnable[i] = FALSE;
        analogType[i] = BYTE_ORIGINAL;
        analogSendFlag[i] = FALSE;
        count[i] = 0;
        currentAnalog[i] = 0;
        prevAnalog[i] = 0;

        for(j = 0; j < FLTR_ADC_CNT; j++)
            analog[i][j] = 0;

        currentValue[i] = 0;
        prevValue[i] = 0;
        boundaryValue[i] = 0;
        currentDirectionValue[i] = 0;
        boundaryPosition[i] = 0;
    }

    currentPosition = 0;
    currentPosition1 = 0;
    prevPosition = 0;
    centerPosition = 0;
    currentSection = 0;
    prevSection = 0;
    currentDirection = 0;
    currentDirection1 = 0;
    prevDirection = 0;

    // A/D Manual Scan 
#if 1 // for PICrouter
    AD1PCFG = 0x0000FFFF;// 0000 0000 0000 0000 1111 1111 1111 1111
    AD1CON2 = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CSSL = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CON1 = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CHS  = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CON3 = 0x00001F08;// 0000 0000 0000 0000 0000 1111 0000 1000
#else // for PrB_MINI
//A/D Auto Scan
#if 0// Infinium
    AD1PCFG = 0x0000FFF3;// 0000 0000 0000 0000 1111 1111 1111 0011
    AD1CON2 = 0x0000041C;// 0000 0000 0000 0000 0000 0100 0001 1100
    AD1CSSL = 0x0000000C;// 0000 0000 0000 0000 0000 0000 0000 1100
    AD1CON1 = 0x000010E6;// 0000 0000 0000 0000 1000 0000 1110 0110
    AD1CHS  = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CON3 = 0x00001F08;// 0000 0000 0000 0000 0000 1111 0000 1000
#else// CF-X2
    AD1PCFG = 0x0000FF03;// 0000 0000 0000 0000 1111 1111 0000 0011
    //AD1CON2 = 0x0000042C;// 0000 0000 0000 0000 0000 0100 0010 1100
    AD1CON2 = 0x00000414;// 0000 0000 0000 0000 0000 0100 0001 0100
    AD1CSSL = 0x000000FC;// 0000 0000 0000 0000 0000 0000 1111 1100
    AD1CON1 = 0x000080E6;// 0000 0000 0000 0000 1000 0000 1110 0110
    AD1CHS  = 0x00000000;// 0000 0000 0000 0000 0000 0000 0000 0000
    AD1CON3 = 0x00001F08;// 0000 0000 0000 0000 0000 1111 0000 1000
#endif
    AD1CON1bits.ON = 1;
#endif
}

/*******************************************************************************
  Function:
    void resetAnalogFlag(BYTE port)

  Precondition:


  Summary:
    This function is to reset an flag whether an analog value is sent or not.

  Description:


  Parameters:
    BYTE port - an analog port number(0-13)

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void resetAnalogFlag(BYTE port)
{
    analogSendFlag[port] = FALSE;
}

/*******************************************************************************
  Function:
    BOOL getAnalogFlag(BYTE port)

  Precondition:


  Summary:
    This function is to known a flag state whether an analog value is sent or not.

  Description:


  Parameters:
    BYTE port - an analog port number(0-13)

  Return Values:
    TREU  - Able to send an analog input value of the "port"
    FALSE - Not able to send an analog input value of the "port"

  Remarks:
    None
*******************************************************************************/
BOOL getAnalogFlag(BYTE port)
{
    return analogSendFlag[port];
}

/*******************************************************************************
  Function:
    BYTE getAnalogByte(BYTE port, BYTE type)

  Precondition:


  Summary:
    This function is to get a 8bit analog value.

  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    BYTE type - an analog input type(MIDI_FADER, MIDI_VOLUME or MIDI_ORIGINAL)

  Return Values:
    value(0-255) - 8-bit value

  Remarks:
    None
*******************************************************************************/
BYTE getAnalogByte(BYTE port, BYTE type)
{
  float fader;
  BYTE vol;

  switch(type)
  {
    case BYTE_FADER:
    	fader = (float)(currentAnalog[port] >> 3) * 1.008;
       	if(fader < 2.0)
          fader = 0.0;
       	else if(fader > 127.0)
          fader = 127.0;
       	vol = (BYTE)fader;
    	break;
    case BYTE_VOLUME:
#if 0
      vol = (BYTE)(127 - (currentAnalog[port] >> 2));
      if(vol > 60 && vol < 66)
        vol = 63;
#else
      vol = (BYTE)(255 - (currentAnalog[port] >> 2));
      if(vol > 124 && vol < 130)
        vol = 127;
#endif
    	break;
    case BYTE_ORIGINAL:
      vol = (BYTE)(currentAnalog[port] >> 2);
      break;
    default:
      vol = 0;
  }
	return vol;
}

/*******************************************************************************
  Function:
    WORD getAnalogWord(BYTE port, BYTE type)

  Precondition:


  Summary:
    This function is to get a 10bit analog value.

  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    BYTE type - an analog input type(LONG_FADER, LONG_VOLUME or LONG_ORIGINAL)

  Return Values:
    value(0-1023) - 16-bit value

  Remarks:
    None
*******************************************************************************/
WORD getAnalogWord(BYTE port, BYTE type)
{
  float fader;
  WORD vol;

  switch(type)
  {
    case WORD_FADER:
      fader = (float)(currentAnalog[port]) * 1.008;
      if(fader < 8.0)
        fader = 0.0;
      else if(fader > 1024.0)
        fader = 1024.0;
      vol = (WORD)fader;
      break;
    case WORD_VOLUME:
      vol = (WORD)(1024 - currentAnalog[port]);
      if(vol > 506 && vol < 518)
        vol = 512;
      break;
    case WORD_ORIGINAL:
      vol = (WORD)currentAnalog[port];
      break;
    default:
      vol = 0;
      break;
  }
  return vol;
}

/*******************************************************************************
  Function:
    void analogInHandle(BYTE port, LONG value)

  Precondition:


  Summary:


  Description:


  Parameters:
    BYTE port - an analog port number(0-13)
    LONG value

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void analogInHandle(BYTE port, LONG value)
{
    BYTE i;

    analog[port][count[port]] = value;
    (count[port])++;

    if(count[port] == FLTR_ADC_CNT)
    {
        currentAnalog[port] = 0;
        for(i = 0; i < FLTR_ADC_CNT; i++)
            currentAnalog[port] += analog[port][i];
        currentAnalog[port] /= (WORD)FLTR_ADC_CNT;
        if((analogType[port] < WORD_FADER && labs(currentAnalog[port] - prevAnalog[port]) > 4) ||
           ((analogType[port] >= WORD_FADER && analogType[port] <= WORD_ORIGINAL) && labs(currentAnalog[port] - prevAnalog[port]) > 0))
        {
            prevAnalog[port] = currentAnalog[port];
            analogSendFlag[port] = TRUE;
        }
        count[port] = 0;
    }
}

/*******************************************************************************
  Function:
    void sendAdc()

  Precondition:


  Summary:
    This function sends the OSC message "/std/adc".

  Description:


  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void sendAdc(void)
{
    BYTE i;

    for(i = 0; i < AN_NUM; i++)
    {
        if(getAnalogFlag(i))
        {
            if(analogType[i] < WORD_FADER)
                sendOSCMessage(getOSCPrefix(), msgAdc, "ii", i, getAnalogByte(i, analogType[i]));
            else if(analogType[i] >= WORD_FADER && analogType[i] <= WORD_ORIGINAL)
                sendOSCMessage(getOSCPrefix(), msgAdc, "ii", i, getAnalogWord(i, analogType[i]));
            resetAnalogFlag(i);
            Delay10us(2);
        }
    }
}

/*******************************************************************************
  Function:
    void sendInfinium(void)

  Precondition:


  Summary:
    This function is to use the infinium fader in place of sendAdc().

  Description:


  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void sendInfinium(void)
{
    BYTE i;
    BOOL flag = FALSE;
    
    for(i = 0; i < 2; i++)
    {
        if(getAnalogFlag(i))
        {
            flag = TRUE;
            currentValue[i] = getAnalogByte(i, BYTE_ORIGINAL);
            if(i == 1)
            {
                BYTE faderValue = 0;
#if 0
                if(currentValue[0] < 127)
                    faderValue = 255 - currentValue[1];
                else
                    faderValue = currentValue[0];
#else
                if(currentValue[0] <= 127 && currentValue[1] > 127)
                {
                    faderValue = currentValue[0];
                    sendOSCMessage(getOSCPrefix(), msgAdc, "iiii", i + 10, faderValue, currentValue[0], currentValue[1]);
                }
                else if(currentValue[0] > 127 && currentValue[1] <= 127)
                {
                    faderValue = 255 - currentValue[1];
                    sendOSCMessage(getOSCPrefix(), msgAdc, "iiii", i + 10, faderValue, currentValue[0], currentValue[1]);
                }
#endif
            }
            resetAnalogFlag(i);
        }
    }
}
/*******************************************************************************
  Function:
    void sendCFX2(void)

  Precondition:


  Summary:
    This function is to use the vestax CF-X2 in place of sendAdc().

  Description:


  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
*******************************************************************************/
void sendCFX2(void)
{
    BYTE i;
    BOOL flag = FALSE;
    int b_min = 120;
    int b_max = 132;

    for(i = 0; i < 6; i++)
    {
        if(getAnalogFlag(i))
        {
            int value = getAnalogByte(i, BYTE_ORIGINAL);
            if(value != currentValue[i])
                prevValue[i] = currentValue[i];

            currentValue[i] = value;//getAnalogByte(i, TYPE_MIDI_ORIGINAL);

            if(currentValue[i] != prevValue[i])
            {
                if(currentValue[i] > prevValue[i])
                    currentDirectionValue[i] = 1;
                else if(currentValue[i] < prevValue[i])
                    currentDirectionValue[i] = -1;
            }
            flag = TRUE;
            resetAnalogFlag(i);
        }
    }

    if(getInitSendFlag() && flag)
    {
        if((currentValue[0] >= b_min && currentValue[0] <= b_max) &&
           (currentValue[1] >= b_min && currentValue[1] <= b_max) &&
           (currentValue[2] >= b_min && currentValue[2] <= b_max) &&
           (currentValue[3] >= b_min && currentValue[3] <= b_max))
        {
            currentSection = 0;
            currentPosition = 152 - currentValue[5 - currentSection];
        }
        else if((currentValue[0] >= b_min && currentValue[0] <= b_max) &&
                (currentValue[1] >= b_min && currentValue[1] <= b_max) &&
                (currentValue[2] >= b_min && currentValue[2] <= b_max) &&
                (currentValue[5] < b_min/*92*/))
        {
            currentSection = 1;
            currentPosition = (192 - currentValue[5 - currentSection]) + 90;
        }
        else if((currentValue[2] >= b_min && currentValue[2] <= b_max) &&
                (currentValue[3] >= b_min && currentValue[3] <= b_max) &&
                (currentValue[4] >= b_min && currentValue[4] <= b_max) &&
                (currentValue[5] >= b_min && currentValue[5] <= b_max))
        {
            currentSection = 5;
            currentPosition = (192 - currentValue[5 - currentSection]) + 550;
        }
        else if((currentValue[3] >= b_min && currentValue[3] <= b_max) &&
                (currentValue[4] >= b_min && currentValue[4] <= b_max) &&
                (currentValue[5] >= b_min && currentValue[5] <= b_max)
                 /**(currentValue + 2] < 72*/)
        {
            currentSection = 4;
            currentPosition = (184 - currentValue[5 - currentSection]) + 430;
        }
        else if((currentValue[0] >= b_min && currentValue[0] <= b_max) &&
                (currentValue[1] >= b_min && currentValue[1] <= b_max) &&
                (currentValue[5] >= b_min && currentValue[5] <= b_max) &&
                //(*(currentValue + 4] < 80))
                (currentValue[4] < 120))
        {
            currentSection = 2;
            currentPosition = (192 - currentValue[5 - currentSection]) + 202;
        }
        else if((currentValue[0] >= b_min && currentValue[0] <= 135) &&
                (currentValue[4] >= b_min && currentValue[4] <= b_max) &&
                (currentValue[5] >= b_min && currentValue[5] <= b_max))
        {
            currentSection = 3;
            currentPosition = (192 - currentValue[5 - currentSection]) + 312;
        }
        else
        {
            currentSection = -1;
        }

        if((currentPosition >> 1) != (currentPosition1 >> 1))
            prevPosition = currentPosition1;

        if((currentPosition >> 1) > (prevPosition >> 1))
            currentDirection = 1;
        else if((currentPosition >> 1) < (prevPosition >> 1))
            currentDirection = -1;
        if((currentPosition >> 1) != (currentPosition1 >> 1))
            sendOSCMessage(getOSCPrefix(), msgAdc, "iiiiiiiii",
                           (currentPosition >> 1),
                            currentDirection,
                            currentSection,
                            currentValue[0],
                            currentValue[1],
                            currentValue[2],
                            currentValue[3],
                            currentValue[4],
                            currentValue[5]);

        if(currentPosition > currentPosition1 && prevDirection == -1 && currentSection)
            prevDirection = -1;
        else
        {
            if(currentPosition <= 1)
                prevDirection = 0;
            else
                prevDirection = currentDirection;
        }
        prevSection = currentSection;
        currentPosition1 = currentPosition;
    }
}
