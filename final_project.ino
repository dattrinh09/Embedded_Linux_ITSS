#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <BH1750.h>
#include <Stepper.h>

void TaskRainSensor(void * pvParameters);
void TaskLightSensor(void * pvParameters);
void TaskStepperMotor(void * pvParameters);

int rainCheck = 0;
int lightCheck = 0;
int clockWise = 0;
int countS = 0;
int rainThresholdValue = 30;
int lightThresholdValue = 500;
BH1750 lightMeter;
int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600);
  xTaskCreate(
    TaskRainSensor
    , "Task Rain"
    , 128
    , NULL
    , 1
    , NULL
  );
  xTaskCreate(
    TaskLightSensor
    , "Task Light"
    , 128
    , NULL
    , 1
    , NULL
  );
  xTaskCreate(
    TaskStepperMotor
    , "Task Stepper"
    , 128
    , NULL
    , 2
    , NULL
  );
  vTaskStartScheduler();
}

void loop()
{}

void TaskRainSensor(void * pvParameters) {
  pinMode(A0, INPUT);
  while (1) {
    int sensorValue = analogRead(A0);
    int percentValue = map(sensorValue,0,1023,100,0);
    Serial.print("Rain Percent: ");
    Serial.print(percentValue);
    Serial.println(" %");
    if (percentValue > rainThresholdValue) {
      rainCheck = 1;
    }
    else {
      rainCheck = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void TaskLightSensor(void * pvParameters) {
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
  while(1){
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
    if (lux > lightThresholdValue) {
      lightCheck = 1;
    } else {
      lightCheck = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void TaskStepperMotor(void * pvParameters){
  myStepper.setSpeed(10);
  while(1){
    if(lightCheck == 1 || rainCheck == 1){
      if(clockWise == 0){
        Serial.println("clockwise");
        while(countS < 4){
          myStepper.step(stepsPerRevolution);
          countS++;     
        }
        clockWise = 1;
        countS = 0;
      }
    }else{
      if(clockWise == 1){
        Serial.println("counterclockwise");
        while(countS < 4){
          myStepper.step(-stepsPerRevolution); 
          countS++;
        }
        clockWise = 0;
        countS = 0;
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
