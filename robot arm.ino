#include <Wire.h>

#include <MPU6050.h>

#include <Servo.h>

Servo servo1, servo2, servo3, servo4;
int servo1_pin = 2, servo2_pin = 3, servo3_pin = 4, servo4_pin = 5;
int angle = 0, LastAngle = 91, angle2=0, LastAngle2 = 90;
int ch = 0;

MPU6050 sensor ;
int16_t ax, ay, az ;
int16_t gx, gy, gz, Gy, Gz, Gx;

void setup ( )

{

  servo1.attach ( servo1_pin );
  servo2.attach ( servo2_pin );
  servo3.attach ( servo3_pin );
  servo4.attach ( servo4_pin );

  servo1.write (90);
  servo2.write (90);
  servo3.write (90);
  servo4.write (110);
 
  Wire.begin ( );
  Serial.begin  (9600);

  Serial.println  ( "Initializing the sensor" );
  sensor.initialize ( );
  Serial.println (sensor.testConnection ( ) ? "Successfully Connected" : "Connection failed");
  delay (1000);
  Serial.println ( "Taking Values from the sensor" );
  delay (1000);

}




void loop ( )
{
  motion();
  Rotate();
  grib();

}


// Function for controlling the motion of the arm
void motion() {
  
  sensor.getMotion6 (&ax, &ay, &az, &gx, &gy, &gz); //getting MPU readings
  Gy = gy / 100; // normalize the value
 
  if (Gy > 5) {  // chek if the motion is in the positive direction or not and 5 is a threshold to avoid minimum readings (noise)
    Gy = map (Gy, 5, 300, 0, 180) ; // map the sensor value to angle from 0 : 180
    angle = Gy;
  /////////////////////////////////This section for moving the arm in a new DOF/////////////////////////
    if (LastAngle <= 90) {
      if (LastAngle + angle > 90) {
        servo1.write(90);
        servo2.write(90);
        LastAngle = 91;
      }
      else {

        angle = abs((angle + LastAngle));
        servo1.write (90);
        servo2.write (180 - angle);
        LastAngle = angle;

      }

    }
//////////////////////////////////////////////////////////////////////////////////////////////////////
    else {
      if ((LastAngle + angle) > 180) { //check if the last angle pluse the new angle > 180, so, you reach the maximum position then go to 180
        LastAngle = 180;
        servo1.write (180);
        servo2.write (180);
      }
      else { //else move to the required angle
        servo1.write (LastAngle + angle);
        servo2.write (LastAngle + angle);
        LastAngle = LastAngle + angle;
      }
          }
  }

  else if (Gy < -5) { //chek if the motion is in the negative direction or not and 5 is a threshold to avoid minimum readings (noise)

    Gy = map (Gy, -300, -5, -180, 0) ; // map the sensor value to angle from -180 : 0 becuse the motion is in the oposite direction

//////////////////////////////////This section for moving the arm in a new DOF/////////////////////////
    if (LastAngle <= 90) {
      if (abs(Gy) > LastAngle) {
        servo1.write(90);
        servo2.write(180);
        LastAngle = 0; //update last angle
      }
    
      else {

        angle = abs((Gy + LastAngle));
        servo1.write (90);
        servo2.write (180 - angle);
        LastAngle = angle; //update last angle

      }

/////////////////////////////////////////////////////////////////////////////////////////////////////

    }
    else { 
      if (abs(Gy) > LastAngle || abs((Gy + LastAngle)) < 90) // chech if the  new angle is greater than the last angle.....
                                                             // this means you need to go to the origin so move the servo to angle 90 

      {
        servo1.write (90);
        servo2.write (90);
        LastAngle = 90;  //update last angle
      }

      else {
        angle = abs((Gy + LastAngle));
        servo1.write (angle);
        servo2.write (angle);
        LastAngle = angle;  //update last angle
      }
    }
  }
}

// Function controlling the rotation of the arm
void Rotate() {
  sensor.getMotion6 (&ax, &ay, &az, &gx, &gy, &gz); //getting MPU readings
  Gz = gz / 100; // normalize the value
  
  if (Gz > 5) {  // threshold to avoid minimum readings (noise) and check if the motion of the MPU is in the positive direction 
    
    Gz = map (Gz, 5, 300, 0, 180) ;// map the sensor value to angle from 0 : 180
    angle2 = Gz;
    if ((LastAngle2 + angle2) > 180) { //check if the last angle pluse the new angle > 180, so, you reach the maximum position then go to 180
      LastAngle2 = 180;  //update last angle
      servo3.write (180);

    }
    else { // if the new angle + last angle < 180 so, move to this angle (means you didn't reach the maximum position)
      servo3.write (LastAngle2 + angle2);
      LastAngle2 = LastAngle2 + angle2; //update last angle
    }
  }

  else if (Gz < -5) { // threshold to avoid minimum readings (noise) and check if the motion of the MPU is in the negative direction 
    
    Gz = map (Gz, -300, -5, -180, 0) ; // map the sensor value to angle from -180 : 0 becuse the motion is in the oposite direction
 
    if (abs(Gz) > LastAngle2) { // chech if the  new angle is greater than the last angle this means you need to go to the origin so move the servo to angle 0 
      servo3.write (0);
      LastAngle2 = 0; //update last angle
    }

    else { // else move to the required angle 
 
      angle2 = abs((Gz + LastAngle2));
      servo3.write (angle2);
      LastAngle2 = angle2; //update last angle
    }

  }


}


//Function for controlling the griber
void grib() {

  if (Serial.available() > 0) { // check if there something sent 
    ch = Serial.read();
    if (ch == 99) { // ch==99 means the coming char is 'C' so close the griber
      Serial.println("closing the griber");
      servo4.write(160);

    }

    else if (ch == 111) { // ch==111 means the coming char is 'O' so open the griber
      Serial.println("opening the griber");
      servo4.write(110);
    }

  }
}
