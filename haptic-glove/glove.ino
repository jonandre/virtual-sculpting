#include <SoftwareSerial.h>

/* Comment this out or compile with -DUSE_USB to use the serial terminal */
#ifndef USE_USB
  #define USE_WIFI
#endif

/* Write to the serial terminal what the WiFi receives */
#ifndef WITHOUT_ECHO
  #define WITH_ECHO
#endif


/**
 * Send 16 8:s twice for on, and 16 7:s twice for off, over UDP port 30000.
 * 
 * The program will react when it have 8 consecutive 8:s or 7:s, sending 16
 * symbols twice is just redundancy for package corruption and package lost
 * that can sometimes happen on UDP.
 * 
 * The 8:s and 7:s are bytes (0x08 and 0x07), but if you use the serial
 * terminal over USB, they are digit characters ('8' and '7').
 * 
 * The Arduino acts as an access point for its private network, its IP
 * address will be 1.2.3.4. The first one to connect to it will get the
 * IP address 1.2.3.14. Bind your UDP socket to 1.2.3.14 to ensure that
 * you can send successfully send to 1.2.3.4.
 * 
 * ------------------------------------------------------------------------
 * 
 * Wiring table:
 * 
 * Arduino pin 8 → RN-VX pin 2
 * Arduino pin 9 → RN-VX pin 3 (Adjust to 3.3 V if not using a 3.3 V Arduino)
 * Arduino pin 3 → Vibrator 1 arbitrary cable (The vibrators are actually unordered)
 * Arduino pin 4 → Vibrator 2 arbitrary cable
 * Arduino pin 6 → Vibrator 3 arbitrary cable
 * Arduino pin 7 → Vibrator 4 arbitrary cable
 * Remaining vibrator cables → Ground
 * Arduino pin 13 → Transistor base (can be exchanged with pin connected from the Arduino to a vibrator)
 * Transistor emitter → Loudspeak arbitrary cable
 * Remaining loadspeak cable → Ground
 * Arduino pin 5 → Transistor collector
 * RN-VX pin 1 → Arduino pin VCC (3.3 V)
 * RN-VX pin 10 → Ground
 */


#define WIFLY_TO_ARDUINO_PIN  8      /* Pin connected to pin 2 on the WiFly */
#define ARDUINO_TO_WIFLY_PIN  9      /* Pin connected to pin 3 on the WiFly */

#define AUDIO_FEEDBACK_TONE_PIN  5   /* Pin connected to the transistor's base (must support pulse-width modulation) */
#define AUDIO_FEEDBACK_TONE_HZ   58  /* Frequency (in Hz) for audio feedback (58,271 Hz ≈ B₂-flat) */

#define PORT  "30000"                /* The UDP port the WiFly listens on */


/* Macros for switching between WiFi and USB */
#ifdef USE_WIFI
  #define ON_SYMBOL   8
  #define OFF_SYMBOL  7
  #define SOURCE      wifi
#else
  #define ON_SYMBOL   '8'
  #define OFF_SYMBOL  '7'
  #define SOURCE      Serial
#endif


/**
 * Invoke METHOD(i, VALUE) for all i
 * that as a pin connected to a vibrartor
 * or the transistors base.
 * 
 * @param  METHOD:void (*)(int i, auto VALUE)  Method to run for all feedback pins
 * @param  VALUE                               Value to send
 */
#define feedback(METHOD, VALUE)  \
  ({                             \
    METHOD(3, VALUE);            \
    METHOD(4, VALUE);            \
    METHOD(6, VALUE);            \
    METHOD(7, VALUE);            \
    METHOD(13, VALUE);           \
  })


/**
 * Serial object between the Arduino and the WiFly
 */
SoftwareSerial wifi(WIFLY_TO_ARDUINO_PIN, ARDUINO_TO_WIFLY_PIN);


/**
 * Cyclic buffer of the last 8 read bytes from the WiFly,
 * this includes its feedback as well as want is sent to it.
 */
char buf[8];

/**
 * The index for the next byte in `buf`
 */
int ptr = 0;


/**
 * Arduino startup method
 */
void setup()
{
  int i;
  feedback(pinMode, OUTPUT);
  
  feedback(digitalWrite, LOW); /* Explicitly turn off all feedback (not really needed) */
  tone(AUDIO_FEEDBACK_TONE_PIN, AUDIO_FEEDBACK_TONE_HZ); /* Send audio frequency */
  
  /* Initialise communication with the WiFly (or USB) */
  SOURCE.begin(9600);
  #ifdef USE_WIFI
    #ifdef WITH_ECHO
      Serial.begin(9600);
    #endif
    
    /* Enter configuration mode */
    wifi.print("$$$");
    delay(300); /* Need to sleep for 250 milliseconds */
    
    ///* Set access point SSID */
    //wifi.print("set wlan ssid haptic-glove\r");
    
    /* Configure UDP listening */
    wifi.print("set ip proto 1\r"); /* UDP */
    wifi.print("set ip local " PORT "\r");
    
    /* Apply configurations by saving and rebooting */
    wifi.print("save\r");
    wifi.print("reboot\r");
  #endif
  
  /* Zero initialise `buf` to be on the safe side (not really needed) */
  for (i = 0; i < 8; i++)
    *(buf + i) = 0;
}


/**
 * Arduino main loop
 */
void loop()
{
  int count_8 = 0;
  int count_7 = 0;
  int i;
  
  if (SOURCE.available())
   {
     *(buf + ptr++) = SOURCE.read();
     #ifdef WITH_ECHO
       Serial.write(*(buf + ptr - 1));
     #endif
     ptr &= 7;
     
     for (i = 0; i < 8; i++)
       if      (*(buf + i) == ON_SYMBOL)   count_8++;
       else if (*(buf + i) == OFF_SYMBOL)  count_7++;
     
     if      (count_8 == 8)  feedback(digitalWrite, HIGH);
     else if (count_7 == 8)  feedback(digitalWrite, LOW);
   }
}


