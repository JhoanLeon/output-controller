#include <main.h>
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)
#use SPI (MASTER, SPI1, BAUD=5000000, MODE=2, BITS=8, STREAM=SPI_1, MSB_FIRST)

void main()
{
   output_low(USER_LED); // start with led turn off
   output_low(COMMS_LED); // start with led turn off
   output_high(CS_PIN); // start in high to disable the slave
   
   setup_timer_2(T2_DIV_BY_1,50,1);      //10,2 us overflow, 10,2 us interrupt

   setup_ccp1(CCP_PWM|CCP_SHUTDOWN_AC_L|CCP_SHUTDOWN_BD_L);
   setup_ccp2(CCP_PWM|CCP_SHUTDOWN_AC_L|CCP_SHUTDOWN_BD_L);
   setup_ccp3(CCP_PWM|CCP_SHUTDOWN_AC_L|CCP_SHUTDOWN_BD_L);
   setup_ccp4(CCP_PWM);
   setup_ccp5(CCP_PWM);
   set_pwm1_duty((int16)0);
   set_pwm2_duty((int16)0);
   set_pwm3_duty((int16)0);
   set_pwm4_duty((int16)0);
   set_pwm5_duty((int16)0);

   while(TRUE)
   {
      //TODO: User Code
      output_low(USER_LED);
      output_high(COMMS_LED);
      delay_ms(500);
      output_high(USER_LED);
      output_low(COMMS_LED);
      delay_ms(500);
      output_toggle(CS_PIN);
   }
}
