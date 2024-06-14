// Step Motor Demo

#define UP_TH           1000   // Joystic Max value TH
#define DOWN_TH         100    // Joystic min value TH
#define MID_TH          520    // Joystic MID TH
#define SPEED           700    // starting speed [us]
#define JSPEED          430    // Joystick speed [us]
#define MIN_SPEED       390    // MIN DELAY
#define INTERVAL        200    // DELAY
#define MAX_HEIGHT      290    // MAX HEIGHT [mm] from base 
#define MID_STEPS       3625   // MID location steps from top Z=0
#define DIST_STEP_RATIO 0.04   // [mm/step]
#define MIN_HEIGHT      110    // MIN Height [mm]

// Stepper Motor Z
  const byte  stepPin   = 4;  //Z.STEP
  const byte  dirPin    = 7;  //Z.DIR
  const byte  zLimitPin = 11; //Z Limit
  const byte  enable    = 8;  //Shield Enable 
// joystick
int vrx = A0; //Abort pin
int vry = A1; //Hold pin 

int vrx_data = 0; 
int vry_data = 0; 

int Zlimit   = 1;

int Nsteps = 0;       // Num of steps
long height = 0;      // Height from base  im mm.
int zAxis = 0;        // Hold Z axis cordinate 
int SMSpeed = SPEED;  // Stepper Motor Speed - delay in micro sec
bool print_once = true; // use to print once

static unsigned long timer = 0;
unsigned long interval     = INTERVAL;


void setup() {
 // Sets the two pins as Outputs
    Serial.begin(9600); 
    pinMode(stepPin,OUTPUT); 
    pinMode(dirPin,OUTPUT);
    pinMode(enable,OUTPUT);
    
    pinMode(zLimitPin, INPUT_PULLUP);
    
    pinMode(vrx , INPUT); 
    pinMode(vry , INPUT); 

    // Move UP till reaching limit - Start from known location
    Serial.println("Start Z location setup");
    digitalWrite(enable,LOW);
    digitalWrite(dirPin,LOW); // GO UP

    while (Zlimit)
    {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(SPEED); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(SPEED); 
        Zlimit = digitalRead(zLimitPin);
        //delayMicroseconds(SMSpeed); 

    }
    Serial.println("Done Z location setup");
    height = MAX_HEIGHT;
    zAxis  = 0;
    Nsteps = 0; //Location  Z = 0 // MAX_HEIGHT*RATIO;

    Serial.print("Max Nsteps:");    
    Serial.print(Nsteps); 
    Serial.print("Max Heigt:");
    Serial.println(height); 

    digitalWrite(enable,LOW);
    digitalWrite(dirPin,HIGH); // GO DOWN
    for (int i=0 ; i<MID_STEPS; i++) {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(SPEED); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(SPEED);  
        Nsteps-=1;
    }
    zAxis  = Nsteps*DIST_STEP_RATIO; 
    height = MAX_HEIGHT + zAxis;
    Serial.print("Mid location: Nsteps:");    
    Serial.print(Nsteps); 
    Serial.print(" zAxis[mm]:");
    Serial.print(zAxis); 
    Serial.print(" Heigt[mm]:");
    Serial.println(height); 

    digitalWrite(enable,HIGH); // disable motor
    SMSpeed = JSPEED; // move to joy speed
}

 
void loop() 
 {
  
   joystick(); 
   Zlimit = digitalRead(zLimitPin);
       
}

void joystick()
{
    vrx_data = analogRead(vrx);
    vry_data = analogRead(vry);
/*
    Serial.print("Vrx:"); 
    Serial.print(vrx_data); 
    Serial.print(" Vry:"); 
    Serial.print(vry_data);
    Serial.print(" Speed:"); 
    Serial.println(SMSpeed); 
*/ 

    if (vrx_data > UP_TH ) 
    {
        SMSpeed+=10;
        Serial.print("Speed:");
        Serial.println(SMSpeed); 
    } 

    if ((vrx_data < DOWN_TH )&&(SMSpeed > MIN_SPEED)) 
    {
        SMSpeed-=10;
        Serial.print("Speed:");
        Serial.println(SMSpeed);    
    }

  
    // to stop the stepper motor
    if ( (vry_data > MID_TH-10)&&(vry_data < MID_TH+10)  )
    {            
        
        if (print_once) {   
            digitalWrite(enable,HIGH); // STOP All motors

            Serial.print("Stop");
            Serial.print("| Speed:");
            Serial.print(SMSpeed);
            Serial.print(" Nsteps:");
            Serial.print(Nsteps); 
            Serial.print(" Heigt:");
            Serial.println(height); 
            print_once = false; 
        }

    }

 
    if (Zlimit &&(vry_data > UP_TH)) //Up if limit is not reached
    {
        digitalWrite(enable,LOW);
        digitalWrite(dirPin,LOW);
        Nsteps+=1; 
        zAxis  = Nsteps*DIST_STEP_RATIO; 
        height = MAX_HEIGHT + zAxis;
        //Serial.print("Up:");
        //Serial.println(vrx_data); 
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(SMSpeed); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(SMSpeed); 
        print_once = true;
    
    }




    if ( (vry_data < DOWN_TH)&&(height>MIN_HEIGHT)) //down till min heigt reached
    {
        //Serial.print("Down:");
        //Serial.println(vrx_data);
        digitalWrite(enable,LOW);
        digitalWrite(dirPin,HIGH);
        Nsteps-=1;
        zAxis  = Nsteps*DIST_STEP_RATIO; 
        height = MAX_HEIGHT + zAxis;
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(SMSpeed); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(SMSpeed);  
        print_once = true;
    }


}
