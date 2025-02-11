// Pin definitions for the IR sensors  
const int LEFTMOST_IR_PIN = A4;    // Pin for the leftmost IR sensor  
const int LEFT_IR_PIN = A3;        // Pin for the left IR sensor  
const int MIDDLE_IR_PIN = A2;      // Pin for the middle IR sensor  
const int RIGHT_IR_PIN = 13;       // Pin for the right IR sensor  
const int RIGHTMOST_IR_PIN = A0;   // Pin for the rightmost IR sensor  

// Pin definitions for the motors  
#define LEFT_MOTOR_ENB 9 // ENB PWM pin for the left motor  
#define RIGHT_MOTOR_IN2  8  // Direction pin for the left motor  
#define RIGHT_MOTOR_IN1 7  
#define LEFT_MOTOR_IN4 12  
#define RIGHT_MOTOR_ENA 10 // ENA PWM pin for the right motor  
#define LEFT_MOTOR_IN3 11 // Direction pin for the right motor  

const int Trig = 3;
const int Echo = 4;

long duration;
float distance;
int Threshold = 20;//cm

// #include <Servo.h>
int pos = 0;
//Servo servo_6;

// Motor speed and tuning parameters  
// Function prototypes  
void FollowingLine();   
void MotorControl(int leftSpeed, int rightSpeed);  

// Initialization function  
void setup() {  
    Serial.begin(9600);  
    
    // Initialize IR sensor pins as INPUT  
    pinMode(LEFTMOST_IR_PIN, INPUT);  
    pinMode(LEFT_IR_PIN, INPUT);  
    pinMode(MIDDLE_IR_PIN, INPUT);  
    pinMode(RIGHT_IR_PIN, INPUT);  
    pinMode(RIGHTMOST_IR_PIN, INPUT);  
    
    // Initialize motor pins  
    pinMode(LEFT_MOTOR_ENB, OUTPUT);  
    pinMode(RIGHT_MOTOR_IN1, OUTPUT);   
    pinMode(RIGHT_MOTOR_IN2, OUTPUT);  
    pinMode(RIGHT_MOTOR_ENA, OUTPUT);  
    pinMode(LEFT_MOTOR_IN3, OUTPUT);  
    pinMode(LEFT_MOTOR_IN4, OUTPUT);    

    pinMode(Trig, OUTPUT);
    pinMode(Echo,INPUT);

  //  servo_6.attach(6, 500, 2500);
  
}

void loop() {
  UltraSound();  
  int error = calculateError();  
  if (distance < Threshold && error !=5){
  //UTurn
  MotorControl(0,0);
  delay(30);
  MotorControl(0, 255);  
  delay(575);  //to be checked 
  }
  else {
    FollowingLine();  
  } 
}  
void pickTreasure(){
  for (pos = 0; pos <= 90; pos += 10) {
  // tell servo to go to position in variable 'pos'
  servo_6.write(pos);
  // wait 15 ms for servo to reach the position
  delay(150); // Wait for 15 millisecond(s)
  }
  for (pos = 90; pos >= 0; pos -= 10) {
   // tell servo to go to position in variable 'pos'
  servo_6.write(pos);
    // wait 15 ms for servo to reach the position
  delay(150); // Wait for 15 millisecond(s)
  break;
  }

}
void UltraSound(){
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(Trig, LOW);
  duration=(pulseIn(Echo,HIGH));
  distance = duration*0.034/2;
  Serial.print(distance);
  Serial.println("cm");
  delay(100);
  return(distance);
  
}
// Function to read IR sensors  
// in general IR light on => low returns 0 (reversed here) on digitalRead ==> reflected IR
void ReadLFSensors(int &leftmost, int &left, int &middle, int &right, int &rightmost) {  
    leftmost = !digitalRead(LEFTMOST_IR_PIN);  
    left = !digitalRead(LEFT_IR_PIN);  
    middle = !digitalRead(MIDDLE_IR_PIN);  
    right = !digitalRead(RIGHT_IR_PIN);  
    rightmost = ! digitalRead(RIGHTMOST_IR_PIN);  

    Serial.print("LeftMost: "); Serial.print(leftmost);  
    Serial.print(", Left: "); Serial.print(left);  
    Serial.print(", Middle: "); Serial.print(middle);  
    Serial.print(", Right: "); Serial.print(right);  
    Serial.print(", RightMost: "); Serial.println(rightmost);  
    
    delay(500); // Optional: Add delay for readability  

}  

// Function to calculate the error based on IR sensor readings  
int calculateError() {  
    int leftmost, left, middle, right, rightmost;  
    ReadLFSensors(leftmost, left, middle, right, rightmost);  
    
    // Determine the error based on sensor readings  
    if (middle == LOW && left == HIGH && right == HIGH) {  //LOW means path detected
        return 0; // Robot is centered on the line  
    } else if (left == LOW && middle == LOW && right == HIGH) {  
        return -1; // Robot should turn left  
    } else if (right == LOW && middle == LOW && left == HIGH) {  
        return 1; // Robot should turn right  
    } else if (left == LOW && middle == HIGH && right == HIGH)  {  
        return -2; // Sharp turn left needed  
    } else if (right == LOW && middle == HIGH && left == HIGH) {  
        return 2; // Sharp turn right needed  
    } else if (rightmost == LOW && leftmost == HIGH) {  
        return 3; // 90 degree turn right needed  
    } else if (leftmost == LOW && rightmost == HIGH) {  
        return -3; // 90 degree turn left needed  
    }else if (leftmost == LOW && left == LOW && middle == LOW && right == LOW && rightmost == LOW) {  //cross or T
        return 5;
    } else if (leftmost == HIGH && left == HIGH && middle == HIGH && right == HIGH && rightmost == HIGH){  
        return 10; // No line detected  
    }  
}  

// Function to control the motors   
void MotorControl(int leftSpeed, int rightSpeed) {  
    // Control left motor  
    analogWrite(LEFT_MOTOR_ENB, leftSpeed); // Set speed for left motor  
    digitalWrite(LEFT_MOTOR_IN3, HIGH); // Set direction for left motor to turn forward
    digitalWrite(LEFT_MOTOR_IN4, LOW);  
    
    // Control right motor  
    analogWrite(RIGHT_MOTOR_ENA, rightSpeed); // Set speed for right motor  
    digitalWrite(RIGHT_MOTOR_IN1, LOW); // Set direction for right motor to turn forward
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
}  
void path(){

  
}
// Implementation of the FollowingLine function  
void FollowingLine() {  
    
    int error = calculateError();  

    switch(error) {  
        case 1: // Both right and middle sensors detect black  
            Serial.println("Turning Right (error value 1)");  
            MotorControl(255, 105);  
            delay(100);  //to be checked
            break;  
        
        case 2: // Only the right sensor detects black  
            Serial.println("Turning Right (error value 2)");  
            MotorControl(230, 80);  
            delay(140);  //to be checked 
            break;   
        case -1: // Both middle and left detect black  
            Serial.println("Turning Left (error value -1)");  
            MotorControl(105, 255);  
            delay(100);  //to be checked                              
            break;  
            
        case -2: // Only the left sensor detects black  
            Serial.println("Turning Left (error value -2)");  
            MotorControl(130, 255);  
            delay(140);  //to be checked  
            break;  
            
        case 3: // Only the rightmost sensor detects black  
            Serial.println("Turning Right 90");  
            MotorControl(0, 0);   
            delay(30);
            MotorControl(150, 0);  
            delay(100);  //to be checked
            break;  
            
        case -3: // Only the leftmost sensor detects black  
            Serial.println("Turning Left 90");
            MotorControl(0, 0);   
            delay(30); 
            MotorControl(0,150);  
            delay(100);  //to be checked
            break;  
            
        case 0: // robot is centered  
            Serial.println("Going Straight");
            MotorControl(180 , 150);//adjust  
            delay(500);
            break;  

        case 5: // Cross or T or destination  
            Serial.println("At junction (Cross or T)");  
            MotorControl(180, 150);  
            delay(60);  
            // Check for continuation or treasure pick-up  
            int newError = calculateError();  
            if (newError == 0 || newError == 1 || newError == 2 || newError == -1 || newError == -2 || newError == 10) { // mean cross or T  
                Serial.println("Decision at Cross - Assuming left turn"); 
                MotorControl(0,0);
                delay(30);
                MotorControl(0, 255);  
                delay(300);  //to be checked
                break;  
            } else { // Reached a destination  
                MotorControl(0,0);
                delay(30);
                MotorControl(0, 255);  //UTurn
                delay(575);  //to be checked 
                //PickTreasure();  
                break; 
            }  
        case 10: // No line   
            Serial.println("Uturn");   
            MotorControl(0,0);
            delay(30);
            MotorControl(0, 255);  
            delay(575);  //to be checked
            pickTreasure();
            break; // Ensure to stop when no line is detected  
    }  
}  

 
