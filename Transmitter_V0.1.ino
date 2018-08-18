
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

const int pinCE = 7; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 8; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 wirelessSPI(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection
const uint64_t pAddress = 0xB00B1E5000LL;   // Radio pipe addresses for the 2 nodes to communicate.

#include <Wire.h>

#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37
#define Register_OFSX 0x1E
#define Register_OFSY 0x1F
#define Register_OFSZ 0x20

int ADXAddress = 0xA7 >> 1;  // the default 7-bit slave address
int reading = 0;
int val=0;
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
double Xg,Yg,Zg;

void setup()  
{
  Serial.begin(57600);   //start serial to communicate process
  wirelessSPI.begin();            //Start the nRF24 module
  wirelessSPI.setAutoAck(1);                    // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  wirelessSPI.setRetries(5,15);                 // Sets up retries and timing for packets that were not ack'd, current settings: smallest time between retries, max no. of retries
  wirelessSPI.openWritingPipe(pAddress);        // pipe address that we will communicate over, must be the same for each nRF24 module
  wirelessSPI.stopListening();

  Wire.begin();   
  delay(100);
  // enable to measute g data
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);                //measuring enable
  Wire.endTransmission();     // stop transmitting
}

void loop()  
{

  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_X0);
  Wire.write(Register_X1);
//  Wire.write(Register_OFSX);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    X0 = Wire.read();
    X1 = Wire.read(); 
    X1=X1<<8;
    X_out=X0+X1;   
  }

  //------------------Y
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Y0);
  Wire.write(Register_Y1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Y0 = Wire.read();
    Y1 = Wire.read(); 
    Y1=Y1<<8;
    Y_out=Y0+Y1;
  }
  //------------------Z
  Wire.beginTransmission(ADXAddress); // transmit to device
  Wire.write(Register_Z0);
  Wire.write(Register_Z1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2); 
  if(Wire.available()<=2)   
  {
    Z0 = Wire.read();
    Z1 = Wire.read(); 
    Z1=Z1<<8;
    Z_out=Z0+Z1;
  }
  //
  Xg=X_out/5.0;
  Yg=Y_out/5.0;
  Zg=Z_out/5.0;
  int allVal = Xg+Yg+Zg;
   byte t = abs(allVal);//note that we can cast the ADC value to a byte because we know the temp sensor is not going to return a value higher than 255
   if (!wirelessSPI.write(&t, 1 )){  //if the send fails let the user know over serial monitor     
   }
   Serial.println(abs(allVal));
}
