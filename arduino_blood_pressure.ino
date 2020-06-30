#include <Wire.h>                // i2c library for arduino.           

// MACRO Definitions
#define BP_START_PIN (2)         // start button of the blood pressure monitor device. replaced a transistor.   
#define VALVE_PIN (3)            // checks if the measurement is done.             
#define MEASURE_BEGIN_PIN (4)    // indicates that a measurement should start. this can be connected to switch or another MCU or raspberry pi.

volatile byte i2c_data_rx;       // indicates there are available data from the i2c bus. 
volatile uint16_t count;         // indicates the total number of data collected.
volatile uint8_t sys, dia, hr;   // stored the measure values: systolic, diastolic and heart rate.

void setup()
{
  Serial.begin(115200);
  pinMode(BP_START_PIN, OUTPUT);
  pinMode(VALVE_PIN, INPUT);
  pinMode(MEASURE_BEGIN_PIN, INPUT_PULLUP);
  Wire.begin(0x50);                           // the address of the EEPROM is 0x50. The arduino should be the same.            
  Wire.onReceive(receiveEvent);               // this is the interrupt initialization for the i2c data.
}

void loop()
{
  if (digitalRead(MEASURE_BEGIN_PIN) == 0)        // The arduino is instructed to start the measurement.
  {
    digitalWrite(BP_START_PIN, HIGH);             // Emulating a push on the button.
    delay(200);
    digitalWrite(BP_START_PIN, LOW);

    delay(2000);
    Serial.println("Start BP check...");
    delay(2000);                          //need to secure that the value is already closed.
    while (digitalRead(VALVE_PIN) == 0)
    {
      Serial.println("BP On-going...");
      delay(1000);
    }
    delay(2000);

    digitalWrite(BP_START_PIN, HIGH);
    delay(200);
    digitalWrite(BP_START_PIN, LOW);

    delay(500);

    if (count == 0)
    {
      Serial.println("No data");
    }
    else if (count == 35)
    {
      Serial.print("Blood Presure Data: ");
      Serial.print(sys);
      Serial.print("/");
      Serial.println(dia);
      Serial.println("");
    }
    else
    {
      Serial.println("error");
    }

    count = 0;
  }
}

void receiveEvent(int iData)   // Interrupt service routine.
{
  if ( iData > 0 )
  {
    while ( iData-- )
    {
      i2c_data_rx = Wire.read();
      //      Serial.println(i2c_data_rx);
      count++;

      if (count == 28)
      {
        sys = i2c_data_rx;
      }
      if (count == 29)
      {
        dia = i2c_data_rx;
      }
      if (count == 30)
      {
        hr = i2c_data_rx;
      }
    }
  }
}
