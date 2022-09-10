#ifndef PINS_RAMPS_H
#define PINS_RAMPS_H


#ifndef BOARD_NAME
  #define BOARD_NAME "RAMPS 1.4"
#endif

/*
 * Laser 
 */
#define RIGHT_LASER_PIN 9   //MOFSET Power 9  Laser 1 in FabScan
#define LEFT_LASER_PIN 10   //MOFSET Power 10 Laser 2 in FabScan

#define MICROSTEP   A5

/*
 * Turntable Stepper  
 */
#define ENABLE_PIN_0  38    //X_ENABLE_PIN
#define STEP_PIN_0    54    //X_STEP_PIN
#define DIR_PIN_0     55    //X_DIR_PIN

/**
 * WS2812 LED 
 */
#define LIGHT_PIN 4        //LED_PIN

/**
 * Optional Laser Stepper  
 */

#define ENABLE_PIN_1  62    //Z_ENABLE_PIN
#define STEP_PIN_1    46    //Z_STEP_PIN
#define DIR_PIN_1     48    //Z_DIR_PIN

 /**
  * Optional Laser Servos
  */

#define RIGHT_SERVO_PIN 6   //SERVO1_PIN
#define LEFT_SERVO_PIN 5    //SERVO2_PIN 

/**
 *  STEPPER DEFINES FOR SELECTION
 */
#define LASER_STEPPER        11  //0xb
#define TURNTABLE_STEPPER    10  //0xa

#endif
