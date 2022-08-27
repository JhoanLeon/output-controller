#include <main.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define __DEBUG__

// Configuration of communication peripherals
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7,bits=8,parity=N)
#use SPI (MASTER, SPI1, BAUD=5000000, MODE=2, BITS=8, STREAM=SPI_1, MSB_FIRST)

// Define variables
unsigned int16 pwm_counter = 0; // counter of timer2 timebase
unsigned int16 desired_pwm_period = 6579; // 100ms

unsigned int16 desired_dutycycle_pwm1 = 25; // 0-100%
unsigned int16 desired_dutycycle_pwm2 = 50; // 0-100%
unsigned int16 desired_dutycycle_pwm3 = 75; // 0-100%
unsigned int16 desired_dutycycle_pwm4 = 100; // 0-100%

unsigned int16 desired_anout1_value = 0; // 0x0000 - 0xFFFF
unsigned int16 desired_anout2_value = 0; // 0x0000 - 0xFFFF

// Variables to UART communications
unsigned char char_received = 0; // variable to save chars received by UART

unsigned char rx_msg[20] = {0}; // buffer to receive UART messages
unsigned int8 index_rx_msg = 0; // index to manage the rx_msg buffer

unsigned char identifier_msg[10] = {0}; // buffer to receive UART messages
unsigned int8 index_identifier_msg = 0; // index to manage the identifier_msg buffer

unsigned int8 index_reader = 0; // to read received commands

char value_msg[5] = {0}; // buffer to save <value> messages
unsigned int8 index_value_msg = 0; // index to manage the value_msg buffer

char desired_pwm_channel = 0; // variable to save the desired channel in the command

// StateMachine to UART Protocol
enum UART_STATES
{
   ST_WAIT = 0,
   ST_A = 1,
   ST_MSG = 2
} uart_state;

// UART commands identifiers
char CMD_1[10] = "ANOUT";
char CMD_1_1[10] = "anout";
char CMD_2[10] = "PWMPERIOD";
char CMD_2_2[10] = "pwmperiod";  
char CMD_3[10] = "PWMDUTY";
char CMD_3_3[10] = "pwmduty";
char CMD_4[10] = "ABOUT";
char CMD_4_4[10] = "about";
char CMD_5[10] = "HELP";
char CMD_5_5[10] = "help";

// Flags to manage events
volatile int8 NEW_MSG_RECEIVED_FLAG = false;


#INT_TIMER2
/*
* ISR Function to generate PWM timebase
*/
void ISR_Timer_2(void)
{   
   // set pwm period
   if (pwm_counter == desired_pwm_period)
   {
      // reset the counter to get the desired period
      pwm_counter = 0;
   }
   else
   {
      // count timebase
      pwm_counter += 1;
   }
   
   // set pwm outputs
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm1)
   {
      // activate pwm1 output
      output_high(PWM1_PIN);
   }
   else
   {
      output_low(PWM1_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm2)
   {
      // activate pwm2 output
      output_high(PWM2_PIN);
   }
   else
   {
      output_low(PWM2_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm3)
   {
      // activate pwm3 output
      output_high(PWM3_PIN);
   }
   else
   {
      output_low(PWM3_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm4)
   {
      // activate pwm4 output
      output_high(PWM4_PIN);
   }
   else
   {
      output_low(PWM4_PIN);
   }
}


#INT_RDA
/*
* ISR Function to read UART messages
*/
void ISR_Receive_UART(void)
{
   char_received = getc(); // read the received byte
   
   switch(uart_state)
   {
      case ST_WAIT: // wait for AT command
      {
         if ( (char_received == 'A') || (char_received == 'a') )
         {
            // next state
            uart_state = ST_A;
         }
         else
         {
            // send error and continue in wait state
            printf("ERROR\r\n");
            uart_state = ST_WAIT;
         }
         
         break;
      };
      
      case ST_A: // wait for AT command
      {
         if (char_received == 'T' || char_received == 't')
         {
            // next state
            uart_state = ST_MSG;
         }
         else
         {
            // send error and return to wait state
            printf("ERROR\r\n");
            uart_state = ST_WAIT;
         }
         
         break;
      };
      
      case ST_MSG:
      {
         if (char_received == '\r') ///////// CHANGE THISSSSSS TO \n TO GET FINAL \r\n
         {
            // end of message flag
            NEW_MSG_RECEIVED_FLAG = true;
            // next state
            uart_state = ST_WAIT; 
         }
         else
         {
            // add character to buffer
            rx_msg[index_rx_msg] = char_received;
            index_rx_msg += 1;
            // next state
            uart_state = ST_MSG; 
         }
         
         break;
      };
      
      default:
      {
         printf("ERROR\r\n");
         break;
      };
   }
}


int32 char_hex_to_dec(char car, int pos)
{
   unsigned int16 ans = 0;
   unsigned int16 base = 0;
   unsigned int16 exp = 0;
   
   switch (car)
   {
      case '1': base = 1;
      break;
      
      case '2': base = 2;
      break;
      
      case '3': base = 3;
      break;
      
      case '4': base = 4;
      break;
      
      case '5': base = 5;
      break;
      
      case '6': base = 6;
      break;
      
      case '7': base = 7;
      break;
      
      case '8': base = 8;
      break;
      
      case '9': base = 9;
      break;
      
      case 'A': base = 10;
      break;
      
      case 'B': base = 11;
      break;
      
      case 'C': base = 12;
      break;
      
      case 'D': base = 13;
      break;
      
      case 'E': base = 14;
      break;
      
      case 'F': base = 15;
      break;
   }
   
   switch (pos)
   {
      case 0: exp = 1;
      break;
      
      case 1: exp = 16;
      break;
      
      case 2: exp = 256;
      break;
      
      case 3: exp = 4096;
      break;
   }
   
   ans = base*exp;
   
   return ans;
}


/* 
* Function to clear buffers
*/
void Clear_Buffers(void)
{
   for (int8 j = 0; j < index_rx_msg; j++)
   {
      rx_msg[j] = 0; 
   }
   index_rx_msg = 0; // reset the buffer index
   
   for (j = 0; j < index_identifier_msg; j++)
   {
      identifier_msg[j] = 0; 
   }
   index_identifier_msg = 0; // reset the buffer index
   
   for (j = 0; j < index_value_msg; j++)
   {
      value_msg[j] = 0; 
   }
   index_value_msg = 0; // reset the buffer index
}


#ifdef __DEBUG__
void Print_Buffers(void)
{
   // print the message received
   for (int8 k = 0; k < index_rx_msg; k++)
   {
      putc(rx_msg[k]); 
   }
   printf("\r\n");
   
   // print the identifier received
   for (k = 0; k < index_identifier_msg; k++)
   {
      putc(identifier_msg[k]); 
   }
   printf("\r\n");
   
   // print the value received
   for (k = 0; k < index_value_msg; k++)
   {
      putc(value_msg[k]); 
   }
   printf("\r\n");
}
#endif


void main()
{
   // Set initial state of IO pins
   output_low(USER_LED); // start with led turn off
   output_low(COMMS_LED); // start with led turn off
   output_high(CS_PIN); // start in high to disable the slave
   
   // Set initial state of PWM pins
   output_low(PWM1_PIN);
   output_low(PWM2_PIN);
   output_low(PWM3_PIN);
   output_low(PWM4_PIN);
   
   // Set initial 0V at DAC output channels
   
   
   // Configure TIM2 to PWM timebase
   setup_timer_2(T2_DIV_BY_1,75,1);      // 15,2 us overflow, 15,2 us interrupt
   
   // Configure interrupts
   enable_interrupts(INT_TIMER2); // timer 2 interrupt
   enable_interrupts(INT_RDA); // UART interrupt
   enable_interrupts(GLOBAL);
   
   // Start UART communication
   uart_state = ST_WAIT;
   printf("READY\r\n");

   while(TRUE)
   {
//!      output_low(USER_LED);
//!      output_high(COMMS_LED);
//!      delay_ms(500);
         
      if (NEW_MSG_RECEIVED_FLAG == true)
      {
         // manage the event
         if (rx_msg[0] == 0) // simple AT command
         {
            printf("OK\r\n"); 
         }
         else if (rx_msg[0] == '+') // complex AT command
         {
            // get the identifier of the received command
            for (int8 i = 1; i < index_rx_msg; i++)
            {
               if ( (rx_msg[i] == '=') || (rx_msg[i] == '?') )
               {
                  // end of command identifier
                  index_reader = i;
                  break;
               }
               else
               {
                  // read the command identifier
                  identifier_msg[index_identifier_msg] = rx_msg[i];
                  index_identifier_msg += 1;
               }
            }
            
            // select between commands
            ////// ANOUT Command //////
            if ( (strcmp(identifier_msg, CMD_1) == 0) || (strcmp(identifier_msg, CMD_1_1) == 0) ) 
            {
               printf(CMD_1);
            }
            
            ////// PWMPERIOD Command //////
            else if ( (strcmp(identifier_msg, CMD_2) == 0) || (strcmp(identifier_msg, CMD_2_2) == 0) ) 
            {
               if (rx_msg[index_reader] == '?') // case 2 of PWMPERIOD command
               {
                  // answer the command
                  printf("+PWMPERIOD:%X%X\r\n", (int)(desired_pwm_period>>8), (int)(desired_pwm_period));
                  printf("OK\r\n");
               }
               else if ((rx_msg[index_reader] == '=') && (rx_msg[index_reader+1] == '?')) // case 3 of PWMPERIOD command
               {
                  // answer the command
                  printf("+PWMPERIOD:%X%X\r\n", (int)(desired_pwm_period>>8), (int)(desired_pwm_period));
               }
               else if (rx_msg[index_reader] == '=') // case 1 of PWMPERIOD command
               {
                  // read the value
                  for (i = index_reader+1; i <= index_rx_msg; i++) ///////// CHANGE THISSSSSS TO JUST < TO GET FINAL \r\n
                  {
                     // add character to value buffer
                     value_msg[index_value_msg] = rx_msg[i];
                     index_value_msg += 1;
                  }
                  
                  // end of value (read buffer to get the number)
                  unsigned int16 temp_desired_pwm_period = 0;
                  for (i = 0; i < index_value_msg; i++)
                  {
                     temp_desired_pwm_period += char_hex_to_dec(value_msg[i],index_value_msg-2-i);
                  }
                  desired_pwm_period = temp_desired_pwm_period; // assig final value of PWM period
                  printf("OK\r\n");
               }
               else
               {
                  // the command is not recognized
                  printf("ERROR\r\n");
               }
            }
            
            ////// PWMDUTY Command //////
            else if ( (strcmp(identifier_msg, CMD_3) == 0) || (strcmp(identifier_msg, CMD_3_3) == 0) ) 
            {
               if (rx_msg[index_reader] == '?') // case 2 of PWMDUTY command
               {
                  // answer the command
                  printf("+PWMDUTY:1,%X%X,2,%X%X,3,%X%X,4,%X%X\r\n", (int)(desired_dutycycle_pwm1>>8), (int)(desired_dutycycle_pwm1), 
                                                                     (int)(desired_dutycycle_pwm2>>8), (int)(desired_dutycycle_pwm2),
                                                                     (int)(desired_dutycycle_pwm3>>8), (int)(desired_dutycycle_pwm3),
                                                                     (int)(desired_dutycycle_pwm4>>8), (int)(desired_dutycycle_pwm4));
                  printf("OK\r\n");
               }
               else if ((rx_msg[index_reader] == '=') && (rx_msg[index_reader+2] == '?')) // case 3 of PWMDUTY command
               {
                  // read the channel
                  desired_pwm_channel = rx_msg[index_reader+1];
                  
                  // answer the command
                  if (desired_pwm_channel == '1')
                  {
                     printf("+PWMDUTY1:%X%X\r\n", (int)(desired_dutycycle_pwm1>>8), (int)(desired_dutycycle_pwm1));
                  }
                  else if (desired_pwm_channel == '2')
                  {
                     printf("+PWMDUTY2:%X%X\r\n", (int)(desired_dutycycle_pwm2>>8), (int)(desired_dutycycle_pwm2));
                  }
                  else if (desired_pwm_channel == '3')
                  {
                     printf("+PWMDUTY3:%X%X\r\n", (int)(desired_dutycycle_pwm3>>8), (int)(desired_dutycycle_pwm3));
                  }
                  else if (desired_pwm_channel == '4')
                  {
                     printf("+PWMDUTY4:%X%X\r\n", (int)(desired_dutycycle_pwm4>>8), (int)(desired_dutycycle_pwm4));
                  }
                  else
                  {
                     // the command is not recognized
                     printf("ERROR\r\n");
                  }
               }
               else if (rx_msg[index_reader] == '=') // case 1 of PWMDUTY command
               {
                  // read the channel
                  desired_pwm_channel = rx_msg[index_reader+1];
                  
                  // read the value
                  for (i = index_reader+3; i <= index_rx_msg; i++) ///////// CHANGE THISSSSSS TO JUST < TO GET FINAL \r\n
                  {
                     // add character to value buffer
                     value_msg[index_value_msg] = rx_msg[i];
                     index_value_msg += 1;
                  }
                  
                  // end of value (read buffer to get the number)
                  unsigned int16 temp_desired_pwm_dutycyle = 0;
                  for (i = 0; i < index_value_msg; i++)
                  {
                     temp_desired_pwm_dutycyle += char_hex_to_dec(value_msg[i],index_value_msg-2-i);
                  }
                  
                  // answer the command
                  if (desired_pwm_channel == '1')
                  {
                     desired_dutycycle_pwm1 = temp_desired_pwm_dutycyle; // assig final value of PWM dutycycle
                  }
                  else if (desired_pwm_channel == '2')
                  {
                     desired_dutycycle_pwm2 = temp_desired_pwm_dutycyle; // assig final value of PWM dutycycle
                  }
                  else if (desired_pwm_channel == '3')
                  {
                     desired_dutycycle_pwm3 = temp_desired_pwm_dutycyle; // assig final value of PWM dutycycle
                  }
                  else if (desired_pwm_channel == '4')
                  {
                     desired_dutycycle_pwm4 = temp_desired_pwm_dutycyle; // assig final value of PWM dutycycle
                  }
                  else
                  {
                     // the command is not recognized
                     printf("ERROR\r\n");
                  }
                  
                  printf("OK\r\n");
               }
               else
               {
                  // the command is not recognized
                  printf("ERROR\r\n");
               }
            }
            
            ////// ABOUT Command //////
            else if ( (strcmp(identifier_msg, CMD_4) == 0) || (strcmp(identifier_msg, CMD_4_4) == 0) )
            {
               // answer the command
               printf("Version: 1.0\r\n");
               printf("Date: 26/08/2022\r\n");
               printf("OK\r\n");
            }
            
            ////// HELP command //////
            else if ( (strcmp(identifier_msg, CMD_5) == 0) || (strcmp(identifier_msg, CMD_5_5) == 0) ) 
            {
               // answer the command
               printf("AT  Comando de prueba\r\n");
               printf("AT+ANOUT  Para establecer salidas analogas\r\n");
               printf("AT+PWMPERIOD  Para establecer periodo de canales PWM\r\n");
               printf("AT+PWMDUTY  Para establecer ciclo de trabajo de canales PWM\r\n");
               printf("AT+ABOUT  Para mostrar informacion del producto\r\n");
               printf("AT+HELP  Para mostrar lista de comandos\r\n");
               printf("OK\r\n");
            }
            
            else
            {
               // the identifier of the command is not recognized
               printf("ERROR\r\n");
            }
            
            #ifdef __DEBUG__
            Print_Buffers(); // to view buffer's info in debugging
            #endif
         }
         else
         {
            // the structure of the command is not correct
            printf("ERROR\r\n");
         }
         
         // clear the buffers
         Clear_Buffers();
         // clear the flag
         NEW_MSG_RECEIVED_FLAG = false;
      }
      else
      {
         // do nothing
      }
   }
}
