#include <16F1939.h>
#device ADC=16

#FUSES NOWDT                    // no Watch Dog Timer
#FUSES NOBROWNOUT               // no brownout reset
#FUSES NOLVP                    // no low voltage programming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal=20000000)  // external crystal clock

// I/O pin configurations
#use STANDARD_IO( B )
#use FIXED_IO( B_outputs=PIN_B4,PIN_B5 )
#use STANDARD_IO( D )
#use FIXED_IO( D_outputs=PIN_D3,PIN_D1 )

// Pin definitions
#define USER_LED     PIN_B4
#define COMMS_LED    PIN_B5
#define CS_PIN       PIN_D3 

#define PWM1_PIN     PIN_C2
#define PWM2_PIN     PIN_C1
#define PWM3_PIN     PIN_E0
#define PWM4_PIN     PIN_D1

