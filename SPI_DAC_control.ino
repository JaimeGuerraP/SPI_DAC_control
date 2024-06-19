#include <SPI.h>

#define V_REF 2.5
#define DAC_RESOLUTION 12
#define FULL_SCALE_VALUE 4095
#define SERIAL_SPEED 9600
#define DAC0 42
#define DAC1 43


int incomingByte = 0; // for incoming serial data
int sync_ = DAC0;
const int spi_ss = 48; //DAC714P A0 shift regsiter
const int dac_lch = 46; // DAC714 A1 DAC Latch
uint16_t input_0; // 16 bit input values
uint8_t byte_0, byte_1; // bytes for SPI transfer
int channel;
String stringMonitorData;
char hexCar[4];
int flag=0;
int test = 44;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_SPEED);  
  pinMode(test, OUTPUT);
  pinMode(DAC0, OUTPUT);
  digitalWrite(DAC0, HIGH);
  pinMode(DAC1, OUTPUT);
  digitalWrite(DAC1, HIGH);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.begin();
  digitalWrite(DAC0, LOW); 
  SPI.transfer16(0x0000);
  digitalWrite(DAC0, HIGH);
  delay(20);
  digitalWrite(DAC1, LOW); 
  SPI.transfer16(0x0000);
  digitalWrite(DAC1, HIGH);
  
}

void loop() {
  
    if (Serial.available() > 0) {

        stringMonitorData = Serial.readString();

        flag = decode_string(stringMonitorData, &channel, &input_0);

       
        if (flag == 1){
            input_0 = input_0 << 2;

            //if (sync_ == DAC1){
            //  input_0 = input_0 + 32768;
              
            //}
            
            Serial.print("Word sent to DAC: ");
            sprintf(hexCar, "%04X", input_0);
            Serial.println(hexCar);
        //   byte_1 = (uint8_t)(input_0 & 0x00FF);
        //   byte_0 = (uint8_t)(input_0 >> 8);
        //   Serial.print("byte_0: ");
        //   sprintf(hexCar, "%02X", byte_0);
        //   Serial.println(hexCar);
        //   Serial.print("byte_1: ");
        //   sprintf(hexCar, "%02X", byte_1);
        //   Serial.println(hexCar);
        //   SPI.transfer(byte_0);
            digitalWrite(sync_, LOW); 
            delay(5);
            SPI.transfer16(input_0);
            delay(5);
            digitalWrite(sync_, HIGH);

        }
        else if (flag == 2)
        {
            Serial.print("Canal: ");
            Serial.println(channel);
            if (channel == 0){
                sync_ = DAC0;
            }
            else if (channel == 1)
            {
                sync_ = DAC1;
                
            }
        } 
        
        stringMonitorData = "";

  }
  digitalWrite(test,HIGH);
  delay(3);
  digitalWrite(test,LOW);
  delay(3);
}


int decode_string(String myString,int *channel, uint16_t *value){

    if (myString.startsWith("DAC"))
    {
        String auxString = myString.substring(3,4);
        *channel = auxString.toInt();
        return 2;
    }
    else if (myString.startsWith("V"))
    {
        myString.remove(0,1);
        double number = myString.toDouble();
        *value = calculate_value(number);
        return 1;
    }
    else
    {
        Serial.println("WARNING: Incorrect command.");
        return 0;

    }

    
}

uint16_t calculate_value (double number)
{
    uint16_t result = uint16_t(round(number*1638.4));
    return result;
    
}
