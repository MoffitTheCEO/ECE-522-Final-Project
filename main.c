#include "main.h" // include header file

#use delay(clock = 32MHZ, internal = 8MHZ) // Slow down clock

#BANK_DMA
unsigned int16 DMA_ADC_BUFFER[BUFFER_SIZE]; // Banked DMA buffer stored in memory

#INT_DMA0
void  DMA_0_ISR(void)  // DMA Handler
{
   if ((TriggerFlag == 2) || (TriggerValueFlag == 0)) // when trigger value found of normlization flag is high 
   {
      DMADoneFlag = 1; // set DMA Done Flag 
   }
}


#INT_RDA2
void UART2_ISR() // UART Handler 
{
   UARTRX = fgetc(SHARP); // read char from UART RX buffer
   UARTRXFlag = 1; // set UART RX Flag
}

#INT_TIMER1
void Timer_ISR() // Timer Handler 
{
   output_toggle(LED_PIN); // Toggle LED
 
   if((NormalizeFlag == 1) || (TriggerValueFlag == 0)) // If nomalize flag high or inital trigger value find
   {
      read_adc(); // read ADC 
      NormalizeDataCounter++; // increment data aquired counter 
   }
   else
   {
      unsigned int16 ADCValue = 0;
      
      if (DMAFlag == 0) // If dma flag is reset 
      {
         memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE * 2); // clear DMA buffer
         TriggerFlag = 0; // reset trigger flag
         DMAFlag = 1; // ser dma flag 
      }
      
      if (TriggerFlag != 2) // if trigger value has not been found 
      {
           
           if(DMAFlag != 2) // is dma has not been stated 
           {
              dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE); // start dma 
              enable_interrupts(INT_DMA0); // enable DMA interrupts
           }
  
           ADCValue = read_adc() >> 4; // read ADC and shift data to be 8 bits 
      }
      
      if((ADCValue == TriggerValue) && (TriggerFlag == 0)) // If trigger value is found 
      {
         TempInputSamples[0] = ADCValue; // store ADC Value into temp array
         DMAFlag = 2; // signal syste m that DMA IS started
         TriggerFlag = 1; // move onto next step of trigger value detections
      }
      else if((ADCValue > TempInputSamples[0]) && (TriggerFlag == 1)) // if ADC Value is greater then trigger value
      {
         TempInputSamples[1] = ADCValue; // store ADC Value into temp array
         TriggerFlag = 2; // signal system that trigger value has been found 
      }
      else if(TriggerFlag == 2) // if trigger value has been found 
      {    
         read_adc(); // run ADC DMA conversion until DMA buffer is full
      }  
      else
      {
         DMAFlag = 0; // If no trigger value found reset and start over      
      }
   }
}

void main()
{   
   memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE); // clear DMA buffer 
   
   setup_dma(ADC_DMA_CHANNEL, DMA_IN_ADC1, DMA_WORD); // set up DMA 
   dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE); // Start DMA 
   
   enable_interrupts(INT_DMA0); // Enable DMA interrupt
   
   setup_adc(ADC_CLOCK_INTERNAL); // set adc clock internal 
   setup_adc_ports(sAN0 | VSS_VDD); // set adc port and refernce voltage
   read_adc(); // read ADC to kickstart DMA tranfer
   
   setup_timer1(TMR_INTERNAL , TimerTicks); // set up timer clock and timer ticks
   EnableInterrupts(); // enable interrupts 
   
   NormalizeFlag = 1; // System needs to normalize data 

   while(TRUE) // loop runs forever 
   {
      if((DMADoneFlag) || (NormalizeDataCounter == BUFFER_SIZE)) // if DMA buffer full or Normlization data is acquired
      {
         disable_interrupts(INT_DMA0); // disable DMA interrupt

         for (IndexType Index = 0; Index < BUFFER_SIZE; Index++)
         {
            AccumulateAnalogData(Index); // Filter data and store into array 
         }
         
         if (NormalizeFlag == 1) // if normalizaon data is present
         {
            NormalizeData(); // normalize data
         }
            
         if (HandShakeFlag == 1) // if C# GUI is ready for more data 
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
         //reset flags and enable DMA interrupt
         NormalizeFlag = 0;
         NormalizeDataCounter = 0;
         HandShakeFlag = 0;   
         CurrentIndex = 0;
         enable_interrupts(INT_DMA0);
         DMADoneFlag = 0;
         TriggerFlag = 0;
         DMAFlag = 0;
      }  
      
      if (UARTRXFlag) // if UART data is recieved
      {
         CommHandler(UARTRX); // Comm Handler will deal with UART RX data 
      }  
   }
}

void AccumulateAnalogData(IndexType DMAADCIndex)
{
   InputSamples[CurrentIndex] = DMA_ADC_BUFFER[DMAADCIndex]; // fill input samples array with current DMA buffer data 
   //Filter provided by Dr. Zheng 
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
   //Filter provided by Dr. Zheng  
   
   AnalogData[DMAADCIndex] =  InputSamples[CurrentIndex] >> 4; // store 12 bit input data into 8 bit array by shifting by 4 
   
   if (NormalizeFlag == 1) // if normalizing data 
   {
      DigitizedData[DMAADCIndex] = Accumulator; // store accumulator data into output array
   }
   else
   {
      // Math to go get 8bit digitzed output data 
      float StepOne = Accumulator - AverageAnalogValue;
      float StepTwo = StepOne * AverageMultiplier;
      float StepThree = StepTwo + (ADC_MAX_DATA_VALUE / 2);
      ConversionValue = (unsigned int8)StepThree;
      DigitizedData[DMAADCIndex] = ConversionValue; // store output data into output array
   }
   //Filter provided by Dr. Zheng 
   if (CurrentIndex == 0)
   {
     CurrentIndex = COEF_LENGTH - 1;
   }
   else
   {
     CurrentIndex--;
   } 
   //Filter provided by Dr. Zheng 
}

void NormalizeData(void)
{
   // initial data points for normlization algorithm
   MaxAnalogValue = DigitizedData[COEF_LENGTH]; 
   MinAnalogValue = DigitizedData[COEF_LENGTH]; 
   InitialTriggerValue = DMA_ADC_BUFFER[COEF_LENGTH];
      
   for (IndexType Index = COEF_LENGTH + 1 ; Index < BUFFER_SIZE; Index++) // loop through digitized output array start at index COEF_LENGTH + 1 
   {
      // Finding the Min Value 
      if (MinAnalogValue > DigitizedData[Index])
      {
         MinAnalogValue = DigitizedData[Index];
      }
      // Finding the Max Value
      if (MaxAnalogValue < DigitizedData[Index])
      {
         MaxAnalogValue = DigitizedData[Index];
      }
      // Finding inital trigger value
      if (InitialTriggerValue > DMA_ADC_BUFFER[Index])
      {
         InitialTriggerValue = DMA_ADC_BUFFER[Index];
      }
      // sum up all the data in the digitzed output array that has been looped through
      AverageAnalogValue = AverageAnalogValue + DigitizedData[Index];
   }
   // Math to find Average data value
   AverageDivider = MaxAnalogValue - MinAnalogValue;
   AverageMultiplier = (255.0/ AverageDivider);
   AverageAnalogValue = AverageAnalogValue / (BUFFER_SIZE - COEF_LENGTH);
   // Trigger value will equal inital trigger value at program start
   TriggerValue = InitialTriggerValue;
            
   memset(DigitizedData, 0, BUFFER_SIZE * 2); // clear DMA buffer         
   // reset flags
   NormalizeDataCounter = 0;
   NormalizeFlag = 0;
   CurrentIndex = 0;
}

void CommHandler(char UARTRX)
{
   switch (UARTRX)
   {
      case '+': // Handshake 
         HandShakeFlag = 1; // turn handshake flag high
         break;
         
      case '*': // stop timer 
         disable_interrupts(INT_TIMER1); // disable timer interrupt
         break;
         
      case 'D': // start sampling 
         enable_interrupts(INT_TIMER1); // enable timer interrut   
         setup_timer1(TMR_INTERNAL , TimerTicks); // set up timer 
         HandshakeFlag = 1; // set handshake flag
         break; 
                     
      case 'L': // new coefficents incoming 
         DisableInterrupts(); // disable interrupts  
         memset(fir_coef, 0, COEF_LENGTH*2); // clear coefficent buffer 
         while (CSharpCoefficentRecieved != COEF_LENGTH) // while 64 coefficent have not been revied
         {
            if (kbhit(SHARP)) // if new data incoming 
            {
               char CoefficentByte = fgetc(SHARP); // read new data 
               
               if ((CoefficentByte == 'L') && (CSharpCoefficentRecieved == 0)) // error detection
               {
                  ; // Do nothing
               }              
               else if (NumberCSharpByteRecieved == 0) // take first coefficent data byte
               {
                  CSharpCoefficent[0] = CoefficentByte; // store in array 
                  NumberCSharpByteRecieved = 1; // tell system to get ready for second coefficent data
               }           
               else // take second coefffincet data byte
               {
                  CSharpCoefficent[1] = CoefficentByte; // store in array 
                  NumberCSharpByteRecieved = 0; // reset coefficent byte number
                  ByteConversionResult = ((unsigned int16)CSharpCoefficent[1] << 8) | CSharpCoefficent[0]; // manipulate data to form one 16bit data 
                  fir_coef[CSharpCoefficentRecieved] = ByteConversionResult; // store coefficent into coefficent buffer
                  CSharpCoefficentRecieved++; // increment number of coefficent recieved
               }
            }
         }
         
         switch (fir_coef[0]) // find out timer frequnecy by looking for unique first element in coefficnent buffer 
         {
         // Timer ticks will take on differnet value depending on coefficent data
         case 210: 
            TimerTicks = 53334;
            break;
         case 40:
            TimerTicks = 53334;
            break;
         case 353:
            TimerTicks = 8000;
            break;
         default: 
            TimerTicks = 8000;
         }
         //reset/set flags
         CSharpCoefficentRecieved = 0;
         NormalizeFlag = 1;
         HandshakeFlag = 1;
         EnableInterrupts(); // enable interrupts
         break;
        
      case '$': // trigger value incoming
         DisableInterrupts(); // disable interrupts 
         TriggerValue = 0; // reset trigger value
         
         while (TRUE) // infite loop
         {
             if (kbhit(SHARP)) // if new data 
            {
               char DigitByte = fgetc(SHARP); // get new data
               
               if (isdigit(DigitByte)) // check to see if data is digit
               {
                  TriggerValue = TriggerValue * 10 + CharToInt(DigitByte); // manipulte incoing data into a trigger value
               }
               else if (DigitByte == ')') // if escape data recived 
               {
                  break; // break out of loop
               }
               else
               {
                  ; //Do nothing
               }
            }
         }
         // reset/set flags 
         TriggerValueFlag = 1;
         setup_timer1(TMR_INTERNAL , TimerTicks); // start timer 
         EnableInterrupts(); // enable interupts
         break; 
         
      case '%': // Timer Ticks incoming
         DisableInterrupts();  // disable interrupts 
         TimerTicks = 0; // reset trigger value
            
         while (TRUE) // infinite loop
         {
             if (kbhit(SHARP))  // if new data 
            {
               char DigitByte = fgetc(SHARP);  // get new data
               
               if (isdigit(DigitByte)) // check to see if data is digit
               {
                  TimerTicks = TimerTicks * 10 + CharToInt(DigitByte); // manipulte incoing data into a timer ticks
               }
               else if (DigitByte == ')') // if escape data recived 
               {
                  break; // break out of loop
               }
               else
               {
                  ; //Do nothing
               }
            }
         }
         // reset/set flags
         HandshakeFlag = 1;
         EnableInterrupts(); // enable interupts
         break;
           
      case '#': // No trigger value needed
         memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE * 2); // clear DMA buffer
         dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE); // start DMA 
         enable_interrupts(INT_DMA0); // enable DMA intterupt
         //set/reset flags
         DMAFlag = 0;
         TriggerFlag = 0;
         TriggerValueFlag = 0;
         HandshakeFlag = 1;
         break;
         
      case '@':// Max Sampling Frequency 
         DisableInterrupts();// disable interrupts 
         memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE * 2); // clear DMA buffer
         dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE); // start DMA 
         enable_interrupts(INT_DMA0); // enable DMA intterupt
         enable_interrupts(GLOBAL); // enable intterupt
         HandshakeFlag = 1; // set handshake flag to allow for first data tranfer
         unsigned int8 TXData;
         while(TRUE) // infinite loop
         {
            output_toggle(LED_PIN); // toggle adc 
            read_adc(); // trigger ADC DMA Conversion
            
            if((DMADoneFlag == 1) && (HandshakeFlag == 1)) // if DMA Buffer full and C# GUI ready for more data 
            {
               for (IndexType i = 0; i < BUFFER_SIZE; i++) // loop through DMA buffer
               {
                   TXData = DMA_ADC_BUFFER[i] >> 4; // shift 12bit data to fit into 8bit char 
                   printf("%c", TXData); // send every emelent of the array as a byte
               }
               //reset flags
               DMADoneFlag = 0;
               HandshakeFlag = 0;
            }
            
            if (kbhit(SHARP)) // if UART RX data 
            {
               char RXData = fgetc(SHARP); // get data 
               
               if (RXData == '+') // if handshake sent from GUI
               {
                  HandshakeFlag = 1; // set handshake flag 
               }
               else if (RXData = '*') // start sent from GUI 
               {
                  EnableInterrupts(); // enable interrupts
                  break;
               }
               
               RXData = '\n'; // reset RXData
            }
         }
         break;
  
      default :
         ; // Do nothing 
      
   }
   //Reset flags and UARTRX
   UARTRX = '\0';
   UARTRXFlag = 0;
}

void DisableInterrupts(void)
{
   disable_interrupts(INT_TIMER1); // disable timer
   disable_interrupts(INT_RDA2); // disable UART interrupts
   disable_interrupts(GLOBAL);  // disable global interrupts
}

void EnableInterrupts(void)
{
   enable_interrupts(INT_RDA2); // enable UART interrupts
   enable_interrupts(INT_TIMER1); // enable timer interrupts
   enable_interrupts(GLOBAL); // enable global interrupts
}
