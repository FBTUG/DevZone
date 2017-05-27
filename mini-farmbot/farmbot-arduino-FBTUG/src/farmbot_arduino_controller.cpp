// Do not remove the include below
#include "farmbot_arduino_controller.h"
#include "pins.h"
#include "Config.h"
#include "StepperControl.h"
#include "ServoControl.h"
#include "PinGuard.h"
#include "TimerOne.h"
#include "MemoryFree.h"
#include "Debug.h"
#include "CurrentState.h"

static char commandEndChar = 0x0A;
static GCodeProcessor *gCodeProcessor = new GCodeProcessor();

unsigned long lastAction;
unsigned long currentTime;
unsigned long cycleCounter = 0;
bool previousEmergencyStop = false;

int lastParamChangeNr = 0;

String commandString = "";
char incomingChar = 0;
char incomingCommandArray[INCOMING_CMD_BUF_SIZE];
int incomingCommandPointer = 0;

// Blink led routine used for testing
bool blink = false;
void blinkLed()
{
  blink = !blink;
  digitalWrite(LED_PIN, blink);
}

// Interrupt handling for:
//   - movement
//   - encoders
//   - pin guard
//
bool interruptBusy = false;
int interruptSecondTimer = 0;
void interrupt(void)
{
  if (!debugInterrupt)
  {
    interruptSecondTimer++;

    if (interruptBusy == false)
    {

      interruptBusy = true;
      StepperControl::getInstance()->handleMovementInterrupt();

      // Check the actions triggered once per second
      if (interruptSecondTimer >= 1000000 / MOVEMENT_INTERRUPT_SPEED)
      {
        interruptSecondTimer = 0;
        PinGuard::getInstance()->checkPins();
        //blinkLed();
      }

      interruptBusy = false;
    }
  }
}

//measure the quantity of square wave
//void pulse() {
//   waterFlow += 1.0 / 5880.0;
//  waterFlow += 1.0 / 5880.0;
//}

//The setup function is called once at startup of the sketch
void setup()
{

  // Setup pin input/output settings
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(E_STEP_PIN, OUTPUT);
  pinMode(E_DIR_PIN, OUTPUT);
  pinMode(E_ENABLE_PIN, OUTPUT);
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(X_MAX_PIN, INPUT_PULLUP);

  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Y_MIN_PIN, INPUT_PULLUP);
  pinMode(Y_MAX_PIN, INPUT_PULLUP);

  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(Z_MIN_PIN, INPUT_PULLUP);
  pinMode(Z_MAX_PIN, INPUT_PULLUP);

  pinMode(HEATER_0_PIN, OUTPUT);
  pinMode(HEATER_1_PIN, OUTPUT);
  pinMode(WATER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(SERVO_0_PIN , OUTPUT);
  pinMode(SERVO_1_PIN , OUTPUT);

  digitalWrite(X_ENABLE_PIN, HIGH);
  digitalWrite(E_ENABLE_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);

  Serial.begin(115200);

  delay(100);

  // Start the motor handling
  //ServoControl::getInstance()->attach();

  // Load motor settings
  StepperControl::getInstance()->loadSettings();
  /**/

  // Dump all values to the serial interface
  // ParameterList::getInstance()->readAllValues();

  // Get the settings for the pin guard
  PinGuard::getInstance()->loadConfig();

  // Start the interrupt used for moving
  // Interrupt management code library written by Paul Stoffregen
  // The default time 100 micro seconds

  Timer1.attachInterrupt(interrupt);
  Timer1.initialize(MOVEMENT_INTERRUPT_SPEED);
  Timer1.start();

  // Initialize the inactivity check
  lastAction = millis();

  if (ParameterList::getInstance()->getValue(MOVEMENT_HOME_AT_BOOT_X) == 1)
  {
    StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, true, false, false);
  }

  if (ParameterList::getInstance()->getValue(MOVEMENT_HOME_AT_BOOT_Y) == 1)
  {
    StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, false, true, false);
  }

  if (ParameterList::getInstance()->getValue(MOVEMENT_HOME_AT_BOOT_Z) == 1)
  {
    StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, false, false, true);
  }
}

// The loop function is called in an endless loop
void loop()
{

  if (debugInterrupt)
  {
    StepperControl::getInstance()->handleMovementInterrupt();
  }

  if (Serial.available())
  {

    // Save current time stamp for timeout actions
    lastAction = millis();

    // Get the input and start processing on receiving 'new line'
    incomingChar = Serial.read();

    // Filter out emergency stop.
    if (!(incomingChar == 69 || incomingChar == 101))
    {
      incomingCommandArray[incomingCommandPointer] = incomingChar;
      incomingCommandPointer++;
    }
    else
    {
      CurrentState::getInstance()->setEmergencyStop();
    }

    // If the string is getting to long, cap it off with a new line and let it process anyway
    if (incomingCommandPointer >= INCOMING_CMD_BUF_SIZE - 1)
    {
      incomingChar = '\n';
      incomingCommandArray[incomingCommandPointer] = incomingChar;
      incomingCommandPointer++;
    }

    if (incomingChar == '\n' || incomingCommandPointer >= INCOMING_CMD_BUF_SIZE)
    {

      char commandChar[incomingCommandPointer + 1];
      for (int i = 0; i < incomingCommandPointer - 1; i++)
      {
        if (incomingChar)
        commandChar[i] = incomingCommandArray[i];
      }
      commandChar[incomingCommandPointer] = 0;

      if (incomingCommandPointer > 1)
      {

        // Copy the command to another string object.
        // because there are issues with passing the
        // string to the command object

        // Create a command and let it execute
        //Command* command = new Command(commandString);
        Command *command = new Command(commandChar);

        if (LOGGING || debugMessages)
        {
          command->print();
        }

        gCodeProcessor->execute(command);

        free(command);

      }

      incomingCommandPointer = 0;
    }
  }

  // In case of emergency stop, disable movement and
  // shut down the pins used
  if (previousEmergencyStop == false && CurrentState::getInstance()->isEmergencyStop())
  {
    StepperControl::getInstance()->disableMotors();
    PinControl::getInstance()->resetPinsUsed();
    if (debugMessages)
    {
      Serial.print(COMM_REPORT_COMMENT);
      Serial.print(" Going to safe state");
      CurrentState::getInstance()->printQAndNewLine();
    }
  }
  previousEmergencyStop = CurrentState::getInstance()->isEmergencyStop();

  // Check if parameters are changes, and if so load the new settings
  if (lastParamChangeNr != ParameterList::getInstance()->paramChangeNumber())
  {
    lastParamChangeNr = ParameterList::getInstance()->paramChangeNumber();

    Serial.print(COMM_REPORT_COMMENT);
    Serial.print(" loading parameters ");
    CurrentState::getInstance()->printQAndNewLine();

    StepperControl::getInstance()->loadSettings();
    PinGuard::getInstance()->loadConfig();
  }

  // Do periodic checks and feedback

  currentTime = millis();
  if (currentTime < lastAction)
  {

    // If the device timer overruns, reset the idle counter
    lastAction = millis();
  }
  else
  {

    if ((currentTime - lastAction) > 5000)
    {
      // After an idle time, send the idle message

      if (CurrentState::getInstance()->isEmergencyStop())
      {
        Serial.print(COMM_REPORT_EMERGENCY_STOP);
        CurrentState::getInstance()->printQAndNewLine();

        if (debugMessages)
        {
          Serial.print(COMM_REPORT_COMMENT);
          Serial.print(" Emergency stop engaged");
          CurrentState::getInstance()->printQAndNewLine();
        }
      }
      else
      {
        Serial.print(COMM_REPORT_CMD_IDLE);
        CurrentState::getInstance()->printQAndNewLine();
      }

      StepperControl::getInstance()->storePosition();
      CurrentState::getInstance()->printPosition();

      CurrentState::getInstance()->storeEndStops();
      CurrentState::getInstance()->printEndStops();

      if (debugMessages)
      {
        Serial.print(COMM_REPORT_COMMENT);
        Serial.print(" MEM ");
        Serial.print(freeMemory());
        CurrentState::getInstance()->printQAndNewLine();

        Serial.print(COMM_REPORT_COMMENT);
        Serial.print(" Cycle ");
        Serial.print(cycleCounter);
        CurrentState::getInstance()->printQAndNewLine();

        StepperControl::getInstance()->test();
      }

      //  Tim 2017-04-20 Temporary disabling the warning of no valid configuration
      //  until fully supported on RPI
      //  if (ParameterList::getInstance()->getValue(PARAM_CONFIG_OK) != 1)
      //  {
      //    Serial.print(COMM_REPORT_NO_CONFIG);
      //	}

      cycleCounter++;
      lastAction = millis();
    }
  }
}
