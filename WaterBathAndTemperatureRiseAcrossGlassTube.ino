#include <Wire.h>
#include <DFRobot_ADS1115.h>
#include <DallasTemperature.h>
#include <OneWire.h>

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// define TimeInterval 187
/********************************************************************/
DFRobot_ADS1115 ads(&Wire);
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/********************************************************************/
// arrays to hold device address
DeviceAddress insideThermometer;
// Inputs and outputs

int ThermistorPin0 = A0;
int ThermistorPin1 = A1;
int firing_pin = 3;
int zero_cross = 8;

// Variables
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 0.8585084342e-03, c2 = 2.661988977e-04, c3 = 0.5596392943e-07;
float CalculateTemperature(int16_t Volt);
int16_t V;
int last_CH1_state = 0;
bool zero_cross_detected = false;
// int firing_delay = 7400;

//////////////////////////////////////////////////////
int maximum_firing_delay = 8000;//7400; micro seconds
/*Later in the code you will se that the maximum delay after the zero detection
 * is 7400. Why? Well, we know that the 220V AC voltage has a frequency of around 50-60HZ so
 * the period is between 20ms and 16ms, depending on the country. We control the firing
 * delay each half period so each 10ms or 8 ms. To amke sure we wont pass thsoe 10ms, I've made tests
 * and the 7400us or 7.4ms was a good value. Measure your frequency and chande that value later */
//////////////////////////////////////////////////////

unsigned long previousMillis = 0;
unsigned long previousMillisTempLog = 0;
unsigned long currentMillis = 0;
unsigned long Time = 0;
unsigned long timePrev = 0;
int temp_read_Delay = 500;
float real_temperature = 0;
float setpoint = 37.00;
float firing_delay = 0.0;
float PowerMax = 1600.0;
float TempThreshold = 30;
float InternalDelay = 2000; // microseconds
// bool pressed_1 = false;
// bool pressed_2 = false;

// PID variables
float PID_error = 0;
float previous_error = 0;
float elapsedTime;
float PID_value = 0;
// PID constants
// float scale = 1;// 4.33;
float kp = 60;// originally 39
float ki= 0.102;   float kd = 385;
float PID_p = 0;
float PID_i = 0;
float PID_d = 0;
unsigned long StartTime, RunningForMillis; // RunForMillis;
// unsigned long RunForMilliseconds = 55000;
// StartTime = millis();

void setup()
{
  // Define the pins
  pinMode(firing_pin, OUTPUT);
  pinMode(zero_cross, INPUT);
  Serial.begin(115200);
  ads.setAddr_ADS1115(ADS1115_IIC_ADDRESS0);   // 0x48
  ads.setGain(eGAIN_TWOTHIRDS);   // 2/3x gain
  ads.setMode(eMODE_SINGLE);       // single-shot mode
  ads.setRate(eRATE_32);          // 128SPS (default)
  ads.setOSMode(eOSMODE_SINGLE);   // Set to start a single-conversion
  ads.init();

  PCICR |= (1 << PCIE0);   // enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT0); // Set pin D8 (zero cross input) trigger an interrupt on state change.

  if (!sensors.getAddress(insideThermometer, 0))
    Serial.println("Unable to find address for Device 0");

  sensors.setResolution(insideThermometer, 12);
  digitalWrite(firing_pin, HIGH);
  // sensors.setResolution(10);
}

void loop()
{
  currentMillis = millis(); // Save the value of time before the loop

  /*  We create this if so we will read the temperature and change values each "temp_read_Delay"
   *  value. Change that value above iv you want. The MAX6675 read is slow. Tha will affect the
   *  PID control. I've tried reading the temp each 100ms but it didn't work. With 500ms worked ok.*/
  if (currentMillis - previousMillis >= temp_read_Delay)
  {                                // (currentMillis - previousMillis >= temp_read_Delay){
    
    ////////////////////////////////////////////// FOR DS18B20 ////////////////////////////////////
    // StartTime = millis();
    sensors.requestTemperatures(); // Send the command to get temperature readings
    real_temperature = sensors.getTempC(insideThermometer);
    PID_error = max(setpoint - real_temperature,0); // Calculate the pid ERROR

    if(PID_error < 5.00)                              //integral constant will only affect errors below 30ºC
      {PID_i = 0;}

    PID_p = kp * PID_error; // Calculate the P value
    PID_i = PID_i + (ki * PID_error);               //Calculate the I value //Calculate the D value 
    PID_d = kd*((PID_error - previous_error)/temp_read_Delay); 

    PID_value = PID_p + PID_i + PID_d;  //Calculate total PID value

    // We define firing delay range between 0 and 7400. Read above why 7400!!!!!!!

    ////////////////////////////////////////////////////////////////////////
    //////////////////////// FROM CURVE FITTING ////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    if (real_temperature > TempThreshold){
      if (PID_value < 10){
        firing_delay = maximum_firing_delay;
      }
      else if (PID_value < 320){
        float a = -0.005173; float b = 0.2677; float c = 0.01;
        firing_delay = (a*pow(PID_value/PowerMax,b) + c)*1e6 - InternalDelay;
      }
      else if (PID_value < 1504){
        float p1 = -0.005676; float p2 = 0.007838;
        firing_delay = max((p1*PID_value/PowerMax + p2)*1e6 - InternalDelay,0);
      }
      else
        firing_delay = 0;
    }
    
    Serial.println(PID_error);
    
    previous_error = PID_error; //Remember to store the previous error.
    previousMillis += temp_read_Delay; // Increase the previous time for next loop
  }

  if (ads.checkADS1115() && (currentMillis - previousMillisTempLog) > 1000 ){
    Serial.print(real_temperature);
    Serial.print('\t');
    Serial.print(CalculateTemperature(ads.readVoltage(0)));
    Serial.print('\t');
    Serial.println(CalculateTemperature(ads.readVoltage(1)));
    previousMillisTempLog += 1000;
  }

  // If the zero cross interruption was detected we create the 100us firing pulse

  if (zero_cross_detected && real_temperature > TempThreshold) 
  {
    delayMicroseconds(firing_delay); // This delay controls the power
    digitalWrite(firing_pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(firing_pin, LOW);
    zero_cross_detected = false;
  }
  else if (real_temperature <= TempThreshold){
    digitalWrite(firing_pin,HIGH);
  }
}

//////////////////////// THERMISTOR FUNCTION ///////////////////
float CalculateTemperature(int16_t Vo){
    R2 = R1*((5000 / (float)Vo) - 1.0); // 5000 mV supply
    logR2 = log(R2);
    T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    Tc = T - 273.15;
    return Tc;
}

// End of void loop
//  |
//  v
// See the interruption vector

// This is the interruption routine (pind D8(zero cross), D11(increase) and D12(decrease))
//----------------------------------------------

ISR(PCINT0_vect)
{
  ///////////////////////////////////////Input from optocoupler
  if (PINB & B00000001)
  { // We make an AND with the state register, We verify if pin D8 is HIGH???
    if (last_CH1_state == 0)
    {                             // If the last state was 0, then we have a state change...
      zero_cross_detected = true; // We have detected a state change! We need both falling and rising edges
    }
  }
  else if (last_CH1_state == 1)
  {                             // If pin 8 is LOW and the last state was HIGH then we have a state change
    zero_cross_detected = true; // We haev detected a state change!  We need both falling and rising edges.
    last_CH1_state = 0;         // Store the current state into the last state for the next loop
  }
}
// End of interruption vector for pins on port B: D8-D13
