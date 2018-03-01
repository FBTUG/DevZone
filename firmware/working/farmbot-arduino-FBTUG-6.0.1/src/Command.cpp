#include "Command.h"

const char axisCodes[3] = {'X', 'Y', 'Z'};
const char axisSpeedCodes[3] = {'A', 'B', 'C'};
const char speedCode = 'S';
const char parameterIdCode = 'P';
const char parameterValueCode = 'V';
const char parameterValue2Code = 'W';
const char elementCode = 'E';
const char timeCode = 'T';
const char modeCode = 'M';
const char msgQueueCode = 'Q';

CommandCodeEnum commandCodeEnum = CODE_UNDEFINED;

//------------------------------------------------------------------------------------------
Command::Command(char *commandChar)
{
char *charBuf = commandChar;
char *charPointer;
bool invalidCommand = false;

    charPointer = strtok(charBuf, " \n\r\0");
  
    if (charPointer[0] == 'G' || charPointer[0] == 'F'){
        commandCodeEnum = getGCodeEnum(charPointer);
    }
    else{
        invalidCommand = true;
        commandCodeEnum = CODE_UNDEFINED;
        return;
    }
  
    while (charPointer != NULL){
        getParameter(charPointer);
  
        charPointer = strtok(NULL, " \n\r");
    }
}

//------------------------------------------------------------------------------------------
CommandCodeEnum Command::getGCodeEnum(char *code)
{
    //Serial.print("code = "); Serial.println(code);   //-2018.02.24
    
    if (strcmp(code, "G0") == 0 || strcmp(code, "G00") == 0){
        //-改良 G00 不用 x y z 都要輸入, 可 單軸 或 兩軸 運動-2018.02.28
        axisValue[0] = -9999;   //-X-2018.02.28-沒資料不動作
        axisValue[1] = -9999;   //-Y-2018.02.28-沒資料不動作
        axisValue[2] = -9999;   //-Z-2018.02.28-沒資料不動作
        return G00;
    }
    
    if (strcmp(code, "G1") == 0 || strcmp(code, "G01") == 0) return G01;
    if (strcmp(code, "G28") == 0) return G28;
  
    if (strcmp(code, "F00") == 0 || strcmp(code, "F0") == 0) return F00;    //-CGH-2018.02.26
    
    if (strcmp(code, "F01") == 0 || strcmp(code, "F1") == 0) return F01;    //-FBTUG-2018.02.24
    if (strcmp(code, "F02") == 0 || strcmp(code, "F2") == 0) return F02;    //-FBTUG-2018.02.24
      
    //if (strcmp(code, "F03") == 0 || strcmp(code, "F3") == 0) return F03;
    
    if (strcmp(code, "F09") == 0 || strcmp(code, "F9") == 0) return F09;
  
    if (strcmp(code, "F11") == 0) return F11;
    if (strcmp(code, "F12") == 0) return F12;
    if (strcmp(code, "F13") == 0) return F13;
    if (strcmp(code, "F14") == 0) return F14;
    if (strcmp(code, "F15") == 0) return F15;
    if (strcmp(code, "F16") == 0) return F16;
    
    if (strcmp(code, "F20") == 0) return F20;
    if (strcmp(code, "F21") == 0) return F21;
    if (strcmp(code, "F22") == 0) return F22;
    
    if (strcmp(code, "F31") == 0) return F31;
    if (strcmp(code, "F32") == 0) return F32;
    
    if (strcmp(code, "F41") == 0) return F41;
    if (strcmp(code, "F42") == 0) return F42;
    if (strcmp(code, "F43") == 0) return F43;
    if (strcmp(code, "F44") == 0) return F44;
  
    if (strcmp(code, "F61") == 0) return F61;
    
    if (strcmp(code, "F81") == 0) return F81;
    if (strcmp(code, "F82") == 0) return F82;
    if (strcmp(code, "F83") == 0) return F83;
    if (strcmp(code, "F84") == 0) return F84;
  
    if (strcmp(code, "T01") == 0 || strcmp(code, "T1") == 0) return T01;    //-FBTUG-2018.02.24
    
    return CODE_UNDEFINED;
}

//------------------------------------------------------------------------------------------
double minusNotAllowed(double value)
{
    if(value < 0) return 0;
    
    return value;
}

//------------------------------------------------------------------------------------------
void Command::getParameter(char *charPointer)
{
    //msgQueue = 24;
  
    //const char axisCodes[3] = {'X', 'Y', 'Z'};
    //const char axisSpeedCodes[3] = {'A', 'B', 'C'};

    if (charPointer[0] == axisCodes[0]){    //-X
        axisValue[0] = atof(charPointer + 1);
        //msgQueue	= 77;
    }
  
    if (charPointer[0] == axisCodes[1]){    //-Y
        axisValue[1] = atof(charPointer + 1);
    }
  
    if (charPointer[0] == axisCodes[2]){    //-Z
        axisValue[2] = atof(charPointer + 1);
    }
  
    if (charPointer[0] == axisSpeedCodes[0]){   //-A
        axisSpeedValue[0] = atof(charPointer + 1);
    }
  
    if (charPointer[0] == axisSpeedCodes[1]){   //-B
        axisSpeedValue[1] = atof(charPointer + 1);
    }
  
    if (charPointer[0] == axisSpeedCodes[2]){   //-C
        axisSpeedValue[2] = atof(charPointer + 1);
    }
  
    if (charPointer[0] == speedCode){
        speedValue = atof(charPointer + 1);
    }
  
    if (charPointer[0] == parameterIdCode){
        parameterId = atof(charPointer + 1);
    }
  
    if (charPointer[0] == parameterValueCode){
        parameterValue = atof(charPointer + 1);
    }
  
    if (charPointer[0] == parameterValue2Code){
        parameterValue2 = atof(charPointer + 1);
    }
  
    if (charPointer[0] == elementCode){
        element = atof(charPointer + 1);
    }
  
    if (charPointer[0] == timeCode){
        time = atof(charPointer + 1);
    }
  
    if (charPointer[0] == modeCode){
        mode = atof(charPointer + 1);
    }
  
    if (charPointer[0] == msgQueueCode){
        msgQueue = atof(charPointer + 1);
        //msgQueue   =  5;
    }
}

//------------------------------------------------------------------------------------------
void Command::print()
{
    Serial.print("R99 Command with code: ");
    Serial.print(commandCodeEnum);
    Serial.print(", X: ");
    Serial.print(axisValue[0]);
    Serial.print(", Y: ");
    Serial.print(axisValue[1]);
    Serial.print(", Z: ");
    Serial.print(axisValue[2]);
    Serial.print(", S: ");
    Serial.print(speedValue);
    Serial.print(", P: ");
    Serial.print(parameterId);
    Serial.print(", V: ");
    Serial.print(parameterValue);
  
    Serial.print(", W: ");
    Serial.print(parameterValue2);
    Serial.print(", T: ");
    Serial.print(time);
    Serial.print(", E: ");
    Serial.print(element);
    Serial.print(", M: ");
    Serial.print(mode);
  
    Serial.print(", A: ");
    Serial.print(axisSpeedValue[0]);
    Serial.print(", B: ");
    Serial.print(axisSpeedValue[1]);
    Serial.print(", C: ");
    Serial.print(axisSpeedValue[2]);
  
    Serial.print(", Q: ");
    Serial.print(msgQueue);
  
    Serial.print("\r\n");
  
    //- 2018.02.25-CGH ----------------------
    #ifdef BT_Serial2_Use   
      Serial2.print("R99 Command with code: ");
      Serial2.print(commandCodeEnum);
      Serial2.print(", X: ");
      Serial2.print(axisValue[0]);
      Serial2.print(", Y: ");
      Serial2.print(axisValue[1]);
      Serial2.print(", Z: ");
      Serial2.print(axisValue[2]);
      Serial2.print(", S: ");
      Serial2.print(speedValue);
      Serial2.print(", P: ");
      Serial2.print(parameterId);
      Serial2.print(", V: ");
      Serial2.print(parameterValue);
    
      Serial2.print(", W: ");
      Serial2.print(parameterValue2);
      Serial2.print(", T: ");
      Serial2.print(time);
      Serial2.print(", E: ");
      Serial2.print(element);
      Serial2.print(", M: ");
      Serial2.print(mode);
    
      Serial2.print(", A: ");
      Serial2.print(axisSpeedValue[0]);
      Serial2.print(", B: ");
      Serial2.print(axisSpeedValue[1]);
      Serial2.print(", C: ");
      Serial2.print(axisSpeedValue[2]);
    
      Serial2.print(", Q: ");
      Serial2.print(msgQueue);
    
      Serial2.print("\r\n");
    #endif  
}

//------------------------------------------------------------------------------------------
CommandCodeEnum Command::getCodeEnum()
{
    return commandCodeEnum;
}

//------------------------------------------------------------------------------------------
double Command::getX()
{
    return axisValue[0];
}

//------------------------------------------------------------------------------------------
double Command::getY()
{
    return axisValue[1];
}

//------------------------------------------------------------------------------------------
double Command::getZ()
{
    return axisValue[2];
}

//------------------------------------------------------------------------------------------
long Command::getA()
{
    return axisSpeedValue[0];
}

//------------------------------------------------------------------------------------------
long Command::getB()
{
    return axisSpeedValue[1];
}

//------------------------------------------------------------------------------------------
long Command::getC()
{
    return axisSpeedValue[2];
}

//------------------------------------------------------------------------------------------
long Command::getP()
{
    return parameterId;
}

//------------------------------------------------------------------------------------------
long Command::getV()
{
    return parameterValue;
}

//------------------------------------------------------------------------------------------
long Command::getW()
{
    return parameterValue2;
}

//------------------------------------------------------------------------------------------
long Command::getT()
{
  return time;
}

//------------------------------------------------------------------------------------------
long Command::getE()
{
  return element;
}

//------------------------------------------------------------------------------------------
long Command::getM()
{
  return mode;
}

//------------------------------------------------------------------------------------------
long Command::getQ()
{
  //msgQueue = 9876;
  return msgQueue;
}

//------------------------------------------------------------------------------------------
long Command::getN()    //-FBTUG-2018.02.24
{
    return numberWatercapacity;
}

//------------------------------------------------------------------------------------------

