#include "main.h"

#use delay(clock = 32MHZ, internal = 8MHZ)
#BANK_DMA
unsigned int16 DMA_ADC_BUFFER[BUFFER_SIZE];
#BANK_DMA
char DMA_UART_TX_BUFFER[BUFFER_SIZE];
//Todo:: Two DMA Buffers for real time data sampling


#INT_DMA0
void  DMA_0_ISR(void) 
{
   DMADoneFlag = 1;
}

#INT_DMA1
void DMA_1_ISR(void)
{
}

#INT_TIMER1
void Timer_ISR()
{
   output_toggle(LED_PIN);   
   if(NormalizeFlag == 1)
   {
      read_adc();
   }
   else
   {
      unsigned int8 ADCValue = 0;
      
      if (TriggerFlag != 2)
      {
          ADCValue = read_adc();
      }
      
      if (DMAFlag == 0)
      {
         disable_interrupts(INT_DMA0);
         memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE);
         DMAFlag = 1;
      }
      
      if((ADCValue == TriggerValue) && (TriggerFlag == 0))
      {
         DMA_ADC_BUFFER[0] = ADCValue;
         TriggerFlag = 1;
      }
      else if( DMA_ADC_BUFFER[1] > ADCValue)
      {
         DMA_ADC_BUFFER[1] = ADCValue;
         TriggerFlag = 2;
      }
      else if(TriggerFlag == 2)
      {
         if(DMAFlag == 1)
         {
            memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE);
            dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE);
            enable_interrupts(INT_DMA0);
            DMAFlag = 2;
         }
         
         read_adc();//Fill DMA_ADC_BUFFER FROM POSITION 2 -> END OF BUFFER
      }  
      else
      {
         ErrorCounter++;
         
         if (ErrorCounter >= 3000)
         {
            NormalizeFlag = 1;
         }
         
         TriggerFlag = 0;
      }
   }
   
}

#INT_RDA2      // interrupt handler for the uart RS232 communication
void ISR_UART2()
{
    UARTRX = fgetc(SHARP); // recieve data
    UARTRXFlag = 1;         // serial flag turned high
}

void main()
{   
   memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE);
   memset(DMA_UART_TX_BUFFER, 'a', BUFFER_SIZE);
   
   setup_dma(ADC_DMA_CHANNEL, DMA_IN_ADC1, DMA_WORD);
   dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE);
   
   setup_dma(UART_TX_DMA_CHANNEL, DMA_OUT_UART2, DMA_BYTE);

   enable_interrupts(INT_DMA0);
   enable_interrupts(INT_DMA1);
   
//!   setup_adc_ports(sAN0, VSS_VDD);
//!   setup_adc(ADC_CLOCK_INTERNAL);

   setup_adc(ADC_CLOCK_DIV_2 | ADC_TAD_MUL_4);
   setup_adc_ports(sAN0 | VSS_VDD);
   
   read_adc();
   
   TimerTicks = 53334;
   
   setup_timer1(TMR_INTERNAL , TimerTicks);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INTR_GLOBAL);
   
   NormalizeFlag = 1;

   while(TRUE)
   {
      
      if(DMADoneFlag)
      {
         disable_interrupts(INT_DMA0);
//!         for(unsigned int16 i=0; i< BUFFER_SIZE; i++)
//!         {
//!               printf("\n\r ADC Val: %c", DMA_ADC_BUFFER[i]);
//!         }
         for (IndexType Index = 0; Index < NumberOfDigitizationRequired; Index++)
         {
            AccumulateAnalogData(Index);
         }
         
         if (NormalizeFlag == 1)
         {
            NormalizeData();
         }
         else
         {
         
         }
         
         //dma_start(UART_TX_DMA_CHANNEL, DMA_ONE_SHOT | DMA_FORCE_NOW, &DMA_ADC_BUFFER[0], BUFFER_SIZE);    
         //dma_start(UART_TX_DMA_CHANNEL, DMA_ONE_SHOT | DMA_FORCE_NOW, &DigitizedData[0], BUFFER_SIZE); 
         //DMA THE ANALOG DATA ARRAY ALSO 
         
            for (IndexType i = 0; i < BUFFER_SIZE; i++) // send input array data
            {
                printf("%c", AnalogData[i]); // send every emelent of the array as a byte
            }

            for (i = 0; i < BUFFER_SIZE; i++) // send digitized data
            {
                printf("%c", DigitizedData[i]); // send every emelent of the array as a byte
            }
         enable_interrupts(INT_DMA0);
         DMADoneFlag = 0;
         //DMAFlag = 0;
         //TriggerFlag = 0;
      }      
   }
}

void AccumulateAnalogData(IndexType NumberOfDigitizationRequired)
{
   IndexType DMAADCIndex = (NumberOfDigitizationRequired * COEF_LENGTH);
   IndexType Index;
   memset(InputSamples, 0, COEF_LENGTH);
   
   for (Index = 0; Index < COEF_LENGTH; Index++) //Todo:: Replace with MemCpy() 
   {
      InputSamples[Index] = DMA_ADC_BUFFER[DMAADCIndex++] >> 4;
   }
   
   DMAADCIndex = (NumberOfDigitizationRequired * COEF_LENGTH);  
   
   for (Index = 0; Index < COEF_LENGTH; Index++)
   {
      InputIndex = CurrentIndex; 
      Accumulator = 0;
      CoefficentIndex = 0;
      
      while (CoefficentIndex < COEF_LENGTH - 1)
      {
         Accumulator += (signed int32)InputSamples[InputIndex] * (signed int32)fir_coef[CoefficentIndex];
           // condition for the circular buffer
         if (InputIndex == COEF_LENGTH - 1)
         {
            InputIndex = 0;
         }
         else
         {
            InputIndex++;
         }
         CoefficentIndex++;
      }
      
      AnalogData[DMAADCIndex++] = InputSamples[Index];
      
      if (NormalizeFlag == 1)
      {
         DigitizedData[DMAADCIndex] = Accumulator;
      }
      else
      {
         float StepOne = Accumulator - AverageAnalogValue;
         float StepTwo = StepOne * AverageMultiplier;
         float StepThree = StepTwo + (ADC_MAX_DATA_VALUE / 2);
         //OutputValue = (Accumulator - AverageAnalogValue) * AverageMultiplier + (ADC_MAX_DATA_VALUE / 2);
         //ConversionValue = (unsigned int8)OutputValue;
         ConversionValue = (unsigned int8)StepThree;
         DebugAccumulator[DMAADCIndex] = Accumulator;
         DigitizedData[DMAADCIndex] = ConversionValue;
      }
      
      if (CurrentIndex == 0)
      {
        CurrentIndex = COEF_LENGTH - 1;
      }
      else
      {
        CurrentIndex--;
      }
   }
  
}

void NormalizeData()
{
   if (ErrorCounter < 3000)
   {
      MaxAnalogValue = DigitizedData[COEF_LENGTH]; //Todo::Remove Gloab Vairables Where Possbile 
      MinAnalogValue = DigitizedData[COEF_LENGTH]; //Todo::Remove Gloab Vairables Where Possbile 
      InitialTriggerValue = DMA_ADC_BUFFER[COEF_LENGTH];
      
      for (IndexType Index = COEF_LENGTH + 1 ; Index < BUFFER_SIZE; Index++)
      {
         if (MinAnalogValue > DigitizedData[Index])
         {
            MinAnalogValue = DigitizedData[Index];
         }
         
         if (MaxAnalogValue < DigitizedData[Index])
         {
            MaxAnalogValue = DigitizedData[Index];
         }
         
         if (InitialTriggerValue > DMA_ADC_BUFFER[Index])
         {
            InitialTriggerValue = DMA_ADC_BUFFER[Index];
         }
         
         AverageAnalogValue = AverageAnalogValue + DigitizedData[Index];
      }
      
      AverageDivider = MaxAnalogValue - MinAnalogValue;
      AverageMultiplier = ((float)ADC_MAX_DATA_VALUE / AverageDivider);
      AverageAnalogValue = AverageAnalogValue / (BUFFER_SIZE - COEF_LENGTH);
      
      TriggerValue = InitialTriggerValue;
               
      memset(DigitizedData, 0, BUFFER_SIZE);          
   }  
   else 
   {
      InitialTriggerValue = DMA_ADC_BUFFER[64];
      for (IndexType i = 65; i < BUFFER_SIZE; i++)
      {
        if (InitialTriggerValue > DMA_ADC_BUFFER[i])
        {
            InitialTriggerValue = DMA_ADC_BUFFER[i];
        }             
      }
      
      TriggerValue = InitialTriggerValue;
      ErrorCounter = 0; 
   }
   
   NormalizeFlag = 0;
   CurrentIndex = 0;
}
