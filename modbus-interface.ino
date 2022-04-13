/* 
   
   V1.0:  17/11/2021  - Data received on MODBUS interface & dashboard.
                      - Single phase TNB testing.
                      - Data: Voltage, Current, Power, Power Factor & Total kWh.

   V1.1:  12/1/2022   - Added one more data to fill the weird character displayed.
                      - Data added: Apparent power.

   V1.2:  17/1/2022   - Apparent power address removed from testing.

   V1.3:  7/4/2022    - New changed data: Voltage, Current, Power, Power Factor & Frequency.

   V1.4:  8/4/2022    - New changed data:  voltPhase1, voltAvg, currentPhase1, maxCurrentDmd, totalKwh
   
*/

/*
 
   
           Description                    Units             Address       Selected[X]
   =============================        =========         ==========     =============
   Phase 1 line to neutral volts            V               0x0000            [X]
   Phase 2 line to neutral volts            V               0x0002            [ ]
   Phase 3 line to neutral volts            V               0x0004            [ ]
   
   Phase 1 current                          A               0x0006            [X]
   Phase 2 current                          A               0x0008            [ ]
   Phase 3 current                          A               0x000A            [ ]
    
   Phase 1 power                            W               0x000C            [ ]
   Phase 2 power                            W               0x000E            [ ]
   Phase 3 power                            W               0x0010            [ ]

   Phase 1 apparent power                   VA              0x0012            [ ]
   Phase 2 apparent power                   VA              0x0014            [ ]
   Phase 3 apparent power                   VA              0x0016            [ ]

   Phase 1 reactive power                   VAr             0x0018            [ ]
   Phase 2 reactive power                   VAr             0x001A            [ ]
   Phase 3 reactive power                   VAr             0x001C            [ ]

   Phase 1 power factor                      -              0x001E            [ ]
   Phase 2 power factor                      -              0x0020            [ ]
   Phase 3 power factor                      -              0x0022            [ ]

   Phase 1 phase angle                      Degrees         0x0024            [ ]
   Phase 2 phase angle                      Degrees         0x0026            [ ]
   Phase 3 phase angle                      Degrees         0x0028            [ ]

   Average line to neutral volts            V               0x002A            [ ]
   Average line current                     A               0x002E            [ ]
   Sum of line currents                     A               0x0030            [ ]
   Total system power                       W               0x0034            [ ]
   Total system apparent power              VA              0x0038            [ ]
   Total system reactive power              VAr             0x003C            [ ]
   Total system power factor (1)              -             0x003E            [ ]
   Total system phase angle                 Degrees         0x0042            [ ]
   Frequency of supply voltages             Hz              0x0046            [ ]
   Import Wh since last reset (2)           kWh/MWh         0x0048            [ ]
   Export Wh since last reset (2)           kWh/MWh         0x004A            [ ]
   Import VArh since last reset (2)         kVArh/MVArh     0x004C            [ ]
   Export VArh since last reset (2)         kVArh/MVArh     0x004E            [ ]
   VAh since last reset (2)                 kVAh/MVAh       0x0050            [ ]
   Ah since last reset(3)                   Ah/kAh          0x0052            [ ]
   Total system power demand (4)            W               0x0054            [ ]
   Maximum total system power demand(4)     VA              0x0056            [ ]
   Total system VA demand                   VA              0x0064            [ ]
   Maximum total system VA demand           VA              0x0066            [ ]
   Neutral current demand                   A               0x0068            [ ]
   Maximum neutral current demand           A               0x006A            [ ]

   Line 1 to Line 2 volts                   V               0x00C8            [ ]
   Line 2 to Line 3 volts                   V               0x00CA            [ ]
   Line 3 to Line 1 volts                   V               0x00CC            [ ]

   Average line to line volts               V               0x00CE            [X]
   Neutral current                          A               0x00E0            [ ]
   Total system power factor (5)            Degree          0x00FE            [ ]

   Phase 1 current demand                   A               0x0102            [ ]
   Phase 2 current demand                   A               0x0104            [ ]
   Phase 3 current demand                   A               0x0106            [ ]

   Maximum phase 1 current demand           A               0x0108            [X]
   Maximum phase 2 current demand           A               0x010A            [ ]
   Maximum phase 3 current demand           A               0x010C            [ ]

   Total kwh                                kWh             0x0156            [X]
   Total kvarh                              kVArh           0x0158            [ ]
*/


#include<ModbusMaster.h>
#include<SoftwareSerial.h>
#include <Wire.h>

#define SLAVE_ADDRESS 91

SoftwareSerial mySerial(3,4); //Rx: Pin 3, Tx: Pin 4

uint16_t m_startAddress[5] = {0x0000, 0x00CE, 0x0006, 0x0108, 0x0156}; 
//                             voltPhase1    voltAvg   currentPhase1   maxCurrentDmd    kwh   
uint16_t m_length = 0x00001;
uint8_t result;
int inState = 0;
int DE = 8;
union
{
  uint16_t u[2];
  float f;
}
meterdata;

//0x0012, 0x0018, 0x0046, 0x0006 0x000C
//APower   RPower  freq   current  power

////////////////////////////////////// Data Send to Cognisense //////////////////////////////////

char outBuffer[30];

String voltPhase1 = String(meterdata.f);
String voltAvg = String(meterdata.f);
String currentPhase1 = String(meterdata.f);
String maxCurrentDmd = String(meterdata.f);
String totalKwh = String(meterdata.f);




//String reactivePower = String(meterdata.f);
//String frequency = String(meterdata.f);

String output = voltPhase1 + "," + voltAvg + "," + currentPhase1 + "," + maxCurrentDmd + "," + totalKwh;



ModbusMaster node;

void enableTx()  //TX
{
  delay(2);   
  digitalWrite(DE, HIGH);
}

void disableTx()  //RX
{
  delay(2);
  digitalWrite(DE, LOW);
}

void requestEvent() {

// Put the data inside Wire.write()
// Wire.write(String(meterdata.f).c_str())

Wire.write(String(output).c_str());
    
}

void setup()
{
  pinMode(DE, OUTPUT);
  digitalWrite(DE, HIGH);
  node.preTransmission(enableTx);
  node.postTransmission(disableTx);
  
  Serial.begin(57600);
  mySerial.begin(9600);
  node.begin(111, mySerial);
  Wire.begin(SLAVE_ADDRESS);
  
//  Serial.println("CS I2C slaves Testing...");
  delay(1000);
  Wire.onRequest(requestEvent);
  // I2C Slave address
  
  //Serial.println("Sending to Cognisense...");
  

}

void loop()
{
  
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.endTransmission();
  
  result = node.readInputRegisters(m_startAddress[inState], m_length);
  if (result == node.ku8MBSuccess)
  {
      if (inState == 0) {
        meterdata.u[1] = node.getResponseBuffer(0);
        meterdata.u[0] = node.getResponseBuffer(1);
        voltPhase1 = String(meterdata.f);
        Serial.print(voltPhase1 + ",");
        
        }
      else if (inState == 1){
        meterdata.u[1] = node.getResponseBuffer(0);
        meterdata.u[0] = node.getResponseBuffer(1);
        voltAvg = String(meterdata.f);
        Serial.print(voltAvg + ",");
        }
      else if (inState == 2) {
        meterdata.u[1] = node.getResponseBuffer(0);
        meterdata.u[0] = node.getResponseBuffer(1);
       currentPhase1 = String(meterdata.f);
       Serial.print(currentPhase1 + ",");
       }
       else if (inState == 3) {
        meterdata.u[1] = node.getResponseBuffer(0);
        meterdata.u[0] = node.getResponseBuffer(1);
        maxCurrentDmd = String(meterdata.f);
        Serial.print(maxCurrentDmd + ",");
       }
        
       else if (inState == 4) {
        meterdata.u[1] = node.getResponseBuffer(0);
        meterdata.u[0] = node.getResponseBuffer(1);
        totalKwh = String(meterdata.f);
        Serial.print(totalKwh);
        Serial.println("");
       }

//       else if (inState == 5) {
//        meterdata.u[1] = node.getResponseBuffer(0);
//        meterdata.u[0] = node.getResponseBuffer(1);
//        freq = String(meterdata.f);
//        Serial.print(freq);
//        Serial.println("");
//       }

//       else if (inState == 4) {
//        meterdata.u[1] = node.getResponseBuffer(0);
//        meterdata.u[0] = node.getResponseBuffer(1);
//        freq = String(meterdata.f);
//        Serial.print(freq);
//        Serial.println("");
//       }
  }
       else {
      Serial.println("ERROR, cannot read output.");
     
      }
  

  inState++;
  if (inState == 5) inState = 0;
  delay(1000);

  }