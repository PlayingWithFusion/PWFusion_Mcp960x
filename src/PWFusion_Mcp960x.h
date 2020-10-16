/***************************************************************************
* File Name: PWFusion_Mcp960x.h
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
#ifndef PWFUSION_MCP960X_H
#define PWFUSION_MCP960X_H

#include <Wire.h>
#include <Arduino.h>

typedef enum Mcp960x_Resolution_e {
  RES_18BIT = 0x0,
  RES_16BIT = 0x1,  
  RES_14BIT = 0x2,
  RES_12BIT = 0x3
} TCA9548_Resolution;

typedef enum Mcp960x_Status_e {
  PENDING,
  OPEN_CIRCUIT,
  SHORT_CIRCUIT,
  READY,
} Mcp960x_Status;

typedef enum Mcp960x_Thermocouple_e {
  TYPE_K = 0x0,
  TYPE_J = 0x1,
  TYPE_T = 0x2,
  TYPE_N = 0x3,
  TYPE_S = 0x4,
  TYPE_E = 0x5,
  TYPE_B = 0x6,
  TYPE_R = 0x7
} Mcp960x_Thermocouple;

typedef enum Mcp960x_CJ_Resolution_e {
  RES_0p0625 = 0x0,
  RES_0p2500 = 0x1
} Mcp960x_CJ_Resolution;

typedef enum Mcp960x_BurstMode_e {
  BURST_1   = 0x0,
  BURST_2   = 0x1,
  BURST_4   = 0x2,
  BURST_8   = 0x3,
  BURST_16  = 0x4,
  BURST_32  = 0x5,
  BURST_64  = 0x6,
  BURST_128 = 0x7
} Mcp960x_BurstMode;

typedef enum Mcp960x_Mode_e {
  MODE_NORMAL   = 0x0,
  MODE_SHUTDOWN = 0x1,
  MODE_BURST    = 0x2
} Mcp960x_Mode;

typedef enum Mcp960x_Reg_e {
  REG_HOT_JUNC   = 0x00,
  REG_DELTA_TEMP = 0x01,
  REG_COLD_JUNC  = 0x02,
  REG_RAW_ADC    = 0x03,
  REG_STATUS     = 0x04,
  REG_SENSOR_CFG = 0x05,
  REG_DEV_CFG    = 0x06,
  REG_ALERT1     = 0x08,
  REG_ALERT2     = 0x09,
  REG_ALERT3     = 0x0A,
  REG_ALERT4     = 0x0B,
  REG_ALERT1_HYS = 0x0C,
  REG_ALERT2_HYS = 0x0D,
  REG_ALERT3_HYS = 0x0E,
  REG_ALERT4_HYS = 0x0F,
  REG_ALERT1_LIM = 0x10,
  REG_ALERT2_LIM = 0x11,
  REG_ALERT3_LIM = 0x12,
  REG_ALERT4_LIM = 0x13,
  REG_DEV_ID     = 0x20
} Mcp960x_Reg;

class Mcp960x
{
  public:
    bool begin(uint8_t addr = 0, TwoWire &i2cPort = Wire);
    bool isConnected();
    float getThermocoupleTemp();
    float getColdJunctionTemp();
    void setMode(Mcp960x_Mode mode);
    void setResolution(TCA9548_Resolution thermocoupleRes, Mcp960x_CJ_Resolution cjRes);
    void setThermocoupleType(Mcp960x_Thermocouple type);
    void setFilterCoeffecients(uint8_t coeff);
    void beginBurst(Mcp960x_BurstMode mode);
    bool burstComplete();
    Mcp960x_Status getStatus();

    bool writeByte(Mcp960x_Reg reg, uint8_t data);
    bool writeWord(Mcp960x_Reg reg, uint16_t data);
    uint8_t readByte(Mcp960x_Reg reg);
    uint16_t readWord(Mcp960x_Reg reg);

  private:
    TwoWire *_i2cPort;
    uint8_t _addr;
    uint8_t snsrCfgReg;
    uint8_t devCfgReg;
};
  
#endif // PWFUSION_MCP960X_H
