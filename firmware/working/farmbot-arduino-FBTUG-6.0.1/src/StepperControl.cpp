#include "StepperControl.h"
#include "Debug.h"
#include "Config.h"

#define cX_Axis   0   //-2018.02.24-CGH
#define cY_Axis   1   //-2018.02.24-CGH
#define cZ_Axis   2   //-2018.02.24-CGH

static StepperControl *instance;

//------------------------------------------------------------------------------------------
StepperControl *StepperControl::getInstance()
{
  if (!instance){
    instance = new StepperControl();
  };
  return instance;
};

//------------------------------------------------------------------------------------------
void StepperControl::reportEncoders()
{
  Serial.print(COMM_REPORT_ENCODER_SCALED);   //R84
  Serial.print(" X"); Serial.print((float)encoderX.currentPosition() / (float)stepsPerMm[0]);
  Serial.print(" Y"); Serial.print((float)encoderY.currentPosition() / (float)stepsPerMm[1]);
  Serial.print(" Z"); Serial.print((float)encoderZ.currentPosition() / (float)stepsPerMm[2]);
  #ifdef BT_Serial2_Use
    Serial2.print(COMM_REPORT_ENCODER_SCALED);    //-2018.02.25-CGH
    Serial2.print(" X"); Serial2.print((float)encoderX.currentPosition() / (float)stepsPerMm[0]);   //-2018.02.25-CGH
    Serial2.print(" Y"); Serial2.print((float)encoderY.currentPosition() / (float)stepsPerMm[1]);   //-2018.02.25-CGH
    Serial2.print(" Z"); Serial2.print((float)encoderZ.currentPosition() / (float)stepsPerMm[2]);   //-2018.02.25-CGH    
  #endif   
  CurrentState::getInstance()->printQAndNewLine();

  //------------------------------------------------------------
  Serial.print(COMM_REPORT_ENCODER_RAW);    //-R85
  Serial.print(" X"); Serial.print(encoderX.currentPositionRaw());
  Serial.print(" Y"); Serial.print(encoderY.currentPositionRaw());
  Serial.print(" Z"); Serial.print(encoderZ.currentPositionRaw());  
  #ifdef BT_Serial2_Use
    Serial2.print(COMM_REPORT_ENCODER_RAW);   //-2018.02.25-CGH
    Serial2.print(" X"); Serial2.print(encoderX.currentPositionRaw());    //-2018.02.25-CGH
    Serial2.print(" Y"); Serial2.print(encoderY.currentPositionRaw());    //-2018.02.25-CGH
    Serial2.print(" Z"); Serial2.print(encoderZ.currentPositionRaw());    //-2018.02.25-CGH  
  #endif  
  CurrentState::getInstance()->printQAndNewLine();
}

//------------------------------------------------------------------------------------------
void StepperControl::reportStatus(StepperControlAxis *axis, int axisStatus)
{  
  serialBuffer += COMM_REPORT_CMD_STATUS;
  serialBuffer += " ";
  serialBuffer += axis->label;
  serialBuffer += axisStatus;
  serialBuffer += CurrentState::getInstance()->getQAndNewLine();

  //Serial.print(COMM_REPORT_CMD_STATUS);
  //Serial.print(" ");
  //Serial.print(axis->label);
  //Serial.print(axisStatus);
  //CurrentState::getInstance()->printQAndNewLine();
}

//------------------------------------------------------------------------------------------
void StepperControl::reportCalib(StepperControlAxis *axis, int calibStatus)
{
  Serial.print(COMM_REPORT_CALIB_STATUS); Serial.print(" "); Serial.print(axis->label); Serial.print(calibStatus);
  #ifdef BT_Serial2_Use
    Serial2.print(COMM_REPORT_CALIB_STATUS); Serial2.print(" "); Serial2.print(axis->label); Serial2.print(calibStatus);    //-2018.02.25-CGH
  #endif    
  CurrentState::getInstance()->printQAndNewLine();
}

//------------------------------------------------------------------------------------------
void StepperControl::checkAxisSubStatus(StepperControlAxis *axis, int *axisSubStatus)
{
  int newStatus = 0;
  bool statusChanged = false;

  if (axis->isAccelerating()){
    newStatus = COMM_REPORT_MOVE_STATUS_ACCELERATING;
  }

  if (axis->isCruising()){
    newStatus = COMM_REPORT_MOVE_STATUS_CRUISING;
  }

  if (axis->isDecelerating()){
    newStatus = COMM_REPORT_MOVE_STATUS_DECELERATING;
  }

  if (axis->isCrawling()){
    newStatus = COMM_REPORT_MOVE_STATUS_CRAWLING;
  }

  // if the status changes, send out a status report
  if (*axisSubStatus != newStatus && newStatus > 0){
    statusChanged = true;
  }
  *axisSubStatus = newStatus;

  if (statusChanged){
    reportStatus(axis, *axisSubStatus);
  }
}

//const int MOVEMENT_INTERRUPT_SPEED = 100; // Interrupt cycle in micro seconds

//------------------------------------------------------------------------------------------
StepperControl::StepperControl()
{
  // Initialize some variables for testing

  motorMotorsEnabled = false;

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  motorLastPosition[0] = 0;
  motorLastPosition[1] = 0;
  motorLastPosition[2] = 0;

  motorConsEncoderLastPosition[0] = 0;
  motorConsEncoderLastPosition[1] = 0;
  motorConsEncoderLastPosition[2] = 0;

  // Create the axis controllers

  axisX = StepperControlAxis();
  axisY = StepperControlAxis();
  axisZ = StepperControlAxis();

  axisX.label = 'X';
  axisY.label = 'Y';
  axisZ.label = 'Z';

  // Create the encoder controller

  encoderX = StepperControlEncoder();
  encoderY = StepperControlEncoder();
  encoderZ = StepperControlEncoder();

  // Load settings

  loadSettings();

  motorMotorsEnabled = false;
}

//------------------------------------------------------------------------------------------
void StepperControl::loadSettings()
{

  // Load motor settings

  axisX.loadPinNumbers(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, E_STEP_PIN, E_DIR_PIN, E_ENABLE_PIN);
  axisY.loadPinNumbers(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, 0, 0, 0);
  axisZ.loadPinNumbers(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, 0, 0, 0);

  axisSubStep[0] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[1] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[2] = COMM_REPORT_MOVE_STATUS_IDLE;

  loadMotorSettings();

  // Load encoder settings

  loadEncoderSettings();

  encoderX.loadMdlEncoderId(_MDL_X1);
  encoderY.loadMdlEncoderId(_MDL_Y);
  encoderZ.loadMdlEncoderId(_MDL_Z);

  encoderX.loadPinNumbers(X_ENCDR_A, X_ENCDR_B, X_ENCDR_A_Q, X_ENCDR_B_Q);
  encoderY.loadPinNumbers(Y_ENCDR_A, Y_ENCDR_B, Y_ENCDR_A_Q, Y_ENCDR_B_Q);
  encoderZ.loadPinNumbers(Z_ENCDR_A, Z_ENCDR_B, Z_ENCDR_A_Q, Z_ENCDR_B_Q);

  encoderX.loadSettings(motorConsEncoderType[0], motorConsEncoderScaling[0], motorConsEncoderInvert[0]);
  encoderY.loadSettings(motorConsEncoderType[1], motorConsEncoderScaling[1], motorConsEncoderInvert[1]);
  encoderZ.loadSettings(motorConsEncoderType[2], motorConsEncoderScaling[2], motorConsEncoderInvert[2]);
}

//------------------------------------------------------------------------------------------
void StepperControl::test()
{
  Serial.print("R99");
  Serial.print(" mot x = "); Serial.print(axisX.currentPosition());
  Serial.print(" enc x = "); Serial.print(encoderX.currentPosition());
  Serial.print("\r\n");

  Serial.print("R99");
  Serial.print(" mot y = "); Serial.print(axisY.currentPosition());
  Serial.print(" enc y = "); Serial.print(encoderY.currentPosition());
  Serial.print("\r\n");

  Serial.print("R99");
  Serial.print(" mot z = "); Serial.print(axisZ.currentPosition());
  Serial.print(" enc z = "); Serial.print(encoderZ.currentPosition());
  Serial.print("\r\n");

  //-2018.02.25-CGH
  #ifdef BT_Serial2_Use
    Serial2.print("R99");
    Serial2.print(" mot x = "); Serial2.print(axisX.currentPosition());
    Serial2.print(" enc x = "); Serial2.print(encoderX.currentPosition());
    Serial2.print("\r\n");
  
    Serial2.print("R99");
    Serial2.print(" mot y = "); Serial2.print(axisY.currentPosition());
    Serial2.print(" enc y = "); Serial2.print(encoderY.currentPosition());
    Serial2.print("\r\n");
  
    Serial2.print("R99");
    Serial2.print(" mot z = "); Serial2.print(axisZ.currentPosition());
    Serial2.print(" enc z = "); Serial2.print(encoderZ.currentPosition());
    Serial2.print("\r\n");
  #endif
  
  // read changes in encoder
  //encoderX.readEncoder();
  //encoderY.readEncoder();
  //encoderZ.readEncoder();
  //reportPosition();

  //bool test = axisX.endStopMin();
  //Serial.print("R99");
  //Serial.print(" test = ");
  //Serial.print(test);
  //Serial.print("\r\n");
}

//------------------------------------------------------------------------------------------
void StepperControl::test2()
{
  CurrentState::getInstance()->printPosition();
  encoderX.test();
  //encoderY.test();
  //encoderZ.test();
}

//------------------------------------------------------------------------------------------
/**
 * xDest - destination X in steps
 * yDest - destination Y in steps
 * zDest - destination Z in steps
 * maxStepsPerSecond - maximum number of steps per second
 * maxAccelerationStepsPerSecond - maximum number of acceleration in steps per second
 */
int StepperControl::moveToCoords(double xDestScaled, double yDestScaled, double zDestScaled,
                                 unsigned int xMaxSpd, unsigned int yMaxSpd, unsigned int zMaxSpd,
                                 bool xHome, bool yHome, bool zHome)
{
long xDest = xDestScaled * stepsPerMm[0];
long yDest = yDestScaled * stepsPerMm[1];
long zDest = zDestScaled * stepsPerMm[2];

unsigned long currentMillis = 0;
unsigned long timeStart = millis();
  
int incomingByte = 0;
int error = 0;
bool EmergencyStop = false;   //-2018.02.24

unsigned int commandSpeed[3];

   //Serial.print("stepsPerMm = "); Serial.print(stepsPerMm[0]); Serial.print(" , "); 
   //Serial.print(stepsPerMm[1]); Serial.print(" , "); Serial.println(stepsPerMm[2]);   //-2018.02.24
   
    serialMessageNr = 0;
    serialMessageDelay = 0;
  
    // if a speed is given in the command, use that instead of the config speed
    if (xMaxSpd > 0 && xMaxSpd < speedMax[0]){
        commandSpeed[0] = xMaxSpd;
    }
    else{
        commandSpeed[0] = speedMax[0];
    }
  
    if (yMaxSpd > 0 && yMaxSpd < speedMax[1]){
        commandSpeed[1] = yMaxSpd;
    }
    else{
        commandSpeed[1] = speedMax[1];
    }
  
    if (zMaxSpd > 0 && zMaxSpd < speedMax[2]){
        commandSpeed[2] = zMaxSpd;
    }
    else{
        commandSpeed[2] = speedMax[2];
    }
  
    axisX.setMaxSpeed(commandSpeed[0]);
    axisY.setMaxSpeed(commandSpeed[1]);
    axisZ.setMaxSpeed(commandSpeed[2]);
          
    //--------------------------------------------------------------------------
    // Load coordinates into axis class
    long sourcePoint[3] = {0, 0, 0};
    sourcePoint[0] = CurrentState::getInstance()->getX();
    sourcePoint[1] = CurrentState::getInstance()->getY();
    sourcePoint[2] = CurrentState::getInstance()->getZ();
  
    long currentPoint[3] = {0, 0, 0};
    currentPoint[0] = sourcePoint[0];   //CurrentState::getInstance()->getX();-2018.02.28
    currentPoint[1] = sourcePoint[1];   //CurrentState::getInstance()->getY();-2018.02.28
    currentPoint[2] = sourcePoint[2];   //CurrentState::getInstance()->getZ();-2018.02.28

    //-改良 G00 不用 x y z 都要輸入, 可 單軸 或 兩軸 運動-2018.02.28
    if(xDestScaled == -9999) xDest = currentPoint[0];  //-X-2018.02.28-沒資料不動作
    if(yDestScaled == -9999) yDest = currentPoint[1];  //-X-2018.02.28-沒資料不動作
    if(zDestScaled == -9999) zDest = currentPoint[2];  //-X-2018.02.28-沒資料不動作
  
    long destinationPoint[3] = {0, 0, 0};
    destinationPoint[0] = xDest;
    destinationPoint[1] = yDest;
    destinationPoint[2] = zDest;
  
    motorConsMissedSteps[0] = 0;
    motorConsMissedSteps[1] = 0;
    motorConsMissedSteps[2] = 0;
  
    motorConsMissedStepsPrev[0] = 0;
    motorConsMissedStepsPrev[1] = 0;
    motorConsMissedStepsPrev[2] = 0;
  
    motorLastPosition[0] = currentPoint[0];
    motorLastPosition[1] = currentPoint[1];
    motorLastPosition[2] = currentPoint[2];
  
    //--------------------------------------------------------------------------
    // Load coordinates into motor control
    axisX.loadCoordinates(currentPoint[0], destinationPoint[0], xHome);
    axisY.loadCoordinates(currentPoint[1], destinationPoint[1], yHome);
    axisZ.loadCoordinates(currentPoint[2], destinationPoint[2], zHome);

    //-絕對座標運動��
    //-2018.02.24-CGH, 1 向max, -1 向min, 0 沒動
    int moveDirection[3] = {0, 0, 0};
    if(xDest > currentPoint[cX_Axis]) moveDirection[cX_Axis] = 1; else moveDirection[cX_Axis] = -1;
    if(yDest > currentPoint[cY_Axis]) moveDirection[cY_Axis] = 1; else moveDirection[cY_Axis] = -1;
    if(zDest > currentPoint[cZ_Axis]) moveDirection[cZ_Axis] = 1; else moveDirection[cZ_Axis] = -1;    
    
    if(xDest == currentPoint[cX_Axis]) moveDirection[cX_Axis] = 0;
    if(yDest == currentPoint[cY_Axis]) moveDirection[cY_Axis] = 0;
    if(zDest == currentPoint[cZ_Axis]) moveDirection[cZ_Axis] = 0;
    
    //Serial.print("xDest = "); Serial.print(xDest); Serial.print(" currentPoint = "); Serial.println(currentPoint[cX_Axis]);    //-2018.02.24
    //Serial.print("yDest = "); Serial.print(yDest); Serial.print(" currentPoint = "); Serial.println(currentPoint[cY_Axis]);    //-2018.02.24
    //Serial.print("zDest = "); Serial.print(zDest); Serial.print(" currentPoint = "); Serial.println(currentPoint[cZ_Axis]);    //-2018.02.24
    
    if(moveDirection[cX_Axis] == 0) axisX.deactivateAxis();   //-2018.02.24-CGH
    if(moveDirection[cY_Axis] == 0) axisY.deactivateAxis();   //-2018.02.24-CGH
    if(moveDirection[cZ_Axis] == 0) axisZ.deactivateAxis();   //-2018.02.24-CGH
    
    //--------------------------------------------------------------------------    
    // Prepare for movement
    axisX.movementStarted = false;
    axisY.movementStarted = false;
    axisZ.movementStarted = false;
  
    storeEndStops();    //-取得各軸極限開關狀態 CurrentState::getInstance()->storeEndStops();
    reportEndStops();
  
    axisX.setDirectionAxis();
    axisY.setDirectionAxis();
    axisZ.setDirectionAxis();
  
    // Enable motors
    axisSubStep[0] = COMM_REPORT_MOVE_STATUS_START_MOTOR;
    axisSubStep[1] = COMM_REPORT_MOVE_STATUS_START_MOTOR;
    axisSubStep[2] = COMM_REPORT_MOVE_STATUS_START_MOTOR;
  
    reportStatus(&axisX, axisSubStep[0]);
    reportStatus(&axisY, axisSubStep[1]);
    reportStatus(&axisZ, axisSubStep[2]);
  
    //enableMotors();   //-啟用所有馬達(激磁)   //-2018.02.24
  
    // Start movement
  
    axisActive[0] = true;
    axisActive[1] = true;
    axisActive[2] = true;
  
    if (xHome || yHome || zHome){
        if (!xHome) axisX.deactivateAxis();
        if (!yHome) axisY.deactivateAxis();
        if (!zHome) axisZ.deactivateAxis();
    
        axisActive[0] = xHome;
        axisActive[1] = yHome;
        axisActive[2] = zHome;
    }
  
    axisX.checkMovement();
    axisY.checkMovement();
    axisZ.checkMovement();
    //checkMovement -> 若在極限SNR上 activateAxis = false;  2018.02.24
  
    axisX.setTicks();
    axisY.setTicks();
    axisZ.setTicks();

    //--------------------------------------------------------------------------
    EmergencyStop = CurrentState::getInstance()->isEmergencyStop();
    if(EmergencyStop){  //-2018.02.24-CGH
        //Serial.println("EmergencyStop = true");   //-2018.02.24
      
        axisX.deactivateAxis();
        axisY.deactivateAxis();
        axisZ.deactivateAxis();

        goto OK_Err;    //-2018.02.24
    }    

    axisActive[cX_Axis] = axisX.isAxisActive();   //-2018.02.24-CGH
    axisActive[cY_Axis] = axisY.isAxisActive();   //-2018.02.24-CGH
    axisActive[cZ_Axis] = axisZ.isAxisActive();   //-2018.02.24-CGH
                
    if(!(axisActive[cX_Axis] || axisActive[cY_Axis] || axisActive[cZ_Axis])) goto OK_Err;   //-Pass-2018.02.24-CGH
    
    enableMotors();   //-啟用所有馬達(激磁)   //-2018.02.24
    
  // Let the interrupt handle all the movements
  while ((axisActive[0] || axisActive[1] || axisActive[2]) && !EmergencyStop){   //-2018.02.24  
    #if defined(FARMDUINO_V14)
      checkEncoders();
    #endif

    checkAxisSubStatus(&axisX, &axisSubStep[0]);
    checkAxisSubStatus(&axisY, &axisSubStep[1]);
    checkAxisSubStatus(&axisZ, &axisSubStep[2]);

    //checkEncoders();

    axisX.checkTiming();
    axisY.checkTiming();
    axisZ.checkTiming();

        //--------------------------------------------------------------------
        if(axisActive[cX_Axis]){    //-2018.02.24
            if (axisX.isStepDone()){
                axisX.checkMovement();    //-變速 或 若在極限SNR上 activateAxis = false;  2018.02.24
                checkAxisVsEncoder(&axisX, &encoderX, &motorConsMissedSteps[0], &motorLastPosition[0], &motorConsEncoderLastPosition[0], &motorConsEncoderUseForPos[0], &motorConsMissedStepsDecay[0], &motorConsEncoderEnabled[0]);
                axisX.resetStepDone();
            }
        }
        
        if(axisActive[cY_Axis]){    //-2018.02.24          
            if (axisY.isStepDone()){
                axisY.checkMovement();    //-變速 或 若在極限SNR上 activateAxis = false;  2018.02.24
                checkAxisVsEncoder(&axisY, &encoderY, &motorConsMissedSteps[1], &motorLastPosition[1], &motorConsEncoderLastPosition[1], &motorConsEncoderUseForPos[1], &motorConsMissedStepsDecay[1], &motorConsEncoderEnabled[1]);
                axisY.resetStepDone();
            }
        }
        
        if(axisActive[cZ_Axis]){    //-2018.02.24
            if (axisZ.isStepDone()){
                axisZ.checkMovement();    //-變速 或 若在極限SNR上 activateAxis = false;  2018.02.24
                checkAxisVsEncoder(&axisZ, &encoderZ, &motorConsMissedSteps[2], &motorLastPosition[2], &motorConsEncoderLastPosition[2], &motorConsEncoderUseForPos[2], &motorConsMissedStepsDecay[2], &motorConsEncoderEnabled[2]);
                axisZ.resetStepDone();
            }
        }
        
    //if (debugInterrupt){
    //  delayMicroseconds(100);
    //  handleMovementInterrupt();
    //}
    //else{
    //  delay(1);
    //}

    /*
    if ((int)motorConsMissedSteps[0] != motorConsMissedStepsPrev[0]){
        motorConsMissedStepsPrev[0] = (int) motorConsMissedSteps[0];
        Serial.print("R99");
        Serial.print(" missed steps X = ");
        Serial.print(motorConsMissedStepsPrev[0]);
        Serial.print("\r\n");
    }

    if ((int)motorConsMissedSteps[1] != motorConsMissedStepsPrev[1]){
        motorConsMissedStepsPrev[1] = (int)motorConsMissedSteps[1];
        Serial.print("R99");
        Serial.print(" missed steps Y = ");
        Serial.print(motorConsMissedStepsPrev[1]);
        Serial.print("\r\n");
    }

    if ((int)motorConsMissedSteps[2] != motorConsMissedStepsPrev[2]){
        motorConsMissedStepsPrev[2] = (int)motorConsMissedSteps[2];
        Serial.print("R99");
        Serial.print(" missed steps Z = ");
        Serial.print(motorConsMissedStepsPrev[2]);
        Serial.print("\r\n");
    }
    */
    
    //--------------------------------------------------------------------    
    if(axisActive[cX_Axis]){    //-2018.02.24
        //if (axisX.isAxisActive() && motorConsMissedSteps[0] > motorConsMissedStepsMax[0]){
        if(motorConsMissedSteps[0] > motorConsMissedStepsMax[0]){   //-2018.02.24
            axisX.deactivateAxis();
            serialBuffer += "R99";
            serialBuffer += " deactivate motor X due to missed steps";
            serialBuffer += "\r\n";
      
            if (xHome){
                encoderX.setPosition(0);
                axisX.setCurrentPosition(0);
            }
            else{
                error = 1;
            }
        }
    }
    
    if(axisActive[cY_Axis]){    //-2018.02.24
        //if (axisY.isAxisActive() && motorConsMissedSteps[1] > motorConsMissedStepsMax[1]){
        if(motorConsMissedSteps[1] > motorConsMissedStepsMax[1]){   //-2018.02.24
            axisY.deactivateAxis();
            serialBuffer += "R99";
            serialBuffer += " deactivate motor Y due to missed steps";
            serialBuffer += "\r\n";
            
            if (yHome){
                encoderY.setPosition(0);
                axisY.setCurrentPosition(0);
            }
            else{
                error = 1;
            }
        }
    }

    if(axisActive[cZ_Axis]){    //-2018.02.24        
        //if (axisZ.isAxisActive() && motorConsMissedSteps[2] > motorConsMissedStepsMax[2]){
        if(motorConsMissedSteps[2] > motorConsMissedStepsMax[2]){   //-2018.02.24
            axisZ.deactivateAxis();
      
            serialBuffer += "R99";
            serialBuffer += " deactivate motor Z due to missed steps";
            serialBuffer += "\r\n";
      
            if (zHome){
                encoderZ.setPosition(0);
                axisZ.setCurrentPosition(0);        
            }
            else{
                error = 1;
            }
        }
    }

    //--------------------------------------------------------------------------
    if(axisActive[cX_Axis] == true){    //-2018.02.24-CGH  
        if (axisX.endStopAxisReached(false)){   //- endStop Reached���� min-Limit            
            //- checkMovement() 有做 Home運動 處理-2018.02.24
            axisX.setCurrentPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH
            if(ENCODER_ENABLED_X_DEFAULT == 1)  //-2018.02.24-CGH
                encoderX.setPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH          
        }
    }
    
    if(axisActive[cY_Axis] == true){    //-2018.02.24-CGH  
        if (axisY.endStopAxisReached(false)){   //- endStop Reached���� min-Limit
            //- checkMovement() 有做 Home運動 處理-2018.02.24
            axisY.setCurrentPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH
            if(ENCODER_ENABLED_Y_DEFAULT == 1)  //-2018.02.24-CGH
                encoderY.setPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH 
        }
    }
    
    if(axisActive[cZ_Axis] == true){    //-2018.02.24-CGH  
        if (axisZ.endStopAxisReached(false)){   //- endStop Reached���� min-Limit
            //- checkMovement() 有做 Home運動 處理-2018.02.24
            axisZ.setCurrentPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH
            if(ENCODER_ENABLED_Z_DEFAULT == 1)  //-2018.02.24-CGH
                encoderZ.setPosition(0);        //歸零機制下,才重設座標為0-2018.02.24-CGH 
        }
    }
    
    //--------------------------------------------------------------------------
    axisActive[0] = axisX.isAxisActive();
    axisActive[1] = axisY.isAxisActive();
    axisActive[2] = axisZ.isAxisActive();
    
    //--------------------------------------------------------------------------
    currentPoint[0] = axisX.currentPosition();
    currentPoint[1] = axisY.currentPosition();
    currentPoint[2] = axisZ.currentPosition();

    //--------------------------------------------------------------------------
    CurrentState::getInstance()->setX(currentPoint[0]);
    CurrentState::getInstance()->setY(currentPoint[1]);
    CurrentState::getInstance()->setZ(currentPoint[2]);
    
    //--------------------------------------------------------------------
    storeEndStops();  //-取得各軸極限開關狀態 CurrentState::getInstance()->storeEndStops();

    //--------------------------------------------------------------------
    // Check timeouts
    if(axisActive[cX_Axis]){    //-2018.02.24
        //if (axisActive[0] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[0] * 1000) || (millis() < timeStart && millis() > timeOut[0] * 1000))){
        if(((millis() >= timeStart && millis() - timeStart > timeOut[0] * 1000) || (millis() < timeStart && millis() > timeOut[0] * 1000))){    //-2018.02.24
            serialBuffer += COMM_REPORT_TIMEOUT_X;
            serialBuffer += "\r\n";
            serialBuffer += "R99 timeout X axis\r\n";
            error = 2;
        }            
    }
    
    if(axisActive[cY_Axis]){    //-2018.02.24
        //if (axisActive[1] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[1] * 1000) || (millis() < timeStart && millis() > timeOut[1] * 1000))){
        if(((millis() >= timeStart && millis() - timeStart > timeOut[1] * 1000) || (millis() < timeStart && millis() > timeOut[1] * 1000))){    //-2018.02.24
            serialBuffer += COMM_REPORT_TIMEOUT_Y;
            serialBuffer += "\r\n";
            serialBuffer += "R99 timeout Y axis\r\n";
            error = 2;
        }
    }
    
    if(axisActive[cZ_Axis]){    //-2018.02.24
        //if (axisActive[2] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[2] * 1000) || (millis() < timeStart && millis() > timeOut[2] * 1000))){
        if(((millis() >= timeStart && millis() - timeStart > timeOut[2] * 1000) || (millis() < timeStart && millis() > timeOut[2] * 1000))){    //-2018.02.24
            serialBuffer += COMM_REPORT_TIMEOUT_Z;
            serialBuffer += "\r\n";
            serialBuffer += "R99 timeout Z axis\r\n";
            error = 2;
        }
    }
    
    //--------------------------------------------------------------------------
    // Check if there is an emergency stop command
    if (Serial.available() > 0){
        incomingByte = Serial.read();
        if (incomingByte == 69 || incomingByte == 101){
            serialBuffer += "R99 emergency stop\r\n";
    
            Serial.print(COMM_REPORT_EMERGENCY_STOP);
            #ifdef BT_Serial2_Use
              Serial2.print(COMM_REPORT_EMERGENCY_STOP);   //-2018.02.28-CGH
            #endif
            CurrentState::getInstance()->printQAndNewLine();
    
            EmergencyStop = true;   //-2018.02.24
    
            //axisX.deactivateAxis();   //-2018.02.24-CGH
            //axisY.deactivateAxis();   //-2018.02.24-CGH
            //axisZ.deactivateAxis();   //-2018.02.24-CGH
    
            //axisActive[0] = false;    //-2018.02.24-CGH
            //axisActive[1] = false;    //-2018.02.24-CGH
            //axisActive[2] = false;    //-2018.02.24-CGH
    
            error = 1;
        }
    }

    //-2018.02.25-CGH ------------------------------------------
    #ifdef BT_Serial2_Use
      if (Serial2.available() > 0){
          incomingByte = Serial2.read();
          if (incomingByte == 69 || incomingByte == 101){
              serialBuffer += "R99 emergency stop\r\n";

              Serial.print(COMM_REPORT_EMERGENCY_STOP);   //-2018.02.28-CGH  
              Serial2.print(COMM_REPORT_EMERGENCY_STOP);
              CurrentState::getInstance()->printQAndNewLine();
      
              EmergencyStop = true;
     
              error = 1;
          }
      }
    #endif
    
    if (error != 0){
        serialBuffer += "R99 error\r\n";

        axisX.deactivateAxis();   //-2018.02.24-CGH
        axisY.deactivateAxis();   //-2018.02.24-CGH
        axisZ.deactivateAxis();   //-2018.02.24-CGH
            
        axisActive[0] = false;    //= axisX.isAxisActive();   //-2018.02.24-CGH
        axisActive[1] = false;    //= axisY.isAxisActive();   //-2018.02.24-CGH
        axisActive[2] = false;    //= axisZ.isAxisActive();   //-2018.02.24-CGH
    }

    // Send the serial buffer one character per cycle to keep motor timing more accuracte
    serialBufferSendNext();

    //--------------------------------------------------------------------------
    // Periodically send message still active
    currentMillis++;
    serialMessageDelay++;

    if (serialMessageDelay > 300 && serialBuffer.length() == 0 && serialBufferSending == 0){
      serialMessageDelay = 0;

      switch(serialMessageNr){
        case 0:
          serialBuffer += COMM_REPORT_CMD_BUSY;
          serialBuffer += CurrentState::getInstance()->getQAndNewLine();
          break;
        case 1:
          serialBuffer += CurrentState::getInstance()->getPosition();
          serialBuffer += CurrentState::getInstance()->getQAndNewLine();
          break;
      }

      serialMessageNr++;
      if (serialMessageNr > 1){
          serialMessageNr = 0;
      }

      serialBufferSending = 0;
      
      if (debugMessages /*&& debugInterrupt*/){
  				Serial.print("R99");
  				Serial.print(" missed step "); Serial.print(motorConsMissedSteps[1]);
  				Serial.print(" encoder pos "); Serial.print(encoderY.currentPosition());
  				Serial.print(" axis pos "); Serial.print(axisY.currentPosition());
  				Serial.print("\r\n");
          //-2018.02.25-CGH
          #ifdef BT_Serial2_Use
            Serial2.print("R99");
            Serial2.print(" missed step "); Serial2.print(motorConsMissedSteps[1]);
            Serial2.print(" encoder pos "); Serial2.print(encoderY.currentPosition());
            Serial2.print(" axis pos "); Serial2.print(axisY.currentPosition());
            Serial2.print("\r\n");         
         #endif   
      }
    }
  }

    serialBufferEmpty();
    Serial.print("R99 stopped\r\n");
    #ifdef BT_Serial2_Use
      Serial2.print("R99 stopped\r\n");    //-2018.02.25-CGH     
    #endif

OK_Err:   //-2018.02.24

    // Send feedback for homing
    if (xHome && !error && !EmergencyStop){    //-2018.02.24
        Serial.print(COMM_REPORT_HOMED_X);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_HOMED_X);    //-2018.02.25-CGH
        #endif  
        CurrentState::getInstance()->printQAndNewLine();
    }
  
    if (yHome && !error && !EmergencyStop){    //-2018.02.24
        Serial.print(COMM_REPORT_HOMED_Y);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_HOMED_Y);    //-2018.02.25-CGH
        #endif  
        CurrentState::getInstance()->printQAndNewLine();
    }
  
    if (zHome && !error && !EmergencyStop){    //-2018.02.24
        Serial.print(COMM_REPORT_HOMED_Z);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_HOMED_Z);    //-2018.02.25-CGH
        #endif  
        CurrentState::getInstance()->printQAndNewLine();
    }
  
    //--------------------------------------------------------------------------
    // Stop motors
    axisSubStep[0] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
    axisSubStep[1] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
    axisSubStep[2] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
  
    //--------------------------------------------------------------------------
    reportStatus(&axisX, axisSubStep[0]);
    reportStatus(&axisY, axisSubStep[1]);
    reportStatus(&axisZ, axisSubStep[2]);
    
    //--------------------------------------------------------------------------
    disableMotors();
  
    //--------------------------------------------------------------------------
    // Report end statuses
    currentPoint[0] = axisX.currentPosition();
    currentPoint[1] = axisY.currentPosition();
    currentPoint[2] = axisZ.currentPosition();
  
    //--------------------------------------------------------------------------
    CurrentState::getInstance()->setX(currentPoint[0]);
    CurrentState::getInstance()->setY(currentPoint[1]);
    CurrentState::getInstance()->setZ(currentPoint[2]);
  
    //--------------------------------------------------------------------------
    storeEndStops();
    reportEndStops();
    reportPosition();
    reportEncoders();
  
    //--------------------------------------------------------------------------
    // Report axis idle
    axisSubStep[0] = COMM_REPORT_MOVE_STATUS_IDLE;
    axisSubStep[1] = COMM_REPORT_MOVE_STATUS_IDLE;
    axisSubStep[2] = COMM_REPORT_MOVE_STATUS_IDLE;
  
    //--------------------------------------------------------------------------
    reportStatus(&axisX, axisSubStep[0]);
    reportStatus(&axisY, axisSubStep[1]);
    reportStatus(&axisZ, axisSubStep[2]);
    
    //--------------------------------------------------------------------------
    disableMotors();
    
    //--------------------------------------------------------------------------
    if (EmergencyStop){    //-2018.02.24
      CurrentState::getInstance()->setEmergencyStop();
    }
  
    Serial.print("R99 error "); Serial.print(error); Serial.print("\r\n");
    #ifdef BT_Serial2_Use
      Serial2.print("R99 error "); Serial2.print(error); Serial2.print("\r\n");    //-2018.02.25-CGH
    #endif  
  
    // Return error
    CurrentState::getInstance()->setLastError(error);
  
    return error;
}

//------------------------------------------------------------------------------------------
void StepperControl::serialBufferEmpty()
{
  while (serialBuffer.length() > 0)
  {
    serialBufferSendNext();
  }
}

//------------------------------------------------------------------------------------------
void StepperControl::serialBufferSendNext()
{
  // Send the next char in the serialBuffer
  if (serialBuffer.length() > 0){

    if (serialBufferSending < serialBuffer.length()){
      //Serial.print("-");
      switch (serialBuffer.charAt(serialBufferSending)){
      case 13:
        Serial.print("\r\n");
        #ifdef BT_Serial2_Use
          Serial2.print("\r\n");   //-2018.02.25-CGH
        #endif   
        break;
      case 10:
        break;
      default:
        Serial.print(serialBuffer.charAt(serialBufferSending));
        #ifdef BT_Serial2_Use
          Serial2.print(serialBuffer.charAt(serialBufferSending));   //-2018.02.25-CGH
        #endif  
        break;
      }

      serialBufferSending++;
    }
    else{
      // Reset length of buffer when done
      serialBuffer = "";
      serialBufferSending = 0;
    }
  }
}

//------------------------------------------------------------------------------------------
// Calibration
int StepperControl::calibrateAxis(int axis)
{

  // Load motor and encoder settings

  loadMotorSettings();
  loadEncoderSettings();

  //unsigned long timeStart             = millis();

  bool movementDone = false;

  int paramValueInt = 0;
  int stepsCount = 0;
  int incomingByte = 0;
  int error = 0;

  bool invertEndStops = false;
  int parEndInv;
  int parNbrStp;

  float *missedSteps;
  int *missedStepsMax;
  long *lastPosition;
  float *encoderStepDecay;
  bool *encoderEnabled;
  int *axisStatus;
  long *axisStepsPerMm;

  // Prepare for movement

  storeEndStops();
  reportEndStops();

  // Select the right axis
  StepperControlAxis *calibAxis;
  StepperControlEncoder *calibEncoder;

  switch (axis)
  {
  case 0:
    calibAxis = &axisX;
    calibEncoder = &encoderX;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_X;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_X;
    invertEndStops = endStInv[0];
    missedSteps = &motorConsMissedSteps[0];
    missedStepsMax = &motorConsMissedStepsMax[0];
    lastPosition = &motorLastPosition[0];
    encoderStepDecay = &motorConsMissedStepsDecay[0];
    encoderEnabled = &motorConsEncoderEnabled[0];
    axisStatus = &axisSubStep[0];
    axisStepsPerMm = &stepsPerMm[0];
    break;
  case 1:
    calibAxis = &axisY;
    calibEncoder = &encoderY;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_Y;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_Y;
    invertEndStops = endStInv[1];
    missedSteps = &motorConsMissedSteps[1];
    missedStepsMax = &motorConsMissedStepsMax[1];
    lastPosition = &motorLastPosition[1];
    encoderStepDecay = &motorConsMissedStepsDecay[1];
    encoderEnabled = &motorConsEncoderEnabled[1];
    axisStatus = &axisSubStep[1];
    axisStepsPerMm = &stepsPerMm[1];
    break;
  case 2:
    calibAxis = &axisZ;
    calibEncoder = &encoderZ;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_Z;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_Z;
    invertEndStops = endStInv[2];
    missedSteps = &motorConsMissedSteps[2];
    missedStepsMax = &motorConsMissedStepsMax[2];
    lastPosition = &motorLastPosition[2];
    encoderStepDecay = &motorConsMissedStepsDecay[2];
    encoderEnabled = &motorConsEncoderEnabled[2];
    axisStatus = &axisSubStep[2];
    axisStepsPerMm = &stepsPerMm[2];
    break;
  default:
    Serial.print("R99 Calibration error: invalid axis selected\r\n");
    #ifdef BT_Serial2_Use
      Serial2.print("R99 Calibration error: invalid axis selected\r\n");   //-2018.02.25-CGH
    #endif
      
    error = 1;
    CurrentState::getInstance()->setLastError(error);
    return error;
  }

  // Preliminary checks
  if (calibAxis->endStopMin() || calibAxis->endStopMax()){    //-Check Limit switch-2018.02.24
      Serial.print("R99 Calibration error: end stop active before start\r\n");
      #ifdef BT_Serial2_Use
        Serial2.print("R99 Calibration error: end stop active before start\r\n");   //-2018.02.25-CGH
      #endif  
      error = 1;
      CurrentState::getInstance()->setLastError(error);
      return error;
  }

  Serial.print("R99"); Serial.print(" axis "); Serial.print(calibAxis->label); Serial.print(" move to start for calibration"); Serial.print("\r\n");
  #ifdef BT_Serial2_Use
    Serial2.print("R99"); Serial2.print(" axis "); Serial2.print(calibAxis->label); Serial2.print(" move to start for calibration"); Serial2.print("\r\n");   //-2018.02.25-CGH
  #endif
  
  *axisStatus = COMM_REPORT_MOVE_STATUS_START_MOTOR;
  reportStatus(calibAxis, axisStatus[0]);

  // Move towards home
  calibAxis->enableMotor();
  calibAxis->setDirectionHome();
  calibAxis->setCurrentPosition(calibEncoder->currentPosition());

  stepsCount = 0;
  *missedSteps = 0;
  movementDone = false;

  //motorConsMissedSteps[0] = 0;    //-2018.02.24-CGH
  //motorConsMissedSteps[1] = 0;    //-2018.02.24-CGH
  //motorConsMissedSteps[2] = 0;    //-2018.02.24-CGH
  motorConsMissedSteps[axis] = 0;   //-2018.02.24-CGH
  
  *axisStatus = COMM_REPORT_MOVE_STATUS_CRAWLING;
  reportStatus(calibAxis, axisStatus[0]);

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_TO_HOME);

  while (!movementDone && error == 0){
    #if defined(FARMDUINO_V14)
      checkEncoders();
    #endif

    checkAxisVsEncoder(calibAxis, calibEncoder, &motorConsMissedSteps[axis], &motorLastPosition[axis], &motorConsEncoderLastPosition[axis], &motorConsEncoderUseForPos[axis], &motorConsMissedStepsDecay[axis], &motorConsEncoderEnabled[axis]);

    // Check if there is an emergency stop command
    if (Serial.available() > 0){
      incomingByte = Serial.read();
      if (incomingByte == 69 || incomingByte == 101){        
        Serial.print("R99 emergency stop\r\n");
        #ifdef BT_Serial2_Use
          Serial2.print("R99 emergency stop\r\n");    //-2018.02.28-CGH
        #endif          
        movementDone = true;
        CurrentState::getInstance()->setEmergencyStop();
        Serial.print(COMM_REPORT_EMERGENCY_STOP);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_EMERGENCY_STOP);    //-2018.02.28-CGH
        #endif    
        
        CurrentState::getInstance()->printQAndNewLine();
        error = 1;
      }
    }
    
    //-2018.02.25-CGH -----------------------------
    #ifdef BT_Serial2_Use
      if (Serial2.available() > 0){
          incomingByte = Serial2.read();
          if (incomingByte == 69 || incomingByte == 101){
              Serial.print("R99 emergency stop\r\n");   //-2018.02.28-CGH
              Serial2.print("R99 emergency stop\r\n");
              movementDone = true;
              CurrentState::getInstance()->setEmergencyStop();
              Serial.print(COMM_REPORT_EMERGENCY_STOP);   //-2018.02.28-CGH
              Serial2.print(COMM_REPORT_EMERGENCY_STOP);
              CurrentState::getInstance()->printQAndNewLine();
              error = 1;
          }
      }
    #endif
    
    // Move until the end stop for home position is reached, either by end stop or motor skipping
    if ((!calibAxis->endStopMin() && !calibAxis->endStopMax()) && !movementDone && (*missedSteps < *missedStepsMax)){

      calibAxis->setStepAxis();
 
      delayMicroseconds(100000 / speedHome[axis] / 2);

      stepsCount++;
      if (stepsCount % (speedHome[axis] * 3) == 0){
        // Periodically send message still active
        Serial.print(COMM_REPORT_CMD_BUSY);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_CMD_BUSY);    //-2018.02.25-CGH
        #endif  
        CurrentState::getInstance()->printQAndNewLine();
      }

      if (debugMessages){
        if (stepsCount % (speedHome[axis] / 6) == 0 /*|| *missedSteps > 3*/){
          Serial.print("R99");
          Serial.print(" step count ");   Serial.print(stepsCount);
          Serial.print(" missed steps "); Serial.print(*missedSteps);
          Serial.print(" max steps ");    Serial.print(*missedStepsMax);
          Serial.print(" cur pos mtr ");  Serial.print(calibAxis->currentPosition());
          Serial.print(" cur pos enc ");  Serial.print(calibEncoder->currentPosition());
          Serial.print("\r\n");

          //-2018.02.25-CGH
          #ifdef BT_Serial2_Use
            Serial2.print("R99");
            Serial2.print(" step count ");   Serial2.print(stepsCount);
            Serial2.print(" missed steps "); Serial2.print(*missedSteps);
            Serial2.print(" max steps ");    Serial2.print(*missedStepsMax);
            Serial2.print(" cur pos mtr ");  Serial2.print(calibAxis->currentPosition());
            Serial2.print(" cur pos enc ");  Serial2.print(calibEncoder->currentPosition());
            Serial2.print("\r\n");     
          #endif        
        }
      }

      calibAxis->resetMotorStep();
      delayMicroseconds(100000 / speedHome[axis] / 2);
    }
    else{
      movementDone = true;
      Serial.print("R99 movement done\r\n");
      #ifdef BT_Serial2_Use
        Serial2.print("R99 movement done\r\n");    //-2018.02.25-CGH
      #endif
  
      // If end stop for home is active, set the position to zero
      if (calibAxis->endStopMax()){
        invertEndStops = true;
      }
    }
  }

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_TO_END);

  Serial.print("R99"); Serial.print(" axis "); Serial.print(calibAxis->label); Serial.print(" at starting point"); Serial.print("\r\n");
  #ifdef BT_Serial2_Use
    Serial2.print("R99"); Serial2.print(" axis "); Serial2.print(calibAxis->label); Serial2.print(" at starting point"); Serial2.print("\r\n");    //-2018.02.25-CGH
  #endif   

  // Report back the end stop setting

  if (error == 0){
    if (invertEndStops){
      paramValueInt = 1;
    }
    else{
      paramValueInt = 0;
    }

    Serial.print("R23"); Serial.print(" "); Serial.print("P"); Serial.print(parEndInv); Serial.print(" "); Serial.print("V"); Serial.print(paramValueInt);
    //Serial.print("\r\n");    
    #ifdef BT_Serial2_Use
      Serial2.print("R23"); Serial2.print(" "); Serial2.print("P"); Serial2.print(parEndInv); Serial2.print(" "); Serial2.print("V"); Serial2.print(paramValueInt);   //-2018.02.25-CGH
      //Serial2.print("\r\n");    //-2018.02.25-CGH
    #endif    
    CurrentState::getInstance()->printQAndNewLine();
  }

  // Store the status of the system

  storeEndStops();
  reportEndStops();

  // Move into the other direction now, and measure the number of steps
  Serial.print("R99"); Serial.print(" axis "); Serial.print(calibAxis->label); Serial.print(" calibrating length"); Serial.print("\r\n");
  #ifdef BT_Serial2_Use
    Serial2.print("R99"); Serial2.print(" axis "); Serial2.print(calibAxis->label); Serial2.print(" calibrating length"); Serial2.print("\r\n");   //-2018.02.25-CGH
  #endif  
  
  stepsCount = 0;
  movementDone = false;
  *missedSteps = 0;
  calibAxis->setDirectionAway();
  calibAxis->setCurrentPosition(calibEncoder->currentPosition());

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  long encoderStartPoint = calibEncoder->currentPosition();
  long encoderEndPoint = calibEncoder->currentPosition();

  while (!movementDone && error == 0)
  {

    #if defined(FARMDUINO_V14)
       checkEncoders();
    #endif

    checkAxisVsEncoder(calibAxis, calibEncoder, &motorConsMissedSteps[axis], &motorLastPosition[axis], &motorConsEncoderLastPosition[axis], &motorConsEncoderUseForPos[axis], &motorConsMissedStepsDecay[axis], &motorConsEncoderEnabled[axis]);

    // Check if there is an emergency stop command
    if (Serial.available() > 0){
      incomingByte = Serial.read();
      if (incomingByte == 69 || incomingByte == 101){
        Serial.print("R99 emergency stop\r\n");
        #ifdef BT_Serial2_Use
          Serial2.print("R99 emergency stop\r\n");    //-2018.02.28-CGH
        #endif           
        movementDone = true;
        CurrentState::getInstance()->setEmergencyStop();
        Serial.print(COMM_REPORT_EMERGENCY_STOP);
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_EMERGENCY_STOP);    //-2018.02.28-CGH
        #endif               
        CurrentState::getInstance()->printQAndNewLine();
        error = 1;
      }
    }

    //-2018.02.25-CGH ---------------------------------
    #ifdef BT_Serial2_Use
      if (Serial2.available() > 0){
          incomingByte = Serial2.read();
          if (incomingByte == 69 || incomingByte == 101){
              Serial.print("R99 emergency stop\r\n");   //-2018.02.28-CGH
              Serial2.print("R99 emergency stop\r\n");
              movementDone = true;
              CurrentState::getInstance()->setEmergencyStop();
              Serial.print(COMM_REPORT_EMERGENCY_STOP);   //-2018.02.28-CGH
              Serial2.print(COMM_REPORT_EMERGENCY_STOP);
              CurrentState::getInstance()->printQAndNewLine();
              error = 1;
          }
      }
    #endif

    // Ignore the missed steps at startup time
    if (stepsCount < 10){
      *missedSteps = 0;
    }

    // Move until the end stop at the other side of the axis is reached
    if (((!invertEndStops && !calibAxis->endStopMax()) || (invertEndStops && !calibAxis->endStopMin())) && !movementDone && (*missedSteps < *missedStepsMax))
    {

      calibAxis->setStepAxis();
      stepsCount++;

      //checkAxisVsEncoder(&axisX, &encoderX, &motorConsMissedSteps[0], &motorLastPosition[0], &motorConsMissedStepsDecay[0], &motorConsEncoderEnabled[0]);

      delayMicroseconds(100000 / speedHome[axis] / 2);

      if (stepsCount % (speedHome[axis] * 3) == 0){
        // Periodically send message still active
        Serial.print(COMM_REPORT_CMD_BUSY); //Serial.print("\r\n");
        #ifdef BT_Serial2_Use
          Serial2.print(COMM_REPORT_CMD_BUSY); //Serial2.print("\r\n");   //-2018.02.25-CGH
        #endif        
        CurrentState::getInstance()->printQAndNewLine();

        Serial.print("R99"); Serial.print(" step count: "); Serial.print(stepsCount); Serial.print("\r\n");
        #ifdef BT_Serial2_Use
          Serial2.print("R99"); Serial2.print(" step count: "); Serial2.print(stepsCount); Serial2.print("\r\n");   //-2018.02.25-CGH
        #endif  
      }

      calibAxis->resetMotorStep();
      delayMicroseconds(100000 / speedHome[axis] / 2);
    }
    else{
      Serial.print("R99 movement done\r\n");
      #ifdef BT_Serial2_Use
        Serial2.print("R99 movement done\r\n");    //-2018.02.25-CGH
      #endif   
      movementDone = true;
    }
  }

  Serial.print("R99"); Serial.print(" axis "); Serial.print(calibAxis->label); Serial.print(" at end point"); Serial.print("\r\n");
  #ifdef BT_Serial2_Use
    Serial2.print("R99"); Serial2.print(" axis "); Serial2.print(calibAxis->label); Serial2.print(" at end point"); Serial2.print("\r\n");   //-2018.02.25-CGH
  #endif
  
  encoderEndPoint = calibEncoder->currentPosition();

  // if the encoder is enabled, use the encoder data instead of the step count

  if (encoderEnabled){
    stepsCount = abs(encoderEndPoint - encoderStartPoint);
  }

  // Report back the end stop setting

  if (error == 0){
    Serial.print("R23"); Serial.print(" "); Serial.print("P"); Serial.print(parNbrStp); Serial.print(" "); Serial.print("V"); Serial.print((float)stepsCount);
    #ifdef BT_Serial2_Use
      Serial2.print("R23"); Serial2.print(" "); Serial2.print("P"); Serial2.print(parNbrStp); Serial2.print(" "); Serial2.print("V"); Serial2.print((float)stepsCount);    //-2018.02.25-CGH
    #endif  
    CurrentState::getInstance()->printQAndNewLine();
  }

  *axisStatus = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
  reportStatus(calibAxis, axisStatus[0]);

  calibAxis->disableMotor();

  storeEndStops();
  reportEndStops();

  switch (axis)
  {
  case 0:
    CurrentState::getInstance()->setX(stepsCount);
    break;
  case 1:
    CurrentState::getInstance()->setY(stepsCount);
    break;
  case 2:
    CurrentState::getInstance()->setZ(stepsCount);
    break;
  }

  reportPosition();

  *axisStatus = COMM_REPORT_MOVE_STATUS_IDLE;
  reportStatus(calibAxis, axisStatus[0]);

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_IDLE);

  CurrentState::getInstance()->setLastError(error);
  return error;
}

int debugPrintCount = 0;

//------------------------------------------------------------------------------------------
// Check encoder to verify the motor is at the right position
void StepperControl::checkAxisVsEncoder(StepperControlAxis *axis, StepperControlEncoder *encoder, float *missedSteps, long *lastPosition, long *encoderLastPosition, int *encoderUseForPos, float *encoderStepDecay, bool *encoderEnabled)
{
  if (*encoderEnabled)
  {
    bool stepMissed = false;

    if (debugMessages){
		  //debugPrintCount++;
		  //if (debugPrintCount % 50 == 0){
			  Serial.print("R99");
			  Serial.print(" encoder pos "); Serial.print(encoder->currentPosition());
        Serial.print(" last enc "); Serial.print(*encoderLastPosition);
        Serial.print(" axis pos "); Serial.print(axis->currentPosition());
			  Serial.print(" last pos "); Serial.print(*lastPosition);
			  Serial.print(" move up "); Serial.print(axis->movingUp());
			  Serial.print(" missed step cons "); Serial.print(motorConsMissedSteps[0]);
			  Serial.print(" missed step "); Serial.print(*missedSteps);
        //Serial.print(" encoder X pos "); Serial.print(encoderX.currentPosition());
        //Serial.print(" axis X pos "); Serial.print(axisX.currentPosition());        
        Serial.print(" decay "); Serial.print(*encoderStepDecay); Serial.print(" enabled "); Serial.print(*encoderEnabled); Serial.print("\r\n");
                
        //-2018.02.25-CGH ----------------------------------------------------
        #ifdef BT_Serial2_Use
          Serial2.print("R99");
          Serial2.print(" encoder pos "); Serial2.print(encoder->currentPosition());
          Serial2.print(" last enc "); Serial2.print(*encoderLastPosition);
          Serial2.print(" axis pos "); Serial2.print(axis->currentPosition());
          Serial2.print(" last pos "); Serial2.print(*lastPosition);
          Serial2.print(" move up "); Serial2.print(axis->movingUp());
          Serial2.print(" missed step cons "); Serial2.print(motorConsMissedSteps[0]);
          Serial2.print(" missed step "); Serial2.print(*missedSteps);
          //Serial2.print(" encoder X pos "); Serial2.print(encoderX.currentPosition());
          //Serial2.print(" axis X pos "); Serial2.print(axisX.currentPosition());     
          Serial2.print(" decay "); Serial2.print(*encoderStepDecay); Serial2.print(" enabled "); Serial2.print(*encoderEnabled); Serial2.print("\r\n");    //-2018.02.25-CGH
        #endif   
		  //}
    }

    // Decrease amount of missed steps if there are no missed step
    if (*missedSteps > 0)
    {
      (*missedSteps) -= (*encoderStepDecay);
    }
    
    // Check if the encoder goes in the wrong direction or nothing moved
    if ((axis->movingUp() && *encoderLastPosition > encoder->currentPositionRaw()) ||
        (!axis->movingUp() && *encoderLastPosition < encoder->currentPositionRaw())){
      stepMissed = true;
    }

    if (stepMissed && *missedSteps < 32000){
      (*missedSteps)++;
    }

    *encoderLastPosition = encoder->currentPositionRaw();
    *lastPosition = axis->currentPosition();

    //axis->resetStepDone();
  }

  if (*encoderUseForPos){
    axis->setCurrentPosition(encoder->currentPosition());
  }
}

//------------------------------------------------------------------------------------------
void StepperControl::loadMotorSettings()
{
  // Load settings
  homeIsUp[0] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_X);
  homeIsUp[1] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_Y);
  homeIsUp[2] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_Z);

  speedMax[0] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_X);
  speedMax[1] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_Y);
  speedMax[2] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_Z);

  speedMin[0] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_X);
  speedMin[1] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_Y);
  speedMin[2] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_Z);

  speedHome[0] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_X);
  speedHome[1] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_Y);
  speedHome[2] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_Z);

  stepsAcc[0] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_X);
  stepsAcc[1] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_Y);
  stepsAcc[2] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_Z);

  motorInv[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_X));
  motorInv[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_Y));
  motorInv[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_Z));

  endStInv[0] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_X);
  endStInv[1] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_Y);
  endStInv[2] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_Z);

  endStInv2[0] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_X);
  endStInv2[1] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_Y);
  endStInv2[2] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_Z);

  endStEnbl[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_X));
  endStEnbl[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_Y));
  endStEnbl[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_Z));

  timeOut[0] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_X);
  timeOut[1] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_Y);
  timeOut[2] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_Z);

  motorKeepActive[0] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_X);
  motorKeepActive[1] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_Y);
  motorKeepActive[2] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_Z);

  motorMaxSize[0] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_X);
  motorMaxSize[1] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_Y);
  motorMaxSize[2] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_Z);

  motor2Inv[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_SECONDARY_MOTOR_INVERT_X));
  motor2Inv[1] = false;
  motor2Inv[2] = false;

  motor2Enbl[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_SECONDARY_MOTOR_X));
  motor2Enbl[1] = false;
  motor2Enbl[2] = false;

  motorStopAtHome[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_X));
  motorStopAtHome[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_Y));
  motorStopAtHome[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_Z));

  motorStopAtMax[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_X));
  motorStopAtMax[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_Y));
  motorStopAtMax[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_Z));

  stepsPerMm[0] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_X);
  stepsPerMm[1] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_Y);
  stepsPerMm[2] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_Z);
   
  //Serial.print("stepsPerMm = "); Serial.print(stepsPerMm[0]); Serial.print(" , "); 
  //Serial.print(stepsPerMm[1]); Serial.print(" , "); Serial.println(stepsPerMm[2]);   //-2018.02.25
  
  //Serial.print("MOVEMENT_STEP_PER_MM_X_DEFAULT = "); Serial.println(MOVEMENT_STEP_PER_MM_X_DEFAULT);   //-2018.02.25    
  //Serial.print("MOVEMENT_STEP_PER_MM_Y_DEFAULT = "); Serial.println(MOVEMENT_STEP_PER_MM_Y_DEFAULT);   //-2018.02.25
  //Serial.print("MOVEMENT_STEP_PER_MM_Z_DEFAULT = "); Serial.println(MOVEMENT_STEP_PER_MM_Z_DEFAULT);   //-2018.02.25
    
  if (stepsPerMm[0] < 1) stepsPerMm[0] = 1;
  if (stepsPerMm[1] < 1) stepsPerMm[1] = 1;
  if (stepsPerMm[2] < 1) stepsPerMm[2] = 1;

  CurrentState::getInstance()->setStepsPerMm(stepsPerMm[0], stepsPerMm[1], stepsPerMm[2]);

  axisX.loadMotorSettings(speedMax[0], speedMin[0], speedHome[0], stepsAcc[0], timeOut[0], homeIsUp[0], motorInv[0], endStInv[0], endStInv2[0], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[0], motor2Inv[0], endStEnbl[0], motorStopAtHome[0], motorMaxSize[0], motorStopAtMax[0]);
  axisY.loadMotorSettings(speedMax[1], speedMin[1], speedHome[1], stepsAcc[1], timeOut[1], homeIsUp[1], motorInv[1], endStInv[1], endStInv2[1], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[1], motor2Inv[1], endStEnbl[1], motorStopAtHome[1], motorMaxSize[1], motorStopAtMax[1]);
  axisZ.loadMotorSettings(speedMax[2], speedMin[2], speedHome[2], stepsAcc[2], timeOut[2], homeIsUp[2], motorInv[2], endStInv[2], endStInv2[2], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[2], motor2Inv[2], endStEnbl[2], motorStopAtHome[2], motorMaxSize[2], motorStopAtMax[2]);

  primeMotors();
}

//------------------------------------------------------------------------------------------
bool StepperControl::intToBool(int value)
{
  if (value == 1){
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------------------
void StepperControl::loadEncoderSettings()
{

  // Load encoder settings

  motorConsMissedStepsMax[0] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_MAX_X);
  motorConsMissedStepsMax[1] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_MAX_Y);
  motorConsMissedStepsMax[2] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_MAX_Z);

  motorConsMissedStepsDecay[0] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_DECAY_X);
  motorConsMissedStepsDecay[1] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_DECAY_Y);
  motorConsMissedStepsDecay[2] = ParameterList::getInstance()->getValue(ENCODER_MISSED_STEPS_DECAY_Z);

  motorConsMissedStepsDecay[0] = motorConsMissedStepsDecay[0] / 100;
  motorConsMissedStepsDecay[1] = motorConsMissedStepsDecay[1] / 100;
  motorConsMissedStepsDecay[2] = motorConsMissedStepsDecay[2] / 100;

  motorConsMissedStepsDecay[0] = min(max(motorConsMissedStepsDecay[0], 0.01), 99);
  motorConsMissedStepsDecay[1] = min(max(motorConsMissedStepsDecay[1], 0.01), 99);
  motorConsMissedStepsDecay[2] = min(max(motorConsMissedStepsDecay[2], 0.01), 99);

  motorConsEncoderType[0] = ParameterList::getInstance()->getValue(ENCODER_TYPE_X);
  motorConsEncoderType[1] = ParameterList::getInstance()->getValue(ENCODER_TYPE_Y);
  motorConsEncoderType[2] = ParameterList::getInstance()->getValue(ENCODER_TYPE_Z);

  motorConsEncoderScaling[0] = ParameterList::getInstance()->getValue(ENCODER_SCALING_X);
  motorConsEncoderScaling[1] = ParameterList::getInstance()->getValue(ENCODER_SCALING_Y);
  motorConsEncoderScaling[2] = ParameterList::getInstance()->getValue(ENCODER_SCALING_Z);

  motorConsEncoderUseForPos[0] = ParameterList::getInstance()->getValue(ENCODER_USE_FOR_POS_X);
  motorConsEncoderUseForPos[1] = ParameterList::getInstance()->getValue(ENCODER_USE_FOR_POS_Y);
  motorConsEncoderUseForPos[2] = ParameterList::getInstance()->getValue(ENCODER_USE_FOR_POS_Z);

  motorConsEncoderInvert[0] = ParameterList::getInstance()->getValue(ENCODER_INVERT_X);
  motorConsEncoderInvert[1] = ParameterList::getInstance()->getValue(ENCODER_INVERT_Y);
  motorConsEncoderInvert[2] = ParameterList::getInstance()->getValue(ENCODER_INVERT_Z);

  if (ParameterList::getInstance()->getValue(ENCODER_ENABLED_X) == 1){
    motorConsEncoderEnabled[0] = true;
  }
  else{
    motorConsEncoderEnabled[0] = false;
  }

  if (ParameterList::getInstance()->getValue(ENCODER_ENABLED_Y) == 1){
    motorConsEncoderEnabled[1] = true;
  }
  else{
    motorConsEncoderEnabled[1] = false;
  }

  if (ParameterList::getInstance()->getValue(ENCODER_ENABLED_Z) == 1){
    motorConsEncoderEnabled[2] = true;
  }
  else{
    motorConsEncoderEnabled[2] = false;
  }
}

//------------------------------------------------------------------------------------------
unsigned long StepperControl::getMaxLength(unsigned long lengths[3])
{
  unsigned long max = lengths[0];
  for (int i = 1; i < 3; i++){
    if (lengths[i] > max){
      max = lengths[i];
    }
  }
  return max;
}

//------------------------------------------------------------------------------------------
void StepperControl::enableMotors()
{
  motorMotorsEnabled = true;

  axisX.enableMotor();
  axisY.enableMotor();
  axisZ.enableMotor();

  delay(100);
}

//------------------------------------------------------------------------------------------
void StepperControl::disableMotorsEmergency()
{
  motorMotorsEnabled = false;

  axisX.disableMotor();
  axisY.disableMotor();
  axisZ.disableMotor();
}

//------------------------------------------------------------------------------------------
void StepperControl::disableMotors()
{
  motorMotorsEnabled = false;

  if (motorKeepActive[0] == 0) { axisX.disableMotor(); }
  if (motorKeepActive[1] == 0) { axisY.disableMotor(); }
  if (motorKeepActive[2] == 0) { axisZ.disableMotor(); }

  delay(100);
}

//------------------------------------------------------------------------------------------
void StepperControl::primeMotors()
{
  if (motorKeepActive[0] == 1) { axisX.enableMotor(); } else { axisX.disableMotor(); }
  if (motorKeepActive[1] == 1) { axisY.enableMotor(); } else { axisY.disableMotor(); }
  if (motorKeepActive[2] == 1) { axisZ.enableMotor(); } else { axisZ.disableMotor(); }
}

//------------------------------------------------------------------------------------------
bool StepperControl::motorsEnabled()
{
  return motorMotorsEnabled;
}

//------------------------------------------------------------------------------------------
bool StepperControl::endStopsReached()
{
  if (axisX.endStopsReached() ||
      axisY.endStopsReached() ||
      axisZ.endStopsReached()){
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------------------
void StepperControl::storePosition()
{
  if (motorConsEncoderEnabled[0]){
    CurrentState::getInstance()->setX(encoderX.currentPosition());
  }
  else{
    CurrentState::getInstance()->setX(axisX.currentPosition());
  }

  if (motorConsEncoderEnabled[1]){
    CurrentState::getInstance()->setY(encoderY.currentPosition());
  }
  else
  {
    CurrentState::getInstance()->setY(axisY.currentPosition());
  }

  if (motorConsEncoderEnabled[2]){
    CurrentState::getInstance()->setZ(encoderZ.currentPosition());
  }
  else{
    CurrentState::getInstance()->setZ(axisZ.currentPosition());
  }
}

//------------------------------------------------------------------------------------------
void StepperControl::reportEndStops()
{
  CurrentState::getInstance()->printEndStops();
}

//------------------------------------------------------------------------------------------
void StepperControl::reportPosition()
{
  CurrentState::getInstance()->printPosition();
}

//------------------------------------------------------------------------------------------
void StepperControl::storeEndStops()
{
  CurrentState::getInstance()->storeEndStops();
}

//------------------------------------------------------------------------------------------
void StepperControl::setPositionX(long pos)
{
  axisX.setCurrentPosition(pos);
  encoderX.setPosition(pos);
}

//------------------------------------------------------------------------------------------
void StepperControl::setPositionY(long pos)
{
  axisY.setCurrentPosition(pos);
  encoderY.setPosition(pos);
}

//------------------------------------------------------------------------------------------
void StepperControl::setPositionZ(long pos)
{
  axisZ.setCurrentPosition(pos);
  encoderZ.setPosition(pos);
}

//------------------------------------------------------------------------------------------
// Handle movement by checking each axis
void StepperControl::handleMovementInterrupt(void)
{
  //if (debugMessages){
  //  i1 = micros();
  //}

  // No need to check the encoders for Farmduino 1.4
  #if defined(RAMPS_V14) || defined(FARMDUINO_V10)
    checkEncoders();
  #endif

  // handle motor timing
  axisX.incrementTick();
  axisY.incrementTick();
  axisZ.incrementTick();

  /*
  axisServiced = axisServicedNext;
  switch (axisServiced){
    case 0:
      axisX.incrementTick();
      axisX.checkTiming();
      axisServicedNext=1;
      break;
    case 1:
      axisY.incrementTick();
      axisY.checkTiming();
      axisServicedNext=2;
      break;
    case 2:
      axisZ.incrementTick();
      axisZ.checkTiming();
      axisServicedNext=0;
      break;
  }
  */

  //if (debugMessages){
  //  i2 = micros();
  //  i3 = i2 - i1;
  //  if (i3 > i4){
  //    i4 = i3;
  //  }
  //}
}

//------------------------------------------------------------------------------------------
void StepperControl::checkEncoders()
{
  // read encoder pins using the arduino IN registers instead of digital in
  // because it used much fewer cpu cycles

  //encoderX.shiftChannels();
  //encoderY.shiftChannels();
  //encoderZ.shiftChannels();

  /*
  Serial.print("R99");
  Serial.print(" PINA ");
  Serial.print(PINA);
  Serial.print(" ");
  Serial.print((bool)(PINA & 0x02));
  Serial.print((bool)(PINA & 0x08));
  Serial.print(" Y ");
  Serial.print(digitalRead(23));
  Serial.print(digitalRead(25));
  Serial.print("\r\n");
  */
    
    if(ENCODER_ENABLED_X_DEFAULT == 1){   //-2018.02.24-CGH
      // A=16/PH1 B=17/PH0 AQ=31/PC6 BQ=33/PC4
      //encoderX.checkEncoder(PINH & 0x02, PINH & 0x01, PINC & 0x40, PINC & 0x10);
      //encoderX.setChannels(PINH & 0x02, PINH & 0x01, PINC & 0x40, PINC & 0x10);
      encoderX.checkEncoder(
        ENC_X_A_PORT   & ENC_X_A_BYTE,
        ENC_X_B_PORT   & ENC_X_B_BYTE,
        ENC_X_A_Q_PORT & ENC_X_A_Q_BYTE,
        ENC_X_B_Q_PORT & ENC_X_B_Q_BYTE);
    }    

    if(ENCODER_ENABLED_Y_DEFAULT == 1){   //-2018.02.24-CGH    
      // A=23/PA1 B=25/PA3 AQ=35/PC2 BQ=37/PC0
      //encoderY.checkEncoder(PINA & 0x02, PINA & 0x08, PINC & 0x04, PINC & 0x01);
      //encoderY.setChannels(PINA & 0x02, PINA & 0x08, PINC & 0x04, PINC & 0x01);
      encoderY.checkEncoder(
        ENC_Y_A_PORT   & ENC_Y_A_BYTE,
        ENC_Y_B_PORT   & ENC_Y_B_BYTE,
        ENC_Y_A_Q_PORT & ENC_Y_A_Q_BYTE,
        ENC_Y_B_Q_PORT & ENC_Y_B_Q_BYTE);
    }    

    if(ENCODER_ENABLED_Z_DEFAULT == 1){   //-2018.02.24-CGH    
      // A=27/PA5 B=29/PA7 AQ=39/PG2 BQ=41/PG0
      //encoderZ.checkEncoder(PINA & 0x20, PINA & 0x80, PING & 0x04, PING & 0x01);
      //encoderZ.setChannels(PINA & 0x20, PINA & 0x80, PING & 0x04, PING & 0x01);
      encoderZ.checkEncoder(
        ENC_Z_A_PORT   & ENC_Z_A_BYTE,
        ENC_Z_B_PORT   & ENC_Z_B_BYTE,
        ENC_Z_A_Q_PORT & ENC_Z_A_Q_BYTE,
        ENC_Z_B_Q_PORT & ENC_Z_B_Q_BYTE);
    }
    
  //encoderX.processEncoder();
  //encoderY.processEncoder();
  //encoderZ.processEncoder();
}

//------------------------------------------------------------------------------------------

