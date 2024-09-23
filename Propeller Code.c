#include "simpletools.h" // Include simpletools
#include "keypad.h"
#include "datetime.h"
#include "ping.h" 
#define OUTPUT 0            // Define OUTPUT constant as HIGH state

#define RELAY_PIN 9        // Digital output pin connected to relay control (e.g., P0)
#define MOTOR_ON_DELAY 5000  // Delay time for motor to turn on in milliseconds
#define MOTOR_OFF_DELAY 500 // Delay time for motor to turn off in milliseconds

unsigned int stack1[40 + 25];
unsigned int stack2[40 + 25]; 
unsigned int stack3[40 + 25];



serial *lcd;
const int ON = 22;
const int CLR = 12;
struct datetime_st dt = {2015, 9, 25, 8, 11, 00};
volatile int note[]={1000, 1100, 1300, 1500, 1700, 1800, 2100};
volatile int rows[4] = {7, 6, 5, 4};                   // Row I/O pins (top to bottom)
volatile int cols[4] = {3, 2, 1, 0};                   // Column I/O pins (left to right)

volatile int values[16] = {  1,   2,    3,  'A',       // Values for each key in the 4x4
                    4,   5,    6,  'B',
                    7,   8,    9,  'C',
                  '*',   0,  '#',  'D' };
volatile int key;
volatile int key1;
volatile int button;
volatile int distance;
static volatile int t =0 ;
int *cog1;
int *cog2;
void PING(void *par);
void LDR(void *par);
int main() // main function
{
lcd = serial_open(12, 12, 0, 9600);
serial_txChar(lcd,12);
serial_txChar(lcd,22);
serial_txChar(lcd,17);
keypad_setup(4, 4, rows, cols, values); 
printf("Set Time");
dprint(lcd,"Set Time");
setDT();
int cog1 = cogstart(&PING, NULL, stack1, sizeof(stack1));
int cog2 = cogstart(&LDR,NULL,stack2,sizeof(stack2));
while(1) // Endless loop
{
button = input(8); // P3 input -> button variable
printf("%d:%d:%d\n", dt_get().h, dt_get().m, dt_get().s);
dprint(lcd, "%d:%d\n", dt_get().h, dt_get().m);

if (button==1){
  key=keypad_getNumber();
  key1=keypad_getNumber();
}

serial_txChar(lcd, 10);
printf("Alarm - %d:%d\n", key,key1);
dprint(lcd, "Alarm - %d:%d\n", key,key1);
if(dt_get().h==key && dt_get().m == key1){
  while(distance >= 8 && t == 0) // put ultrasonic distance condition here
  { 
  printf("distance%d",distance);
  printf("BRO I AM RUNNING");
  for(int i=0; i<sizeof(note)/sizeof(note[0]); i++)
{
freqout(10, 1000, note[i]); // pin, duration, frequency
//print("%d \n",note[i]);
pause(68);
}
}
        high(RELAY_PIN);
        print("Motor ON\n");

        // Wait for the motor to be on
        pause(MOTOR_ON_DELAY);

        // Turn off motor
        low(RELAY_PIN);
        print("Motor OFF\n");

        // Wait for the motor to be off
        pause(MOTOR_OFF_DELAY);
t =0;
key =0;
key1 = 0;

}
pause(600); // Wait 0.1 second before repeat
serial_txChar(lcd,12);
}
}
void PING(void *par)

{ while(1)
 {
  distance = ping_cm(13);
  pause(50);
  if ( distance <8 )
  {
    t = 1;
  }   
}
}


void LDR(void *par){
  while(1){
while(1) // Endless loop
{ high(15); // Set P5 high
pause(1); // Wait for circuit to charge
int r = rc_time(15, 1); // Measure decay time on P5; // Display decay time
if(r>300){
  high(14); 
  }
 else{
    low(14);
    }
pause(100); // Wait 1/10th of a second
}
}
 }
  
void setDT(){
dt.h = keypad_getNumber();// Get pressed key (or -1 for none)
dt.m= keypad_getNumber();// Get pressed key (or -1 for none)
      
      //print("%d:%d\n", key, key1);               // If <= 9, dispaly as decimal 
   // }       
dt_set(dt);
dt_run(dt);
}