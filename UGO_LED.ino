//Jan 31 2016

//Ok so this is the script for the lights that moves along the arm.
//you only have to input 2 variables in terms of what you want to display
//the first variable is the width of the illuminated segment, the width is in number of LEDs and it MUST BE A ODD NUMBER
//the maths portion of my code are all in uint16_t so if a even number of width is applied, the adresses outputted to the LEDs will be wrong
//then you have to set the color of the moving segment, right now its set as green
//there is a variable down there called "defaultColor", this is a array of 3 elements, the elements being {red, green, blue}
//the values supplied must be between 0 and 255, 255 being the brightest.
//the LED in the middle of the lit strip will have exactly that color 
//all the other LEDs on the left anf right will be the same color but slightly less bright.
//you ofcourse have to set the 4 digital pins used. only one is output, the rest is input
//in the preprocessor directives, "PIN" is the output, "BOOL1" "BOOL2" "BOOL3" are the input pins

//This is the current configuration of input pins:
/*
 * BOOL1 BOOL2 BOOL3     Output
 *   0     0     0       Hold/Do Nothing
 *   0     0     1       location 1
 *   0     1     0       location 2
 *   0     1     1       location 3
 *   1     0     0       location 4
 *   1     0     1       location 5
 *   1     1     0       location 6
 *   1     1     1       location 7  
 */
 
//All of the locations can be adjusted in the function "boolInput"
//the variable called "nextLocation" represents next location its supposed to go to.
//your LED strip has 60 LEDs in total, so locations represent the index of those LEDs, from 0 to 59 in consecutive order on the strip.

//the only set-up you have to do is to upload this script to another arduino, connect the digital output to the input on the strip.
//If you are only going to run this script then you probably can power the entire strip from the arduino, this script will not draw enough current to reset the arduino
//but say for example, you set every led to {255, 255, 255} then it will need a massive current since that's 180 LEDs being lit at the maximum intensity so it most likely will draw too much current
//to test the strip is as easy as feeding 5v to the input pins to input different locations to go to.





#include <Adafruit_NeoPixel.h>        //you have to install the adafruit neopixel library, download it here: https://github.com/adafruit/Adafruit_NeoPixel/archive/master.zip
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6                         //digital output pin
#define BOOL1 8                       //first digital input pin
#define BOOL2 9                       //seconhd digital input pin
#define BOOL3 10                      //third digital input pin

#define TOTAL_LED 60                  //total number of LEDs has been defined as 60, i saw that from the amazon link that the strip only has 60, if it's different you can change it, the code works for any length of LEDs

//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LED, PIN, NEO_GRB + NEO_KHZ800);    //I have set up the connection to the WS2812 LEDs, according to the amazon link

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//that was the warning from adafruit, some strips of WS series LEDs do not have the resistor or capacitor on them BUT MOST DO, atleast most i've seen do, just look on the flex, there should be SMD resistors and caps on it for every LED



//here are the variables you can set:
const int width = 9;                      //width of the strip of LED thats lit, WIDTH MUST BE A ODD NUMBER, even numbers will give invalid address
uint16_t defaultColor [3] = {150,50,80};     //Array to hold the default Color, its set as {red, green, blue} and each can have a value between 0 and 255





uint16_t location = 0;
uint16_t nextLocation =0;

bool standBy = true;                      //bool to hold if the system is in standby, currently standby does not do anything

uint16_t litStrip[width];
uint16_t red[width];
uint16_t green[width];
uint16_t blue[width];
double gradientArray[width];



void setColor()
{
  for(int c = 0; c<width; c++)
  {
    red[c] = defaultColor[0] * gradientArray[c];
    blue[c] = defaultColor[2] * gradientArray[c];
    green[c] = defaultColor[1] * gradientArray[c];
  }
}

void setGradientArray()
{
  for(int g = 0; g < (((width-1)/2)); g++)
  {
    gradientArray[g] = (g+1.0)/((width+1)/2);
    gradientArray[width-g-1] = (g+1.0)/((width+1)/2);
  }
  gradientArray[(width-1)/2] = 1;
}


void locationToArray (uint16_t location)
{

  int distCenterToEdge = (width-1)/2;
  
  for(int i= 0; i <width ; i++)
  {
      int temp = location - distCenterToEdge + i;
      
      if (temp < 0)
        temp = temp + TOTAL_LED;
      else if(temp > TOTAL_LED-1)
        temp = temp - TOTAL_LED; 

      litStrip[i] = temp;
  }
} 

/*
 * The follwing function changes the behavior of the LEDs based on the input:
 * after each if/elseif statement, you can change it do do pretty much anything
 * right now the first combination of input is standby
 * every other input combination is an location you can change
 */

void boolInput(bool inputWire1, bool inputWire2, bool inputWire3)
{
  if (!inputWire1 && !inputWire2 && !inputWire3)        //000
  standBy=true;

  else if(!inputWire1 && !inputWire2 && inputWire3)     //001
  nextLocation=0;

  else if(!inputWire1 && inputWire2 && !inputWire3)     //010
  nextLocation=8;

  else if(!inputWire1 && inputWire2 && inputWire3)      //011
  nextLocation=16;

  else if(inputWire1 && !inputWire2 && !inputWire3)     //100
  nextLocation=24;

  else if(inputWire1 && !inputWire2 && inputWire3)      //101
  nextLocation=32;

  else if(inputWire1 && inputWire2 && !inputWire3)      //110    
  nextLocation=40;

  else if(inputWire1 && inputWire2 && inputWire3)       //111
  nextLocation=50;

  else
  standBy = true;
}




void setup() 
{
  pinMode(BOOL1, INPUT);
  pinMode(BOOL2, INPUT);
  pinMode(BOOL3, INPUT);
  
  //Serial.begin(9600);               //uncomment to test inputs
  strip.begin();
  strip.show();
  setGradientArray();
  setColor();
}

void loop() 
{
  strip.clear();
  bool inputWire1 = ((digitalRead(BOOL1) == HIGH)? true:false);
  bool inputWire2 = ((digitalRead(BOOL2) == HIGH)? true:false);
  bool inputWire3 = ((digitalRead(BOOL3) == HIGH)? true:false);
  //Serial.print(location);           //uncomment to see current location
  //Serial.print("\t");
  //Serial.print(nextLocation);       //uncomment to see next location
  //Serial.print("\t");
  //Serial.print(inputWire1);         //uncomment to see input into BOOL1
  //Serial.print(inputWire2);         //uncomment to see input into BOOL2
  //Serial.println(inputWire3);       //uncomment to see input into BOOL3
  boolInput(inputWire1, inputWire2, inputWire3);

  if(nextLocation != location)
  {
    if(nextLocation < location)
    location--;
    if(nextLocation > location)
    location++;
  }
  
  for(int i = 0; i < width; i++)
  {
    setColor();
    locationToArray(location);
    strip.setPixelColor(litStrip[i], red[i],green[i],blue[i]);
  }
  
  strip.show();
  delay(100);                         //adjust delay to change speed, i can implement a couple of changes to get variable speed too if you need it
}

