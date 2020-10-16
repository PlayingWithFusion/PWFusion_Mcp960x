/***************************************************************************
* File Name: PWFusion_Mcp960x.cpp
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.8.13
*
* Designed for use with with Playing With Fusion Mcp960x Thermocouple 
* Breakouts: SEN-30010 and SEN-30011
*
*   SEN-30010 (universal applications)
*   ---> http://www.playingwithfusion.com/productview.php?pdid=116
*
*   SEN-30011 (universal applications)
*   ---> http://www.playingwithfusion.com/productview.php?pdid=120
*
* Copyright © 2020 Playing With Fusion, Inc.
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* REVISION HISTORY:
* Author        Date      Comments
* J. Leonard    2020Aug   Original version
* 
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source 
* development by buying products from Playing With Fusion!
**************************************************************************/
#include "PWFusion_Mcp960x.h"

#define BASE_ADDR 0x60

// Initialize the MCP9601 driver, where addr is an address between 0 and 7.
bool Mcp960x::begin(uint8_t addr, TwoWire &i2cPort)
{
    _addr = BASE_ADDR | addr;
    _i2cPort = &i2cPort;
    
    // Default to high resolution, normal (periodic) mode
    setResolution(RES_18BIT, RES_0p0625);
    setMode(MODE_NORMAL);
    
    return true;
}


// Does the MCP9601 respond with the correct device ID?
bool Mcp960x::isConnected()
{  
  uint16_t devId = readWord(REG_DEV_ID);
  return (devId & 0xFF00) == 0x4100;
}


// Return the calculated thermocouple temperature in degrees centigrade
float Mcp960x::getThermocoupleTemp()
{
  int16_t temp = (int16_t)readWord(REG_HOT_JUNC);
 
  // Clear Burst and Th complete bits
  writeByte(REG_STATUS, 0);  

  return (float)temp / 16.0;
}


// Return the cold junction temperature (the temperature of the MCP9601
// itself) in degrees centigrade.   This can be used to estimate the 
// ambient temperature.
float Mcp960x::getColdJunctionTemp()
{
  int16_t temp = (int16_t)readWord(REG_COLD_JUNC);
  return (float)temp / 16.0;
}


// Set the MCP9601 operating mode (normal, burst, shutdown).  See MCP9601
// part datasheet for more information
void Mcp960x::setMode(Mcp960x_Mode mode)
{
  devCfgReg &= 0xFC;
  devCfgReg |= mode;

  writeByte(REG_DEV_CFG, devCfgReg);  
}


// Specify the thermocouple (hot junction) and the cold junction ADC resolution.
// Higher resolution will result in greater measurement prescision, but measurements
// will take more time.
void Mcp960x::setResolution(TCA9548_Resolution thermocoupleRes, Mcp960x_CJ_Resolution cjRes)
{
  devCfgReg &= 0x1F;
  devCfgReg |= (uint8_t)cjRes << 7;
  devCfgReg |= (uint8_t)thermocoupleRes << 5;

  writeByte(REG_DEV_CFG, devCfgReg);  
}


// Specify the thermocouple type (J, K, T, etc).  The thermocouple type is used to
// determine the correct lookup table to convert thermocouple voltage to temperature
// in degrees C.
void Mcp960x::setThermocoupleType(Mcp960x_Thermocouple type)
{
  snsrCfgReg &= 0x8F;
  snsrCfgReg |= (uint8_t)type << 4;
  
  writeByte(REG_SENSOR_CFG, snsrCfgReg);
}


// Specify the coeffecient used by the MCP9601 built in low-pass filter.  A value
// of zero results in very little filtering.   A value of 7 results in heavy filtering.
// See the MCP9601 datasheet for more information
void Mcp960x::setFilterCoeffecients(uint8_t coeff)
{
  snsrCfgReg &= 0xF8;
  snsrCfgReg |= (coeff & 0x7);
  
  writeByte(REG_SENSOR_CFG, snsrCfgReg);  
}


// Place the MCP9601 into burst mode and begin a burst measurement.  Burst mode can
// be used to take several temperature mesurements then place the MCP9601 into
// sleep mode automatically.    After calling this function, wait for the measurment
// to complete by calling the burstComplete() function.  Once burtComplete()
// returns true, call getThermocoupleTemp() to receive the measured temperature.
// See MCP9601 datasheet for more information on burst mode
void Mcp960x::beginBurst(Mcp960x_BurstMode mode)
{
    // Clear Burst and Th complete bits
  writeByte(REG_STATUS, 0);  
  
  devCfgReg &= 0xE0;
  devCfgReg |= (uint8_t)mode << 2;
  devCfgReg |= MODE_BURST;

  writeByte(REG_DEV_CFG, devCfgReg);  
}


// Determine if a burst measurement is complete.  beginBurst() should be called
// before calling burstComplete()
bool Mcp960x::burstComplete()
{
  bool complete = false;
  uint8_t statusReg = readByte(REG_STATUS);

  complete = (statusReg & 0x80) != 0;

  return complete; 
}


// Determine the status of the MCP9601.  OPEN_CIRCUIT indicates that no
// thermocouple is connected.   SHORT_CIRCUIT indicates that one or both
// thermocouple inputs are shorted to VDD or GND.
Mcp960x_Status Mcp960x::getStatus()
{
  uint8_t statusReg = readByte(REG_STATUS);
  Mcp960x_Status status;

  if ((statusReg & 0x10) != 0) {
    status = OPEN_CIRCUIT;
  }
  else if ((statusReg & 0x20) != 0) {
    status = SHORT_CIRCUIT;
  }
  else if ((statusReg & 0xC0) != 0) {
    status = READY;
  }
  else {
    status = PENDING;
  }

  return (Mcp960x_Status)status;  
}

    
// Write a byte to a specific MCP9601 register
bool Mcp960x::writeByte(Mcp960x_Reg reg, uint8_t data)
{
  _i2cPort->beginTransmission(_addr);  
  _i2cPort->write(reg);
  _i2cPort->write(data & 0xFF); 

  return (_i2cPort->endTransmission(true) != 0);
}


// Write 16-bits of data to a specific MCP9601 register
bool Mcp960x::writeWord(Mcp960x_Reg reg, uint16_t data)
{
  _i2cPort->beginTransmission(_addr);  
  _i2cPort->write(reg);
  _i2cPort->write(data >> 8); 
  _i2cPort->write(data & 0xFF); 

  return (_i2cPort->endTransmission(true) != 0);
}


// Read a byte from a specific MCP9601 register
uint8_t Mcp960x::readByte(Mcp960x_Reg reg)
{
  uint8_t result = 0;

  for (int i; i<4; i++)
  {
    _i2cPort->beginTransmission(_addr);  
    _i2cPort->write(reg);         
    _i2cPort->endTransmission(false);  

    if (_i2cPort->requestFrom(_addr, 1) != 0)
    {
      result = _i2cPort->read();
      break;
    }
  }

  return result;
}


// Read a 16-bit value from a specific MCP9601 register
uint16_t Mcp960x::readWord(Mcp960x_Reg reg)
{
  uint16_t result = 0;

  for (int i; i<4; i++)
  {
    _i2cPort->beginTransmission(_addr);  
    _i2cPort->write(reg);         
    _i2cPort->endTransmission(false);  

    if (_i2cPort->requestFrom(_addr, 2) != 0)
    {
      result = (uint16_t)_i2cPort->read() << 8;
      result |= (uint16_t)_i2cPort->read();
      break;
    }
  }

  return result;
}
