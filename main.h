#ifndef MAIN_H
#define MAIN_H

#include <33FJ64MC802.h>
#device ADC=12
#device ICSP=1
#use delay(internal=32MHz)
//#use delay(internal=40MHz)


#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOJTAG                   //JTAG disabled
#FUSES FRC_PLL

#pin_select U2TX=PIN_B6
#pin_select U2RX=PIN_B7
#USE RS232(UART2, BAUD = 115200, PARITY = N, BITS = 8, STOP = 1, TIMEOUT = 500,  stream = SHARP)) // RECEIVE_BUFFER=255, TRANSMIT_BUFFER=255, TXISR,

#include <math.h>
#include <stdio.h>
#include <ctype.h>

#define BUFFER_SIZE 256
#define COEF_LENGTH 64
#define LOADER_PAGES 50
#define ADC_MAX_DATA_VALUE 255
#define LED_PIN PIN_B14   

typedef unsigned int32 IndexType;
typedef unsigned int8 FlagType;

const unsigned int8 ADC_DMA_CHANNEL = 0;
const unsigned int8 UART_TX_DMA_CHANNEL = 1;
const IndexType NumberOfDigitizationRequired = BUFFER_SIZE / COEF_LENGTH;

unsigned int16 TimerTicks = 0;

unsigned int8 ConversionValue; // normalized value
unsigned int8 AnalogData[BUFFER_SIZE]; // input array

unsigned int8 CSharpCoefficent[2];          // array to hold byte data of coefficents

unsigned int16 InitialTriggerValue = 0; // used for initail trigger value
unsigned int16 TriggerValue = 0; // used for initail trigger value

signed int16 ByteConversionResult = 0;     // result of byte convertion

signed int32 Accumulator = 0; // accumulator of the output value in the difference equation calculation
signed int32 DigitizedData[BUFFER_SIZE];   // output array
signed int32 DebugAccumulator[BUFFER_SIZE];   // output array
signed int32 MaxAnalogValue = 0; // use for normalization
signed int32 MinAnalogValue = 0; // use for normalization

signed int64 AverageAnalogValue = 0; // use for normalization

signed int16  InputSamples[COEF_LENGTH]; // array used as a circular buffer for the input samples

float OutputValue;          // holds the current output value
float AverageDivider = 0; // use for normalization
float AverageMultiplier = 0;    // use for normalization

char UARTRX; // data in

IndexType CSharpCoefficentRecieved = 0;    // counter of coefficents recieved from c#
IndexType CoefficentIndex = 0;            // used as the index for the filter coefficients array in the difference equation calculation
IndexType InputIndex= 0;           // used as the index for the input samples array in the difference equation calculation
IndexType CurrentIndex = 0;          // keeps track of the current position of the circular buffer
IndexType NumberCSharpByteRecieved = 0; // counter to track which byte is being recieved
IndexType ErrorCounter = 0; // error detection
IndexType NormalizeDataCounter = 0;

FlagType DMADoneFlag = 0;
FlagType NormalizeFlag = 0;
FlagType UARTRXFlag = 0;   // serial flag
FlagType UARTTXFlag = 0;       // flag to send data out
FlagType HandShakeFlag = 1;    // handshake
FlagType TriggerFlag = 0;
FlagType DMAFlag = 0;

void AccumulateAnalogData(IndexType);
void NormalizeData();
unsigned int8 QuickDigitize(unsigned int16);
signed int16 fir_coef[COEF_LENGTH] =
{  
210,   -167,   -150,   -155,   -171,   -192,   -213,   -231,   -243, // 10 HZ LPF FS 300HZ freq = 53334 Fin
-246,   -239,   -218,   -183,   -133,    -65,     19,    120,    237,
368,    511,    664,    823,    984,   1145,   1302,   1449,   1584,
1703,   1802,   1879,   1931,   1958,   1958,   1931,   1879,   1802,
1703,   1584,   1449,   1302,   1145,    984,    823,    664,    511,
368,    237,    120,     19,    -65,   -133,   -183,   -218,   -239,
-246,   -243,   -231,   -213,   -192,   -171,   -155,   -150,   -167,
210
};

#endif // MAIN_H



