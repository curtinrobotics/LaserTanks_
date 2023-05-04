#include <Wire.h>
//#include <Adafruit_NeoPixel.h>
#include "Animations.h"
#include "Powerups.h"

#define RING_ID 0x01

#if 1
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include "PN532.h"
  
  PN532_SPI pn532spi(SPI, 10);
  PN532 nfc(pn532spi);

/* When the number after #elif set as 1, it will be switch to HSU Mode*/
#elif 0
  #include <PN532_HSU.h>
  #include <PN532.h>
    
  PN532_HSU pn532hsu(Serial1);
  PN532 nfc(pn532hsu);

/* When the number after #if & #elif set as 0, it will be switch to I2C Mode*/
#else 
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>
  #include <NfcAdapter.h>
  
  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);
#endif

/*The following code is for the light ring*/
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define UID_RED 0x2198E6B3 //Tom White
#define UID_YELLOW 0xA944F83 //Samsung
#define UID_GREEN 0x3C473AEA //Smartrider
#define UID_BLUE 0x268FCE59 //DFES

enum tank {
  TANK_NONE,
  TANK_RED,
  TANK_YELLOW,
  TANK_GREEN,
  TANK_BLUE
};

uint8_t tank_type;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID of the card
int toggle = 5; //The toggeling pin from the master to indicate when card is found*****************************
byte x = 0;
boolean success = false;
boolean set = false;

int Powerup_type = POWERUP_NONE;
int Cooldown = 0;
bool Hidden = false;

//const String type_lookup[10] = {"None","Fire rate","Damage increase","Damage resistance","Repair","Movement speed","Shield","Turret rotation","Cleanse","Random"};

void setup() {
  pinMode(toggle, OUTPUT);//setting the bit for toggleing as output
  Serial.begin(115200);//sets the baud rate
  Wire.begin(RING_ID);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.println("Light Ring Start...");
  nfc.begin(); 
  //Wire.onRequest(requestEvent);//If the go ahead is given from the master, the slave will start sending the data
  
  uint32_t versiondata;// = nfc.getFirmwareVersion();//storing board details
  int spi_retries;
  while (!versiondata) //checking if a board is found
  {
    versiondata = nfc.getFirmwareVersion();
    //if(versiondata) break;
    Serial.println("Didn't find PN53x board");
    Serial.print("retry: ");
    spi_retries++;
    Serial.println(spi_retries);

    if(spi_retries == 4) {
      ringBlink(COLOUR_RED,500,10);
      break;
    }
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  nfc.setPassiveActivationRetries(0x01);\
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Ready!");
  //The 2 following codes are for the intializing of the light strip 
  ring_init();
  pinMode(2,INPUT_PULLUP);
}

void loop() {
  if(Powerup_type == POWERUP_RANDOM || Hidden) rainbowCycle(1);
  //else setRing(Powerup_type); // send it to the function setring
  
  if(Powerup_type != POWERUP_NONE) {
    uint8_t uidLength,i;// Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    if(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength)) {
  
      set = true;
      success = false;
      //Serial.println("in");
      //The card details now get printed to serial including its length and values in uid
      //Serial.println("Found a card!");
      for (uint8_t i=0; i < uidLength; i++) 
      {
        Serial.print(" 0x");Serial.print(uid[i], HEX);
      }
      Serial.println();
      uint32_t id_4;
      memcpy(&id_4,uid,4);
      Serial.println(id_4,HEX);
      tank_type = 0;
      
      if(id_4 == UID_RED) {
        Serial.println("Red tank detected");
        tank_type = TANK_RED;
      }
      else if(id_4 == UID_YELLOW) {
        Serial.println("Yellow tank detected!");
        tank_type = TANK_YELLOW;
      }
      else if(id_4 == UID_GREEN) {
        Serial.println("Green tank detected");
        tank_type = TANK_GREEN;
      }
      else if(id_4 == UID_BLUE) {
        Serial.println("Blue tank detected!");
        tank_type = TANK_BLUE;
      }
      if(tank_type != TANK_NONE) {
        Wire.beginTransmission(0);
        Wire.write((RING_ID << 4) + tank_type);
        Wire.endTransmission();
      }
       else {
        Serial.println("Unknown card");  
        tank_type = TANK_NONE;
        delay(500);
      }
      //Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      //Serial.print("UID Value: ");
      //calculating the UID
      Serial.println();
      
      if(tank_type != TANK_NONE) {
        collect(Powerup_type, Hidden);
        Powerup_type = POWERUP_NONE;
        Hidden = false;
        Cooldown = 0;
      } 
    }
  }
}

void powerup_set(int type, int cooldown, bool hidden) {
  //todo create queue to run from loop rather than interrupt (after collect animation)
    if(type != Powerup_type && !hidden) setRing(POWERUP_NONE);
    Powerup_type = type;
    Hidden = hidden;
  //Serial.print("New powerup: ");
  //Serial.println(Powerup_type);
  if(Powerup_type == POWERUP_NUM) { //restart cycle
    //rainbowCycle(10);
    setRing(0);
    Powerup_type = 1;
  }
  //Cooldown animations (solid colours)
  if(hidden) {
    if(cooldown > 0) rainbowWipe(cooldown);
  }
  else if(Powerup_type < COLOUR_NUM) {
    if(cooldown == 0) setRing(Powerup_type);
    else {
      colorWipe(Powerup_type, cooldown); // this sets the reset time
      ringBlink(0, 60, 2); // slow flashes for ready
    }
    setRing(Powerup_type);
  } //todo flag colours
  else if(Powerup_type == POWERUP_RANDOM) {
    if(cooldown > 0) rainbowWipe(cooldown);
  }
  else {
    dualTone(Powerup_type);
  }
  //Wire.onRequest(waiting);
  Serial.print(type_lookup[Powerup_type]);
  Serial.println(" ready!");
  Serial.println();
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  int type = (x & 0xF0) >> 4;
  int cooldown = ((x & 0x0E) >> 1)*COOLDOWN_INTERVAL;
  bool hidden = x & 0x01;

  if(type >= POWERUP_NUM) {
    Serial.println("Invalid command");
    Serial.println();
    return;
  }
  if(cooldown == 70) cooldown = 120; 
  //success = true;

  //Print the values (for debug)
  //Serial.println(x,HEX);
  Serial.print("Type: ");
  //Serial.println(Powerup_type);
  Serial.println(type_lookup[type]);
  Serial.print("Cooldown: ");
  if(cooldown == 0) Serial.println("Instant");
  else {
    Serial.print(cooldown);
    Serial.println("s");
  }
  Serial.print("Hidden: ");
  if(hidden) Serial.println("True");
  else Serial.println("False");

//  Wire.beginTransmission(0);
//  Wire.write((RING_ID << 4) + 0xF);
//  Wire.endTransmission();

  powerup_set(type, cooldown, hidden);
}

//void requestEvent(int howMany) {
//  Wire.beginTransmission(0);
//  Wire.write((RING_ID << 4) + 0xF);
//  Wire.endTransmission();
//}
