#include <main.h>
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)
#use SPI (MASTER, SPI1, BAUD=5000000, MODE=2, BITS=8, STREAM=SPI_1, MSB_FIRST)


unsigned int16 pwm_counter = 0;
unsigned int16 desired_pwm_period = 6579; // xxs
unsigned int16 desired_dutycycle_pwm1 = 0; // 0-100%
unsigned int16 desired_dutycycle_pwm2 = 50; // 0-100%
unsigned int16 desired_dutycycle_pwm3 = 75; // 0-100%
unsigned int16 desired_dutycycle_pwm4 = 100; // 0-100%


#INT_TIMER2
/*
* ISR function to generate PWM timebase
*/
void timer_2_timebase(void)
{
   output_toggle(CS_PIN);
   
   // set period
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
      // activate in high pwm1 output
      output_high(PWM1_PIN);
   }
   else
   {
      output_low(PWM1_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm2)
   {
      // activate in high pwm2 output
      output_high(PWM2_PIN);
   }
   else
   {
      output_low(PWM2_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm3)
   {
      // activate in high pwm3 output
      output_high(PWM3_PIN);
   }
   else
   {
      output_low(PWM3_PIN);
   }
   
   if (pwm_counter < (desired_pwm_period/100)*desired_dutycycle_pwm4)
   {
      // activate in high pwm4 output
      output_high(PWM4_PIN);
   }
   else
   {
      output_low(PWM4_PIN);
   }
}


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
   
   // Configure TIM2 to PWM timebase
   setup_timer_2(T2_DIV_BY_1,76,1);      // 15,2 us overflow, 15,2 us interrupt
   
   enable_interrupts(INT_RDA); // UART interrupt
   enable_interrupts(INT_TIMER2); // timer 2 interrupt
   enable_interrupts(GLOBAL);

   // Configure/set initial 0V at DAC output channels

   while(TRUE)
   {
//!      output_low(USER_LED);
//!      output_high(COMMS_LED);
//!      delay_ms(500);
//!      output_high(USER_LED);
//!      output_low(COMMS_LED);
//!      delay_ms(500);
//!      output_toggle(CS_PIN);
   }
}
