/* Grove - Dust Sensor v2.0
 added LED that responds to moving average
 Interface to Shinyei Model PPD42NS Particle Sensor
 
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 
 JST Pin 1 (Black Wire)  => Arduino GND
 JST Pin 2 (Brown) => Arduino Pin 7 (P2, variable output)
 JST Pin 3 (Red wire)    => Arduino 5VDC
 JST Pin 4 (Yellow wire) => Arduino Digital Pin 8 (P1, fixed 1mu output)
 JST Pin 5 (Orange) => Arduino Pin 13 (T1, threshold for P2)
 
Dylos Air Quality Chart - Small Count Reading (0.5 micron)+

3000 +     = VERY POOR
1050-3000  = POOR
300-1050   = FAIR
150-300    = GOOD
75-150     = VERY GOOD
0-75       = EXCELLENT

Converted to 1 micron + (or whatever shinyei reads) (/4)
750+ = VERY POOR
262.5 - 750 = POOR
75 - 262.5 = FAIR
37.5 - 75 = GOOD
18.75 - 37.5 = VERY GOOD
0 - 18.75 = TOTALLY EXCELLENT, DUDE!
 */

int redpin = 4; // select the pin for the red LED
int greenpin = 5 ;// select the pin for the green LED
int bluepin = 6; // select the pin for the blue LED

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
bool ledON = 0;                // turn LED on or off


int P1pin = 8;
int P2pin = 7;
int T1pin = 13;
unsigned long P1duration;
unsigned long P2duration;
unsigned long startTime_ms;
unsigned long sampleTime_ms = 3000;//sample time 1s;
unsigned long P1lowPulseOccupancy = 0;
unsigned long P2lowPulseOccupancy = 0;
float ledSignal = 0;
float P1ratio = 0;
float P1concentration = 0;
float P2ratio = 0;
float P2concentration = 0;
float smallConcentration = 0;
bool firstTime = true;
int firstTimeCounter = 1;

void setup() {
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  Serial.begin(9600);
  pinMode(P1pin,INPUT);
  pinMode(P2pin,INPUT);
  pinMode(T1pin,OUTPUT);
  startTime_ms = millis();//get the current time;
  
  // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}

void loop() {
  analogWrite(T1pin,255);
  P1duration = pulseIn(P1pin, LOW);
  P2duration = pulseIn(P2pin, LOW);
  P1lowPulseOccupancy = P1lowPulseOccupancy+P1duration;
  P2lowPulseOccupancy = P2lowPulseOccupancy+P2duration;

  if ((millis()-startTime_ms) > sampleTime_ms)//if the sampel time == 30s
  {
    P1ratio = P1lowPulseOccupancy/(sampleTime_ms*10.0);  // Integer percentage 0=>100
    P1concentration = 1.1*pow(P1ratio,3)-3.8*pow(P1ratio,2)+520*P1ratio+0.62; // using spec sheet curve
    P2ratio = P2lowPulseOccupancy/(sampleTime_ms*10.0);  // Integer percentage 0=>100
    P2concentration = 1.1*pow(P2ratio,3)-3.8*pow(P2ratio,2)+520*P2ratio+0.62; // using spec sheet curve
    smallConcentration = P1concentration*4;
    Serial.println(smallConcentration); //roughly multiply by 4 to get particles > 0.5 micron
    Serial.println(P1concentration); //1mu particle concentration
    Serial.println(P2concentration); //variable particle size concentration
    P1lowPulseOccupancy = 0;
    startTime_ms = millis();
    
    
    // subtract the last reading:
    total = total - readings[index];         
    // read from the sensor:  
    readings[index] = smallConcentration; 
    // add the reading to the total:
    total = total + readings[index];       
    // advance to the next position in the array:  
    index = index + 1;                    
    
    // if we're at the end of the array...
    if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

    // calculate the average:
    average = total / numReadings; 

    // wait until moving average has been completed to use it as a judgement for LED color, otherwise use current value
    if (!firstTime) {
      ledSignal = average;
    }
    else {
      ledSignal = smallConcentration;
      firstTimeCounter += 1;
      if (firstTimeCounter > numReadings) {
        firstTime = false;
      }
    } 
    if (ledON) {
      if (ledSignal > 3000.0) { // air quality is VERY POOR
        analogWrite(redpin, 255);
        analogWrite(greenpin, 0);
        analogWrite(bluepin, 0);
        Serial.println("very poor");
      }
      else if (ledSignal > 1050.0) { // air quality is POOR
        analogWrite(redpin, 255);
        analogWrite(greenpin, 255);
        analogWrite(bluepin, 0);
        Serial.println("poor");
      }
      else if (ledSignal > 300.0) { // air quality is FAIR
        analogWrite(redpin, 255);
        analogWrite(greenpin, 0);
        analogWrite(bluepin, 255);
        Serial.println("fair");
      }
      else if (ledSignal > 150.0) { // air quality is GOOD
        analogWrite(redpin, 0);
        analogWrite(greenpin, 255);
        analogWrite(bluepin, 255);
        Serial.println("good");
      }
      else if (ledSignal > 75.0) { // air quality is VERY GOOD
        analogWrite(redpin, 0);
        analogWrite(greenpin, 0);
        analogWrite(bluepin, 255);
        Serial.println("very good");
      }
      else { // air quality is EXCELLENT (<75)
        analogWrite(redpin, 0);
        analogWrite(greenpin, 255);
        analogWrite(bluepin, 0);
        Serial.println("excellent");
      }
    }
  else {
      analogWrite(redpin, 0);
      analogWrite(greenpin, 0);
      analogWrite(bluepin, 0);
    }
  }
  
}
