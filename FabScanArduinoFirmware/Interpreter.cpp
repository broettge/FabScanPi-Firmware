

// Parts of the follwing Code are from the 2 AXIS CNC Demo Code ( see info below)
//------------------------------------------------------------------------------
// 2 Axis CNC Demo v2 - supports Adafruit motor shields v1 and v2
// dan@marginallycelver.com 2013-08-30
//------------------------------------------------------------------------------
// Copyright at end of file.
// please see http://www.github.com/MarginallyClever/GcodeCNCDemo for more information.

#include "FabScanConfig.h"
#include "Interpreter.h"
#include "MotorDriver.h"
#include "LaserDriver.h"

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LIGHT_PIN, NEO_GRBW + NEO_KHZ800);

void set_leds(int r, int g, int b, int w){
  for(int i=0; i<NUMPIXELS; i++){
    pixels.setPixelColor(i,pixels.Color(r,g,b,w));
  }
  pixels.show();
  delay(100);
}


void initialize_led_driver(){
	pixels.begin();
  //set_leds(0,0,0,0);
}


char serialBuffer[MAX_BUF];  // where we store the message until we get a ';'
int sofar;  // how much is in the buffer
long line_number=0;


float px, py;  // location

// speeds
float fr=10000;  // human version
// machine version
long step_delay_ms;
long step_delay_us;
// settings
char mode_abs=0;  // absolute mode??



/**
 * remove any leading or trailing spaces from a string.
 **/
void strTrim(char* str) {
    int start = 0; // number of leading spaces
    char* buffer = str;
    while (*str && *str++ == ' ') ++start;
    while (*str++); // move to end of string
    int end = str - buffer - 1; 
    while (end > 0 && buffer[end - 1] == ' ') --end; // backup over trailing spaces
    buffer[end] = 0; // remove trailing spaces
    if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
    str = buffer + start;
    while ((*buffer++ = *str++));  // remove leading spaces: K&R
}



/**
 * Look for character /code/ in the buffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
long parseNumber(char code, int val) {
  char *ptr=serialBuffer;  // start at the beginning of buffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atol(ptr+1);  // convert the digits that follow into a long int and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}

/**
 * Look for character /code/ in the buffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
float parseFloat(char code, float val) {
  char *ptr=serialBuffer;  // start at the beginning of buffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atof(ptr+1);  // convert the digits that follow into a float and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}

/**
 * write a string followed by a float to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the float.
 */
void output(const char *code,float val) {
	Serial.print(code);
	Serial.println(val);
}

/**
 * display helpful information
 */
void help() {
	Serial.print(F("FabScan G-CODDE Interpreter Version: "));
	Serial.println(BUILD_VERSION);
	Serial.println(F("Commands:"));
	Serial.println(F("G00 [T(steps)]; - linear move"));
	Serial.println(F("G01 [T(steps)] [L(steps)] [F(feedrate)]; - move"));
  Serial.println(F("M05 [R(value)] [G(value)] [B(value)] [W(value)] ; - set NeoPixel LED values"));
	Serial.println(F("M17; - enable motors"));
	Serial.println(F("M18; - disable motors"));
	Serial.println(F("M19; - turn left laser on"));
	Serial.println(F("M20; - turn left laser off"));
	Serial.println(F("M21; - turn right laser on"));
	Serial.println(F("M22; - turn right laser off"));
	Serial.println(F("M100; - this help message"));
	Serial.println(F("M114; - report position and feedrate"));
}

void version(){
	Serial.println(BUILD_VERSION);
}

void board(){
  Serial.println(MOTHERBOARD);
}

void settingsmode(int state){
  if (state) {
      right_laser_on();
      start_turning();
  } else {
      right_laser_off();
      stop_turning();
  }
}

/**
 * Read the input buffer and find any recognized commands.  One G or M command per line.
 */
void processCommand() {
  strTrim(serialBuffer);
  
  if (serialBuffer[0] == 'G'){
  	int cmd = parseNumber('G',-1);
  	switch(cmd) {
  		case  0: // move linear
  		case  1: // move linear
  			do_move(parseNumber('T',0), parseNumber('L',0), parseFloat('F',0), TURN_NON_BLOCKING);
  			break;
  		case 2: 
  			do_move(parseNumber('T',0), parseNumber('L',0), parseFloat('F',0), TURN_BLOCKING);
  		break;
  		case 6:
  			start_turning();
  			break;
  		case 7:
  			stop_turning();
  			break;
        
  		default:  break;
  	}
  }
  else if (serialBuffer[0] == 'M'){
  	int cmd = parseNumber('M',-1);
  	switch(cmd) {
  		case 4:
  			right_laser_off();
        		left_laser_off();
  			set_leds(parseNumber('R',0),parseNumber('G',0),parseNumber('B',0),parseNumber('W',0));
  			break;
  		case 5:
  			set_leds(parseNumber('R',0),parseNumber('G',0),parseNumber('B',0),parseNumber('W',0));
  			break;
      case 13: 
        turntable_motor_enable();
        break;
      case 14: 
        turntable_motor_release();
        break;
  		case 17:  // enable motors
  			turntable_motor_enable();
  			break;
  		case 18:  // disable motors
  			turntable_motor_release();
  			break;
  		case 19:
  			left_laser_on();
  			break;
  		case 20:
  			left_laser_off();
  			break;
  		case 21:
  			right_laser_on();
  			break;
  		case 22:
  			right_laser_off();
  			break;
  		case 100:  help();  break;
  		case 200:  version(); break;
      case 201:  board(); break;
  		default:  break;
  	}
  }
  else{
    Serial.println("Invalid Command Ignored!");
  }
}


/**
 * prepares the input buffer to receive a new message and tells the serial connected device it is ready for more.
 */
void ready() {
	sofar=0;  // clear input buffer
	Serial.println(F(">"));  // signal ready to receive input
}
