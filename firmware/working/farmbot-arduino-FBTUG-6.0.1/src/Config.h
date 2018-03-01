/*
   Config.h

    Created on: 16 maj 2014
        Author: MattLech
        Author: Tim Evers
*/

#ifndef CONFIG_H_
#define CONFIG_H_

#define BT_Serial2_Use    //-2018.02.25-CGH

const int LOGGING = 0;

const int INCOMING_CMD_BUF_SIZE = 50;

const char COMM_REPORT_CMD_IDLE[4] = {'R', '0', '0', '\0'};
const char COMM_REPORT_CMD_START[4] = {'R', '0', '1', '\0'};
const char COMM_REPORT_CMD_DONE[4] = {'R', '0', '2', '\0'};
const char COMM_REPORT_CMD_ERROR[4] = {'R', '0', '3', '\0'};
const char COMM_REPORT_CMD_BUSY[4] = {'R', '0', '4', '\0'};
const char COMM_REPORT_CMD_STATUS[4] = {'R', '0', '5', '\0'};
const char COMM_REPORT_CALIB_STATUS[4] = {'R', '0', '6', '\0'};
const char COMM_REPORT_CMD_RETRY[4] = { 'R', '0', '7', '\0' };
const char COMM_REPORT_CMD_ECHO[4] = { 'R', '0', '8', '\0' };
const char COMM_REPORT_BAD_CMD[4] = { 'R', '0', '9', '\0' };

const char COMM_REPORT_HOMED_X[4] = { 'R', '1', '1', '\0' };
const char COMM_REPORT_HOMED_Y[4] = { 'R', '1', '2', '\0' };
const char COMM_REPORT_HOMED_Z[4] = { 'R', '1', '3', '\0' };

const char COMM_REPORT_TIMEOUT_X[4] = { 'R', '7', '1', '\0' };
const char COMM_REPORT_TIMEOUT_Y[4] = { 'R', '7', '2', '\0' };
const char COMM_REPORT_TIMEOUT_Z[4] = { 'R', '7', '3', '\0' };

const char COMM_REPORT_ENCODER_SCALED[4] = { 'R', '8', '4', '\0' };
const char COMM_REPORT_ENCODER_RAW[4] = { 'R', '8', '5', '\0' };

const char COMM_REPORT_EMERGENCY_STOP[4] = { 'R', '8', '7', '\0' };
const char COMM_REPORT_NO_CONFIG[4] = {'R', '8', '8', '\0'};
const char COMM_REPORT_COMMENT[4] = {'R', '9', '9', '\0'};

const int COMM_REPORT_MOVE_STATUS_IDLE = 0;
const int COMM_REPORT_MOVE_STATUS_START_MOTOR = 1;
const int COMM_REPORT_MOVE_STATUS_ACCELERATING = 2;
const int COMM_REPORT_MOVE_STATUS_CRUISING = 3;
const int COMM_REPORT_MOVE_STATUS_DECELERATING = 4;
const int COMM_REPORT_MOVE_STATUS_STOP_MOTOR = 5;
const int COMM_REPORT_MOVE_STATUS_CRAWLING = 6;
const int COMM_REPORT_MOVE_STATUS_ERROR = -1;

const int COMM_REPORT_CALIBRATE_STATUS_IDLE = 0;
const int COMM_REPORT_CALIBRATE_STATUS_TO_HOME = 1;
const int COMM_REPORT_CALIBRATE_STATUS_TO_END = 2;
const int COMM_REPORT_CALIBRATE_STATUS_ERROR = -1;

const int MOVEMENT_INTERRUPT_SPEED = 64; // Interrupt cycle in micro seconds
//const int MOVEMENT_INTERRUPT_SPEED = 40; // Interrupt cycle in micro seconds

const unsigned int MOVEMENT_SPEED_BASE_TIME = 2000;
const unsigned int MOVEMENT_DELAY = 250;

const int PARAM_VERSION_DEFAULT = 1;
const int PARAM_TEST_DEFAULT = 0;

const int PARAM_CONFIG_OK_DEFAULT = 0;        //-2018.02.27
const int PARAM_USE_EEPROM_DEFAULT = 1;

const int PARAM_E_STOP_ON_MOV_ERR_DEFAULT = 0;
const int PARAM_MOV_NR_RETRY_DEFAULT = 3;

const int MOVEMENT_TIMEOUT_X_DEFAULT = 120;
const int MOVEMENT_TIMEOUT_Y_DEFAULT = 120;
const int MOVEMENT_TIMEOUT_Z_DEFAULT = 120;

const int MOVEMENT_KEEP_ACTIVE_X_DEFAULT = 0;
const int MOVEMENT_KEEP_ACTIVE_Y_DEFAULT = 0;
const int MOVEMENT_KEEP_ACTIVE_Z_DEFAULT = 1;

const int MOVEMENT_HOME_AT_BOOT_X_DEFAULT = 0;   //-啟動時,歸零運動
const int MOVEMENT_HOME_AT_BOOT_Y_DEFAULT = 0;
const int MOVEMENT_HOME_AT_BOOT_Z_DEFAULT = 0;

//const int MOVEMENT_ENABLE_ENDPOINTS_X_DEFAULT = 0;
//const int MOVEMENT_ENABLE_ENDPOINTS_Y_DEFAULT = 0;
//const int MOVEMENT_ENABLE_ENDPOINTS_Z_DEFAULT = 0;
const int MOVEMENT_ENABLE_ENDPOINTS_X_DEFAULT = 1;   //-Default 0 -2018.02.24-FBTUG
const int MOVEMENT_ENABLE_ENDPOINTS_Y_DEFAULT = 1;   //-Default 0 -2018.02.24-FBTUG
const int MOVEMENT_ENABLE_ENDPOINTS_Z_DEFAULT = 1;   //-Default 0 -2018.02.24-FBTUG

const int MOVEMENT_INVERT_ENDPOINTS_X_DEFAULT = 0;
const int MOVEMENT_INVERT_ENDPOINTS_Y_DEFAULT = 0;
const int MOVEMENT_INVERT_ENDPOINTS_Z_DEFAULT = 0;

const int MOVEMENT_INVERT_MOTOR_X_DEFAULT = 0;
const int MOVEMENT_INVERT_MOTOR_Y_DEFAULT = 0;
const int MOVEMENT_INVERT_MOTOR_Z_DEFAULT = 0;

const int MOVEMENT_SECONDARY_MOTOR_X_DEFAULT = 1;
//const int MOVEMENT_SECONDARY_MOTOR_INVERT_X_DEFAULT = 1;
const int MOVEMENT_SECONDARY_MOTOR_INVERT_X_DEFAULT = 0;   //-2018.02.24-FBTUG

const int MOVEMENT_HOME_UP_X_DEFAULT = 0;
const int MOVEMENT_HOME_UP_Y_DEFAULT = 0;
const int MOVEMENT_HOME_UP_Z_DEFAULT = 1;

//-原廠設定
//const int MOVEMENT_STEP_PER_MM_X_DEFAULT = 5;
//const int MOVEMENT_STEP_PER_MM_Y_DEFAULT = 5;
//const int MOVEMENT_STEP_PER_MM_Z_DEFAULT = 25;

// X,Y 1圈 20齒 , 皮帶 1齒 2mm, 1圈 = 40mm
// X,Y 3200pulse / 40mm = 80 pulse/mm
// Z軸螺桿 1圈 8mm, 3200pulse / 8mm = 400 pulse/mm
//const int MOVEMENT_STEP_PER_MM_X_DEFAULT = 80;    //-2018.02.24-miniFarmbot
//const int MOVEMENT_STEP_PER_MM_Y_DEFAULT = 80;    //-2018.02.24-miniFarmbot
//const int MOVEMENT_STEP_PER_MM_Z_DEFAULT = 400;   //-2018.02.24-miniFarmbot

#define MOVEMENT_STEP_PER_MM_X_DEFAULT 80   //-2018.02.25-miniFarmbot
#define MOVEMENT_STEP_PER_MM_Y_DEFAULT 80   //-2018.02.25-miniFarmbot
#define MOVEMENT_STEP_PER_MM_Z_DEFAULT 400  //-2018.02.25-miniFarmbot

//-有點錯亂, 統一將 const long 改成 const int -2018.02.25
//-內容改不過來, 一直是 5, 5, 25, 換一片新的 Mega2560 OK
//-搞錯了,原來是從 EEPROM讀出, 若初值 -1 才從 Default值讀出->寫入EEPROM, readAllValuesFromEeprom()

// Number of steps used for acceleration or deceleration
const int MOVEMENT_STEPS_ACC_DEC_X_DEFAULT = 300;
const int MOVEMENT_STEPS_ACC_DEC_Y_DEFAULT = 300;
const int MOVEMENT_STEPS_ACC_DEC_Z_DEFAULT = 300;

// Minimum speed in steps per second
const int MOVEMENT_MIN_SPD_X_DEFAULT = 50;
const int MOVEMENT_MIN_SPD_Y_DEFAULT = 50;
const int MOVEMENT_MIN_SPD_Z_DEFAULT = 50;

// Speed used for homing and calibration
const int MOVEMENT_HOME_SPEED_X_DEFAULT = 50;
const int MOVEMENT_HOME_SPEED_Y_DEFAULT = 50;
const int MOVEMENT_HOME_SPEED_Z_DEFAULT = 50;

// Maximum speed in steps per second
//const int MOVEMENT_MAX_SPD_X_DEFAULT = 400;
//const int MOVEMENT_MAX_SPD_Y_DEFAULT = 400;
//const int MOVEMENT_MAX_SPD_Z_DEFAULT = 400;
const int MOVEMENT_MAX_SPD_X_DEFAULT = 3500;   //-400-2018.02.24
const int MOVEMENT_MAX_SPD_Y_DEFAULT = 3500;   //-400-2018.02.24
const int MOVEMENT_MAX_SPD_Z_DEFAULT = 3500;   //-400-2018.02.24

// switch the end contacts from NO to NC
const int MOVEMENT_INVERT_2_ENDPOINTS_X_DEFAULT = 0;
const int MOVEMENT_INVERT_2_ENDPOINTS_Y_DEFAULT = 0;
const int MOVEMENT_INVERT_2_ENDPOINTS_Z_DEFAULT = 0;

// Stop at the home position or continue to other size of axis
const int MOVEMENT_STOP_AT_HOME_X_DEFAULT = 0;
const int MOVEMENT_STOP_AT_HOME_Y_DEFAULT = 0;
const int MOVEMENT_STOP_AT_HOME_Z_DEFAULT = 0;

// Stop at the maximum size of the axis
const int MOVEMENT_STOP_AT_MAX_X_DEFAULT = 0;
const int MOVEMENT_STOP_AT_MAX_Y_DEFAULT = 0;
const int MOVEMENT_STOP_AT_MAX_Z_DEFAULT = 0;

// Use encoder (0 or 1)
const int ENCODER_ENABLED_X_DEFAULT = 0;
const int ENCODER_ENABLED_Y_DEFAULT = 0;
const int ENCODER_ENABLED_Z_DEFAULT = 0;

// Type of enocder.
// 0 = non-differential encoder, channel A,B
// 1 = differenttial encoder, channel A, A*, B, B*
const int ENCODER_TYPE_X_DEFAULT = 0;
const int ENCODER_TYPE_Y_DEFAULT = 0;
const int ENCODER_TYPE_Z_DEFAULT = 0;

// Position = encoder position * scaling / 10000
const int ENCODER_SCALING_X_DEFAULT = 5556;
const int ENCODER_SCALING_Y_DEFAULT = 5556;
const int ENCODER_SCALING_Z_DEFAULT = 5556;

// Number of steps missed before motor is seen as not moving
const int ENCODER_MISSED_STEPS_MAX_X_DEFAULT = 5;
const int ENCODER_MISSED_STEPS_MAX_Y_DEFAULT = 5;
const int ENCODER_MISSED_STEPS_MAX_Z_DEFAULT = 5;

// How much a good step is substracted from the missed step total (1-99)
// 10 means it ignores 10 steps in 100. This is normal because of jerkiness while moving
const int ENCODER_MISSED_STEPS_DECAY_X_DEFAULT = 5;
const int ENCODER_MISSED_STEPS_DECAY_Y_DEFAULT = 5;
const int ENCODER_MISSED_STEPS_DECAY_Z_DEFAULT = 5;

// Use the encoder for positioning
const int ENCODER_USE_FOR_POS_X_DEFAULT = 0;
const int ENCODER_USE_FOR_POS_Y_DEFAULT = 0;
const int ENCODER_USE_FOR_POS_Z_DEFAULT = 0;

// Invert the encoder position sign
const int ENCODER_INVERT_X_DEFAULT = 0;
const int ENCODER_INVERT_Y_DEFAULT = 0;
const int ENCODER_INVERT_Z_DEFAULT = 0;

// Length of axis in steps. Zero means don't care
const int MOVEMENT_AXIS_NR_STEPS_X_DEFAULT = 0;
const int MOVEMENT_AXIS_NR_STEPS_Y_DEFAULT = 0;
const int MOVEMENT_AXIS_NR_STEPS_Z_DEFAULT = 0;

// Pin guard default settings
const int PIN_GUARD_1_PIN_NR_DEFAULT = 0;
const int PIN_GUARD_1_TIME_OUT_DEFAULT = 60;
const int PIN_GUARD_1_ACTIVE_STATE_DEFAULT = 1;

const int PIN_GUARD_2_PIN_NR_DEFAULT = 0;
const int PIN_GUARD_2_TIME_OUT_DEFAULT = 60;
const int PIN_GUARD_2_ACTIVE_STATE_DEFAULT = 1;

const int PIN_GUARD_3_PIN_NR_DEFAULT = 0;
const int PIN_GUARD_3_TIME_OUT_DEFAULT = 60;
const int PIN_GUARD_3_ACTIVE_STATE_DEFAULT = 1;

const int PIN_GUARD_4_PIN_NR_DEFAULT = 0;
const int PIN_GUARD_4_TIME_OUT_DEFAULT = 60;
const int PIN_GUARD_4_ACTIVE_STATE_DEFAULT = 1;

const int PIN_GUARD_5_PIN_NR_DEFAULT = 0;
const int PIN_GUARD_5_TIME_OUT_DEFAULT = 60;
const int PIN_GUARD_5_ACTIVE_STATE_DEFAULT = 1;

const int STATUS_GENERAL_DEFAULT = 0;

//const char SOFTWARE_VERSION[] = "4.0.2\0";

#define NSS_PIN       22
#define READ_ENA_PIN  49
#define NULL          0

// Control the water servo angle
// MAX: Full open
// MIN: Close the water switch
const int MAX_WATER_SERVO_ANGLE = 70;
const int MIN_WATER_SERVO_ANGLE = 0;

//- FBTUG - 2018.02.24 (Joe Ho) ----------------------------------
// Water flow counts of 1L (default)
const int WATER_FLOW_1L_COUNTS_DEFAULT = 5200;

// Water flow time(ms) of 100mL (default)
const int WATER_FLOW_100ML_TIME_DEFAULT = 2635;

const int SOIL_SENSOR_HIGH_DEFAULT = 518;   // 0%
const int SOIL_SENSOR_LOW_DEFAULT = 250;    // 100%

const int END_STOP_NC_ENABLE_DEFAULT = 1;

//----------------------------------------------------------------
static const int mdl_spi_encoder_offset = 4;

enum MdlSpiEncoders{
  _MDL_X1 = 0b0001,
  _MDL_X2 = 0b0010,
  _MDL_Y = 0b0100,
  _MDL_Z = 0b1000,
};

#endif /* CONFIG_H_ */

//----------------------------------------------------------------
#if defined(RAMPS_V14) && !defined(SOFTWARE_VERSION)
#define SOFTWARE_VERSION "6.0.1.R\0"
#endif

#if defined(FARMDUINO_V10) && !defined(SOFTWARE_VERSION)
#define SOFTWARE_VERSION "6.0.1.F\0"
#endif

#if defined(FARMDUINO_V14) && !defined(SOFTWARE_VERSION)
#define SOFTWARE_VERSION "6.0.1.G\0"
#endif

//----------------------------------------------------------------
