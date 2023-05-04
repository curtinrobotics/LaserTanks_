void ring_init(void);
void collect(int, bool);
void colorWipe(int type, uint16_t wait);
void setRing(int type);
void dualTone(int type);
void ringFlash(int type, uint16_t rate, int times);
void pulseFlash(int type);
void ringBlink(int type, uint16_t rate, int times);
void theaterChase(int type, uint8_t wait);
void rainbowWipe(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

#define COOLDOWN_INTERVAL 10

#define PIN 6 /*the light ring signal cable is at pin 6*/
enum COLOURS {
  COLOUR_OFF,
  COLOUR_RED,
  COLOUR_ORANGE,
  COLOUR_YELLOW,
  COLOUR_GREEN,
  COLOUR_CYAN,
  COLOUR_BLUE,
  COLOUR_MAGENTA,
  COLOUR_WHITE,
  
  COLOUR_NUM,
};

enum POWERUPS {
  POWERUP_NONE,
  POWERUP_FIRE_RATE,
  POWERUP_DAMAGE_INCREASE,
  POWERUP_DAMAGE_RESISTANCE,
  POWERUP_REPAIR,
  POWERUP_SHIELD,
  POWERUP_MOVEMENT_SPEED,
  POWERUP_TURRET_ROTATION,
  POWERUP_CLEANSE,
  POWERUP_RANDOM,
  SPAWN_WARM,
  SPAWN_COOL,

  POWERUP_NUM,
};

const String type_lookup[12] = {"None","Fire rate","Damage increase","Damage resistance","Repair","Shield","Movement speed","Turret rotation","Cleanse","Random","Red spawn","Blue spawn"};
#define SOLID_COLOURS (COLOUR_NUM)
//int Powerup_type = POWERUP_CLEANSE;
//int Cooldown = 200;
//bool Hidden = false;
