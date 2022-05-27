#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include "Adafruit_MPRLS.h"

#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin

TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;

Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);
int motorPin = 3, valvePin = 4;

void setup()
{  
  pinMode(motorPin, OUTPUT);
  pinMode(valvePin, OUTPUT);
  Serial.begin(9600);
  if (! mpr.begin()) {
    while (1) {
      delay(10);
    }
  }

  /* Create 4-tasks with priorities 1-4. Capture the Task details to respective handlers */
  xTaskCreate(MyTask1, "Task1", 100, NULL, 1, &TaskHandle_1);
  xTaskCreate(MyTask2, "Task2", 100, NULL, 2, &TaskHandle_2);  
}


void loop()
{ // Hooked to Idle Task, will run when CPU is Idle
  Serial.println(F("Loop function"));
  delay(50);
      vTaskResume(TaskHandle_4);
}

/* Task1 with priority 3 */
static void MyTask1(void* pvParameters)
{
    vTaskSuspend(TaskHandle_2);

    String incomingByte;
    while(1)
    {
if (Serial.available()) {
            incomingByte = Serial.readStringUntil('\n');
          }
          if (incomingByte == "GO"){
            break;
          }
     }
    analogWrite(motorPin, 255);
    analogWrite(valvePin, 255);
    vTaskResume(TaskHandle_4);
    vTaskDelete(NULL);
}

/* Task2 with priority 4 */
static void MyTask2(void* pvParameters)
{
    float nMax = 0;
    int count = 0;
    bool vFlag = false;
    bool dFlated = false;
    vTaskSuspend(TaskHandle_1);

    Serial.print("psi"); 
    Serial.print(" \t");
    Serial.print("pHg");
    Serial.print(" \t");
    Serial.print("fPhg");
    Serial.print(" \t");
    Serial.print("nMax");
    Serial.print(" \t");
    Serial.print("count");
    Serial.println(" \t");

    while(1)
    {   
     float psi = (((mpr.readPressure())/ 68.947572932)-14.4159);
     float pHg = psi*51.715;
     float fPhg = (fPhg*.88)+((1-.88)*pHg);
     Serial.print(psi); 
     Serial.print(" \t");
     Serial.print(pHg);
     Serial.print(" \t");
     Serial.print(fPhg);
     Serial.print(" \t");
     Serial.print(nMax);
     Serial.print(" \t");
     Serial.print(count);
     Serial.println(" \t");
     delay(50);      

     if(nMax == max(nMax,fPhg ))
     {
      count++;   
     }  
     else
     {
      count = 0; 
     }
     if(count >= 3)
     {
      vFlag = true;
      analogWrite(motorPin, 0);
      analogWrite(valvePin, 0);
      delay(5);
      analogWrite(valvePin, 255);
     }

     if(vFlag && pHg < 40)
     {
      analogWrite(valvePin, 0);
      dFlated = true;
     }
     if(vFlag && dFlated)
     {
      break; 
     }
      nMax = max(nMax,fPhg );
    }
    vTaskResume(TaskHandle_1);
}
