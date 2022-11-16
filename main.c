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

#INT_RDA2
void UART2_ISR()
{
   UARTRX = fgetc(SHARP);
   UARTRXFlag = 1;
}

#INT_TIMER1
void Timer_ISR()
{
   output_toggle(LED_PIN);
   read_adc();
//!   if(NormalizeFlag == 1)
//!   {
//!      read_adc();
//!      NormalizeDataCounter++;
//!   }
//!   else
//!   {
//!      unsigned int16 ADCValue = 0;
//!      
//!      if (TriggerFlag != 2)
//!      {
//!          ADCValue = QuickDigitize(read_adc());
//!      }
//!      
//!      if (DMAFlag == 0)
//!      {
//!         disable_interrupts(INT_DMA0);
//!         memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE * 2);
//!         DMAFlag = 1;
//!      }
//!      
//!      if((ADCValue == TriggerValue) && (TriggerFlag == 0))
//!      {
//!         TempInputSamples[0] = ADCValue;
//!         TriggerFlag = 1;
//!      }
//!      else if((ADCValue > TempInputSamples[0]) && (TriggerFlag == 1))
//!      {
//!         TempInputSamples[1] = ADCValue;
//!         TriggerFlag = 2;
//!      }
//!      else if(TriggerFlag == 2)
//!      {
//!         if(DMAFlag == 1)
//!         {
//!            memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE * 2);
//!            dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE);
//!            enable_interrupts(INT_DMA0);
//!            DMAFlag = 2;
//!         }
//!         
//!         read_adc();//Fill DMA_ADC_BUFFER FROM POSITION 2 -> END OF BUFFER
//!      }  
//!      else
//!      {
//!         ErrorCounter++;
//!         
//!         if (ErrorCounter > 3000)
//!         {
//!            NormalizeFlag = 1;
//!         }
//!         
//!         TriggerFlag = 0;
//!      }
//!   }
}

void main()
{   
   memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE);
   memset(DMA_UART_TX_BUFFER, 'a', BUFFER_SIZE);
   
   setup_dma(ADC_DMA_CHANNEL, DMA_IN_ADC1, DMA_WORD);
   dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE);
   
//!   setup_dma(UART_TX_DMA_CHANNEL, DMA_OUT_UART2, DMA_BYTE);
//!   enable_interrupts(INT_DMA1);

   enable_interrupts(INT_DMA0);

// setup_adc(ADC_CLOCK_DIV_2 | ADC_TAD_MUL_4);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(sAN0 | VSS_VDD);
   
   read_adc();
   
   //TimerTicks = 53334;
   
   setup_timer1(TMR_INTERNAL , TimerTicks);
   EnableInterrupts();
   
   NormalizeFlag = 1;

   while(TRUE)
   {
      
      if((DMADoneFlag) || (NormalizeDataCounter == BUFFER_SIZE))
      {
         disable_interrupts(INT_DMA0);
         for (IndexType Index = 0; Index < BUFFER_SIZE; Index++)
         {
            AccumulateAnalogData(Index);
         }
         
         if (NormalizeFlag == 1)
         {
            NormalizeData();
         }
            
         //dma_start(UART_TX_DMA_CHANNEL, DMA_ONE_SHOT | DMA_FORCE_NOW, &DigitizedData[0], BUFFER_SIZE); 
//!         Todo:: DMA THE ANALOG DATA ARRAY ALSO 
         if (HandShakeFlag == 1)
         {
            for (IndexType i = 0; i < BUFFER_SIZE; i++) // send input array data
            {
                printf("%c", AnalogData[i]); // send every emelent of the array as a byte
            }
   
            for (i = 0; i < BUFFER_SIZE; i++) // send digitized data
            {
                printf("%c", DigitizedData[i]); // send every emelent of the array as a byte
            }
         }
         
         NormalizeFlag = 0;
         HandShakeFlag = 0;   
         CurrentIndex = 0;
         enable_interrupts(INT_DMA0);
         DMADoneFlag = 0;
         TriggerFlag = 0;
         DMAFlag = 0;
      }  
      
      if (UARTRXFlag)
      {
         CommHandler(UARTRX);
      }  
   }
}

void AccumulateAnalogData(IndexType DMAADCIndex)
{
   InputSamples[CurrentIndex] = DMA_ADC_BUFFER[DMAADCIndex];
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
   
   AnalogData[DMAADCIndex] =  InputSamples[CurrentIndex] >> 4;
   
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

void NormalizeData(void)
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
      AverageMultiplier = (255.0/ AverageDivider);
      AverageAnalogValue = AverageAnalogValue / (BUFFER_SIZE - COEF_LENGTH);
      
      TriggerValue = InitialTriggerValue;
               
      memset(DigitizedData, 0, BUFFER_SIZE * 2);          
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
   
   NormalizeDataCounter = 0;
   NormalizeFlag = 0;
   CurrentIndex = 0;
}

unsigned int8 QuickDigitize(unsigned int16 ADCValue)
{
    InputIndex = CurrentIndex;
    CoefficentIndex = 0;
    Accumulator = 0;
    while (CoefficentIndex < COEF_LENGTH - 1)
      {
         Accumulator += (signed int32)ADCValue * (signed int32)fir_coef[CoefficentIndex];
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
      
    float StepOne = Accumulator - AverageAnalogValue;
    float StepTwo = StepOne * AverageMultiplier;
    float StepThree = StepTwo + (ADC_MAX_DATA_VALUE / 2);
    unsigned int8 ConversionValue = (unsigned int8)StepThree;
      
    return ConversionValue;  
}

void CommHandler(char UARTRX)
{
   switch (UARTRX)
   {
      case '+':
         HandShakeFlag = 1;
         break;
         
      case '*':
         disable_interrupts(INT_TIMER1);
         break;
         
      case 'D':
         enable_interrupts(INT_TIMER1);   
         setup_timer1(TMR_INTERNAL , TimerTicks);
         HandshakeFlag = 1;  
         break; 
                     
      case 'L':
         DisableInterrupts();
         memset(fir_coef, 0, COEF_LENGTH*2); 
         while (CSharpCoefficentRecieved != COEF_LENGTH)
         {
            if (kbhit(SHARP))
            {
               char CoefficentByte = fgetc(SHARP);
               
               if (CoefficentByte == 'L')
               {
                  ; // Do nothing
               }              
               else if (NumberCSharpByteRecieved == 0)
               {
                  CSharpCoefficent[0] = CoefficentByte; 
                  NumberCSharpByteRecieved = 1;
               }           
               else
               {
                  CSharpCoefficent[1] = CoefficentByte;
                  NumberCSharpByteRecieved = 0;
                  ByteConversionResult = ((unsigned int16)CSharpCoefficent[1] << 8) | CSharpCoefficent[0];
                  fir_coef[CSharpCoefficentRecieved] =  ByteConversionResult;
                  CSharpCoefficentRecieved++;
               }
            }
         }
         
         switch (fir_coef[0])
         {
         case 210: //Todo:: Fall Through 
            TimerTicks = 53334;
            break;
         case 40:
            TimerTicks = 53334;
            break;
//!                  case -12:
//!                     TimerTicks = 8000;
//!                     break;
//!                  case -9:
//!                     TimerTicks = 8000;
//!                     break;
         case 353:
            TimerTicks = 8000;
            break;
         default: 
            TimerTicks = 8000;
         }
         
         CSharpCoefficentRecieved = 0;
         HandshakeFlag = 1;
         EnableInterrupts();
         break;
        
      case '$':
         DisableInterrupts();
         TriggerValue = 0; // reset trigger value
         
         while (TRUE)
         {
             if (kbhit(SHARP))
            {
               char DigitByte = fgetc(SHARP);
               
               if (isdigit(DigitByte))
               {
                  TriggerValue = TriggerValue * 10 + CharToInt(DigitByte);
               }
               else if (DigitByte == ')')
               {
                  break;
               }
               else
               {
                  ; //Do nothing
               }
            }
         }
         setup_timer1(TMR_INTERNAL , TimerTicks);
         EnableInterrupts();
         break; 
         
      case '%':
         DisableInterrupts();  
         TimerTicks = 0; // reset trigger value
            
         while (TRUE)
         {
             if (kbhit(SHARP))
            {
               char DigitByte = fgetc(SHARP);
               
               if (isdigit(DigitByte))
               {
                  TimerTicks = TimerTicks * 10 + CharToInt(DigitByte);
               }
               else if (DigitByte == ')')
               {
                  break;
               }
               else
               {
                  ; //Do nothing
               }
            }
         }
         EnableInterrupts();
         break;
         
      default :
         ; // Do nothing 
      
   }
   
   UARTRX = '\0';
   UARTRXFlag = 0;
}

void DisableInterrupts(void)
{
   disable_interrupts(INT_TIMER1); // disable timer
   disable_interrupts(INT_RDA2);
   disable_interrupts(GLOBAL); 
}

void EnableInterrupts(void)
{
   enable_interrupts(INT_RDA2);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
}
