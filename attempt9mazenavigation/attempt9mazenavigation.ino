//****************************************//
//* Example Code for Sending             *//
//* Signed Integers over I2C             *//
//* ESP32 (Master) to Arduino (Slave)    *//
//*                                      *//
//* Master Code                          *//
//*                                      *//
//* UoN 2022 - Nat Dacombe               *//
//****************************************//

// read through all of the code and the comments before asking for help
// research 'two's complement' if this is not familiar to you as it is used to represented signed (i.e. positive and negative) values

#include <LiquidCrystal.h>
#include <Wire.h>
#include <Keypad.h>
#include <MPU6050_tockn.h>
#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal
#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns

MPU6050 mpu6050(Wire);

int command[10];
int arrayLength = sizeof(command) / sizeof(command[0]);
char keyexecute;
char keydistance;
char keydistance1;
char keyrotation;
char keyrotation1;
int x;
int y;
int z;
int i;
int j;
int k;
int16_t enc2_count;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(33, 25, 23, 32, 3, 15);

//function to transmit data from ESP32 to Arduino NANO through I2C
void I2Cslaveaddresstransmission()
{
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
   /* depending on the mirocontroller, the int variable is stored as 32-bits or 16-bits
     if you want to increase the value range, first use a suitable variable type and then modify the code below
     for example; if the variable used to store x and y is 32-bits and you want to use signed values between -2^31 and (2^31)-1
     uncomment the four lines below relating to bits 32-25 and 24-17 for x and y
     for my microcontroller, int is 32-bits hence x and y are AND operated with a 32 bit hexadecimal number - change this if needed

     >> X refers to a shift right operator by X bits
  */
  Wire.write((byte)((x & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(x & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  //Wire.write((byte)((y & 0xFF000000) >> 24)); // bits 32 to 25 of y
  //Wire.write((byte)((y & 0x00FF0000) >> 16)); // bits 24 to 17 of y
  Wire.write((byte)((y & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(y & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1 
  Wire.write((byte)((z & 0x0000FF00) >> 8));
  Wire.write((byte)(z & 0x000000FF)); 
  Wire.endTransmission();   // stop transmitting
  //delay(100);
}

void encodercount()
{
  //enc2_count = 0;
  Wire.requestFrom(4, 2);
  uint8_t enc2_16_9 = Wire.read();   // receive bits 16 to 9 of z (one byte)
  uint8_t enc2_8_1 = Wire.read(); 
  enc2_count = (enc2_16_9 << 8) | enc2_8_1;
}

byte pin_rows[ROW_NUM] = {18, 5, 17, 16}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {4, 0, 2};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

char key;

void setup()
{
  Serial.begin(9600);
  delay(500);
  Wire.begin();   // join i2c bus (address optional for the master) - on the Arduino NANO the default I2C pins are A4 (SDA), A5 (SCL)
  lcd.begin(16, 2);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

// the minimum and maximum values here are determined by the amount of bits used by the chosen variable type
// for int, this is either 16-bits or 32-bits
// due to two's complement, the minimum value is -2^(N-1), and the maximum is (2^(N-1))-1; where N is the number of bits
// int x = 200;
// int y = -210;



void loop()
{
  lcd.clear();
  //for loop tells how many loops happened with the amount of loops corresponding to a value that is stored in that position in the array of the amount of loops that has happened
  for (i = 0;i<=arrayLength;i++){
  do    
  {
    
    {
      lcd.clear();
      lcd.print("1.FWD 0.BWD 3.GO");
      lcd.setCursor(0,1);
      // print the number of seconds since reset:
      lcd.print(" 7.CLOCK 9.ANTI ");
      //delay(5000);
      Serial.print("\npress 1 to go forward \npress 0 to go backward\npress 7 to go clockwise\npress 9 to go anticlockwise\npress 3 to execute");
      

      key = keypad.getKey();
      while (key == NO_KEY)
        {
          // Serial.println("no key pressed");
          //key = keypad.getKey();
          //Serial.print("\npress 1 to go forward \npress 0 to go backward\npress 7 to go clockwise\npress 9 to go anticlockwise\npress 3 to execute");
          //delay(500);
          key = keypad.getKey();
        }

    
      //condition on if 1 is pressed on keypad
      if (key == '1')
      {

        Serial.print("\n1 pressed");
        delay(500);
        Serial.print("\nforward by how much\npress 1 for 10 cm\npress 3 for 30 cm\npress any other key to cancel command\n");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.clear();
        lcd.print("  BY HOW MUCH:  ");
        lcd.setCursor(0,1);
        // print 1. 10  3. 30 on second line of LCD:
        lcd.print("  1. 10  3. 30  ");
        //delay(10000);
        //lcd.clear();
        keydistance = NO_KEY;
        while (keydistance == NO_KEY)
        {
          // Serial.println("no key pressed");
          keydistance = keypad.getKey();
        }

        //condition on if 1 is pressed on keypad
        if (keydistance == '1')
        {
          lcd.clear();
          Serial.print("\nGo forward 10cm");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          //LCD displays GO FORWARD 10CM
          lcd.print("GO FORWARD 10CM");
          delay(2000);
          //stores 10 to the position in array of the amount of loops that have happened
          command[i] = 10;  
          //lcd.clear();   
        }

        //condition on if 3 is pressed on keypad
        else if (keydistance == '3')
        {
          // lcd.clear();
          Serial.print("\nGo forward 30cm");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.print("GO FORWARD 30CM");
          delay(2000);
          //stores 30 to the position in array of the amount of loops that have happened
          command[i] = 30;
        }
      }

      //condition on if 0 is pressed on keypad
      else if (key == '0')
      {
        // lcd.clear();
        Serial.print("\n0 pressed");
        delay(500);
        Serial.print("\nbackward by how much\npress 1 for 10 cm\npress 3 for 30 cm\npress any other key to cancel command\n");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.clear();
        lcd.print("  BY HOW MUCH:  ");
        lcd.setCursor(0,1);
        // print 1. 10  3. 30 on second line of LCD screen:
        lcd.print("  1. 10  3. 30  ");
        keydistance1 = NO_KEY;
        while (keydistance1 == NO_KEY)
        {
          
          keydistance1 = keypad.getKey();
        }

        //condition on if 1 is pressed on keypad
        if (keydistance1 == '1')
        {
          lcd.clear();
          Serial.print("\nGo backward 10cm");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          //Displays GO BACKWARD 10CM on LCD
          lcd.print("GO BACKWARD 10CM");
          delay(2000);
          //stores -10 to the position in array of the amount of loops that have happened
          command[i] = -10;         
        }

        //condition on if 3 is pressed on keypad
        else if (keydistance1 == '3')
        {
          // lcd.clear();
          Serial.print("\nGo backward 30cm");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.print("GO BACKWARD 30CM");
          delay(2000);
          //stores -30 to the position in array of the amount of loops that have happened
          command[i] = -30;
        }
      }
      //condition on if 7 is pressed on keypad
      else if (key == '7')
      {
        lcd.clear();
        Serial.print("\n7 pressed");
        delay(500);
        Serial.print("\nhow much rotation clockwise?\n9 to turn\npress 9 to turn 90 degrees\npress 8 to turn 180 degrees\npress any other key to cancel command\n");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.clear();
        lcd.print("  BY HOW MUCH:  ");
        lcd.setCursor(0,1);
        // print 9. 90   8. 180 on second line of LCD:
        lcd.print(" 9. 90   8. 180 ");
        //delay(2000);
        keyrotation = NO_KEY;
        while (keyrotation == NO_KEY)
        {
          
          keyrotation = keypad.getKey();
        }

        //condition on if 9 is pressed on keypad
        if (keyrotation == '9')
        {
          lcd.clear();
          Serial.print("\nRotate 90 degrees");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          // displays ROTATE 180 DEGREE in second line on LCD
          lcd.print("ROTATE 90 DEGREE");
          delay(2000);
          //stores 90 to the position in array of the amount of loops that have happened
          command[i] = 90;
        }

        //condition on if 8 is pressed on keypad
        else if (keyrotation == '8')
        {
          lcd.clear();
          Serial.print("\nRotate 180 degrees");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.print("     ROTATE     ");
          lcd.setCursor(0, 1);
          // displays 180 degrees in second line on LCD
          lcd.print("  180 DEGREES  ");
          delay(2000);
          //stores 180 to the position in array of the amount of loops that have happened
          command[i] = 180;
        }
      }

      //condition on if 9 is pressed on keypad
      else if (key == '9')
      {
        lcd.clear();
        Serial.print("\n9 pressed");
        delay(500);
        Serial.print("\nhow much rotation anticlockwise?\n9 to turn\npress 9 to turn 90 degrees\npress 8 to turn 180 degrees\npress any other key to cancel command\n");
        //delay(500);
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.clear();
        lcd.print("  BY HOW MUCH:  ");
        lcd.setCursor(0,1);
        // print the number of seconds since reset:
        lcd.print(" 9. 90   8. 180 ");
        delay(2000);
        keyrotation1 = NO_KEY;
        while (keyrotation1 == NO_KEY)
        {
          //Serial.println("no key pressed");
          keyrotation1 = keypad.getKey();
        }

        //condition on if 9 is pressed on keypad
        if (keyrotation1 == '9')
        {
          lcd.clear();
          Serial.print("\nRotate 90 degrees");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.print("ROTATE 90 DEGREE");
          delay(2000);
          //stores -90 to the position in array of the amount of loops that have happened
          command[i] = -90;
        }

        //condition on if 8 is pressed on keypad
        else if (keyrotation1 == '8')
        {
          lcd.clear();
          Serial.print("\nRotate 180 degrees");
          delay(500);
          // set the cursor to column 0, line 1
          // (note: line 1 is the second row, since counting begins with 0):
          lcd.print("     ROTATE     ");
          lcd.setCursor(0, 1);
          // print the number of seconds since reset:
          lcd.print("  180 DEGREES  ");
          delay(2000);
          //stores -180 to the position in array of the amount of loops that have happened
          command[i] = -180;
        }
        //i = i+1;
      }

      //z = i;
      i = i+1;

    }     

  }
  while (key != '3');
 
  Serial.print("\nExecuted\n");
  //goes through array printing it out and executes command
  for(i = 0;i <= arrayLength;i++)
  {
    Serial.println(command[i]);
    delay(1000);
    //Serial.print(" ");
    //Serial.println("checking");    
  }
  for(j = 0;j <= 50;j++)
  {
    //when number stored in array is 10    
    if (command[j] == 10)
    {
      unsigned long previousMillis = 0;
      unsigned long interval = 5000;
      unsigned long currentTime = millis();
      enc2_count = 0;
      
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 83;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(1000);
      enc2_count = -1;
      
      //car goes forward 10cm using encoder
      do 
      {
        lcd.clear();
        encodercount();
        Serial.println("forward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        //LCD displays GO FORWARD 10CM
        lcd.print("GO FORWARD 10CM");
        x = 143;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("forward 10");
        
        encodercount();        
        x = 145;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 84;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        //enc2_count = enc2_count + 0.5;
        //1
      }
      while (enc2_count<5);
      enc2_count = 0;
       while (enc2_count<4)
      //9
      //10
      //7
      {
        lcd.clear();
        encodercount();
        Serial.println("forward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO FORWARD 10CM");
        //delay(2000);
        x = 143;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("forward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        //lcd.print("GO FORWARD 10CM");
        encodercount();        
        x = 145;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 84;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        //enc2_count = enc2_count - 0.5;
      }
      //9 instead of 10 cause EEEbot went 3 cm extra
      
    }

    //when number stored in array is -10   
    else if (command[j] == -10)
    {
      
      //backward 10
      unsigned long previousMillis = 0;
      unsigned long interval = 5000;
      unsigned long currentTime = millis();
      enc2_count = 0;
      
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 81;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(1000);
      //backward 10
      
      //car goes backward 10cm using encoder
      do
      {
        encodercount();
        lcd.clear();
        encodercount();
        Serial.println("backward 10");
        encodercount();
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 10CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        encodercount();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();  
        encodercount();    
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        encodercount();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("backward 10");
        encodercount();
      }
      //goes back 10cm
      //goes back 9 cm not 10cm cause multipe of 3
      while (enc2_count>-1);
      
      enc2_count = 0;
      
      do
      {
        lcd.clear();
        encodercount();
        Serial.println("backward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 10CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        encodercount();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();  
        encodercount();    
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        encodercount();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("backward 10");
        encodercount();
      }
      while (enc2_count>0);
      enc2_count = 0;
      do
      {
        lcd.clear();
        encodercount();
        Serial.println("backward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 10CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        encodercount();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();  
        encodercount();    
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        encodercount();
        I2Cslaveaddresstransmission();
        delay(50);
        Serial.println("backward 10");
        encodercount();
      }
      while (enc2_count>-1); 
      //-1     
      enc2_count = 0;
      do
      {
        lcd.clear();
        encodercount();
        Serial.println("backward 10");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 10CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        encodercount();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();  
        encodercount();    
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        encodercount();
        I2Cslaveaddresstransmission();
        delay(50);
        //50
        Serial.println("backward 10");
        encodercount();
      }
      while (enc2_count>-2);  
      //-2    
      
    } 

    //when number stored in array is 30 
    else if (command[j] == 30)
    {
      unsigned long previousMillis = 0;
      unsigned long interval = 5000;
      unsigned long currentTime = millis();
      enc2_count = 0;
      
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 83;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(1000);
      //forward 30 cm using encoder
      do
      {
        lcd.clear();
        encodercount();
        Serial.println("forward 30");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO FORWARD 30CM");    
        x = 143;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("forward 30");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO FORWARD 30CM");
        encodercount();
        x = 145;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -99;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 84;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
      }
      while (enc2_count<21);
    }

    //when number stored in array is -30 
    else if (command[j] == -30)
    {
      //backward 10
      unsigned long previousMillis = 0;
      unsigned long interval = 5000;
      unsigned long currentTime = millis();
      
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 81;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(1000);
      //backward 30
      //while (millis() - currentTime< 5000)
      do
      {
        lcd.clear();
        Serial.println("backward 30");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 30CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
        Serial.println("backward 30");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("GO BACKWARD 30CM");
        x = -175;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = 155;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 83;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(100);
      }
      while (enc2_count>-30);
    } 

    //when number stored in array is 90 
    else if (command[j] == 90)
    {
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 170;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(10);
      mpu6050.update();
      float initial = mpu6050.getAngleZ();
      //clockwise 90
      do
      {
        mpu6050.update();
        // initial = mpu6050.getAngleZ();
        lcd.clear();
        Serial.println("clockwise 90");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("ROTATE 90 DEGREE");
        x = 200;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = 190;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();
        z = 170;
        Serial.println("z = ");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
      }
      while (mpu6050.getAngleZ()- initial>-90);
      
    }

    //when number stored in array is -90 
    else if (command[j] == -90)
    {
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 170;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(10);
      mpu6050.update();
      float initial = mpu6050.getAngleZ();
      //anticlockwise 90 using MPU 6500
      do
      {
        mpu6050.update();
        lcd.clear();
        Serial.println("anticlockwise 90");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("ROTATE 90 DEGREE");
        x = -255;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -172;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();
        z = 170;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
      }
      while(mpu6050.getAngleZ()- initial<90);

    } 

    //when number stored in array is 180
    else if (command[j] == 180)
    {
      //stop 1 sec
        Serial.println("pause");
        x = 0;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = 0;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();      
        z = 170;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
        delay(10);
      mpu6050.update();
      float initial = mpu6050.getAngleZ();
      //clockwise 180 using MPU 6500
      do
      {
        mpu6050.update();
        lcd.clear();
        Serial.println("clockwise 180");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("     ROTATE     ");
        lcd.setCursor(0, 1);
        // print 180 DEGREES oon second line of LCd:
        lcd.print("  180 DEGREES  ");
        x = 200;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = 190;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();
        z = 170;
        Serial.println("z = ");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
      }
      while (mpu6050.getAngleZ()- initial>-180);

    }    

    //when number stored in array is -180
    else if (command[j] == -180)
    {
      //stop 1 sec
      Serial.println("pause");
      x = 0;
      Serial.println("x = ");
      Serial.print(x);
      Serial.println();
      y = 0;
      Serial.println("y = ");
      Serial.print(y);
      Serial.println();      
      z = 170;
      Serial.println("z =");
      Serial.print(z);
      Serial.println();
      I2Cslaveaddresstransmission();
      delay(10);
      mpu6050.update();
      float initial = mpu6050.getAngleZ();
      //anticlockwise 180
      do
      {
        mpu6050.update();
        lcd.clear();
        Serial.println("anticlockwise 180");
        // set the cursor to column 0, line 1
        // (note: line 1 is the second row, since counting begins with 0):
        lcd.print("     ROTATE     ");
        lcd.setCursor(0, 1);
        // print the number of seconds since reset:
        lcd.print("  180 DEGREES  ");
        x = -255;
        Serial.println("x = ");
        Serial.print(x);
        Serial.println();
        y = -172;
        Serial.println("y = ");
        Serial.print(y);
        Serial.println();
        z = 170;
        Serial.println("z =");
        Serial.print(z);
        Serial.println();
        I2Cslaveaddresstransmission();
      }
      while(mpu6050.getAngleZ()- initial<180);
        
    } 

    //when number stored in array is 0
    else if (command[j] == 0)
    {
      //anticlockwise 180
      Serial.println("stopped");
      lcd.clear();
      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      lcd.print("STOPPED...      ");
      //delay(500);
      x = 0;
      // Serial.println("x = ");
      // Serial.print(x);
      // Serial.println();
      y = 0;
      // Serial.println("y = ");
      // Serial.print(y);
      // Serial.println();
      z = 82;
      // Serial.println("z =");
      // Serial.print(z); 
      // Serial.println();
      I2Cslaveaddresstransmission();
      delay(5000);           
    }    
  }
  //Serial.println();
  while(1){
    
  }
  }
}



