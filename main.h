#ifndef MAIN_H
#define MAIN_H

#include <33FJ64MC802.h>
#device ADC=12
#device ICD=TRUE
#device ICSP=1
#use delay(internal=40MHz)

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOJTAG                   //JTAG disabled

#pin_select U2TX=PIN_B6
#pin_select U2RX=PIN_B7
#USE RS232(UART2, BAUD = 115200, PARITY = N, BITS = 8, STOP = 1, TIMEOUT = 500,  stream = SHARP)) // RECEIVE_BUFFER=255, TRANSMIT_BUFFER=255, TXISR,

#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define BUFFER_SIZE 256
#define COEF_LENGTH 64
#define LOADER_PAGES 50
#define ADC_MAX_DATA_VALUE 255

typedef unsigned int32 IndexType;
typedef unsigned int8 FlagType;

const unsigned int8 ADC_DMA_CHANNEL = 0;
const unsigned int8 UART_TX_DMA_CHANNEL = 1;
const IndexType NumberOfDigitizationRequired = BUFFER_SIZE / COEF_LENGTH;

unsigned int16 Frequency = 0;

unsigned int8 ConversionValue; // normalized value
unsigned int8 AnalogData[BUFFER_SIZE]; // input array

unsigned int8 CSharpCoefficent[2];          // array to hold byte data of coefficents

unsigned int16 InitialTriggerValue = 0; // used for initail trigger value
unsigned int16 TriggerValue = 0; // used for initail trigger value

signed int16 ByteConversionResult = 0;     // result of byte convertion

signed int32 Accumulator = 0; // accumulator of the output value in the difference equation calculation
signed int32 DigitizedData[BUFFER_SIZE];   // output array
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

FlagType DMADoneFlag = 0;
FlagType NormalizeFlag = 0;

FlagType UARTRXFlag = 0;   // serial flag
FlagType UARTTXFlag = 0;       // flag to send data out
FlagType HandShakeFlag = 1;    // handshake

void AccumulateAnalogData(IndexType);
void NormalizeData();

signed int16 fir_coef[COEF_LENGTH] = {  };

#endif // MAIN_H



