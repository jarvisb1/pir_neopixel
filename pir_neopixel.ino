// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define NEOPIXEL_PIN    6
#define PIR_PIN         2

//These two values control the speed of the fading on and off. They are essentially two knobs that both control the fade speed. Lower numbers for either will make the fading go faster. Larger numbers will make the fading go slower.
#define NUM_FADE_STOPS         20
#define FADE_DELAY_PER_STOP_MS 100

// How many NeoPixels are attached to the Arduino?
#define NUM_PIXELS      16

enum state { S_LED_OFF, S_FADE_ON, S_LED_ON, S_FADE_OFF };
state curr_state = S_LED_OFF;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  pinMode(PIR_PIN, INPUT); // declare sensor as input

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
}

void set_leds(float brightness = 1.0)
{
  uint8_t val = (uint8_t)(255.0 * brightness);
  for(int i = 0; i < NUM_PIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(val, val, val));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }  
}

void loop()
{
  int pir_val;
  switch(curr_state)
  {
    case S_FADE_ON:
      //In "fade on" state, fade the lights on then move to the "led on" state
      for (int i = 0; i < NUM_FADE_STOPS; i++)
      {
        set_leds((float)i / NUM_FADE_STOPS);
        delay(FADE_DELAY_PER_STOP_MS);
      }
      curr_state = S_LED_ON;
      break;
      
    case S_LED_ON:
      //In "led on" state, look for the PIR to go low, then move to the "fade off" state.
      set_leds(1.0);
      pir_val = digitalRead(PIR_PIN);
      if (pir_val == LOW)
      {
        curr_state = S_FADE_OFF;
      }
      break;
      
    case S_FADE_OFF:
      //In "fade off" state, fade the lights off then move to the "led off" state
      for (int i = NUM_FADE_STOPS - 1; i >= 0; i--)
      {
        set_leds((float)i / NUM_FADE_STOPS);
        delay(FADE_DELAY_PER_STOP_MS);
      }
      curr_state = S_LED_OFF;
      break;
      
    case S_LED_OFF:
    default:
      //In "led off" state, look for the PIR to go high, then switch to "fade on" state
      pir_val = digitalRead(PIR_PIN);
      if (pir_val == HIGH)
      {
        curr_state = S_FADE_ON;
      }
      break;
  }
}
