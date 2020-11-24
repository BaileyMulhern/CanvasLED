//Include needed Libraries at beginning
#include <Arduino.h>
#include <FastLED.h> // FastLED library for WS2812 RGB Stick http://fastled.io/
#include "src/LedUtils/ScriptManager.h"


// Interrupts on 2, 3
// PWM on 3, 5, 6, 9, 10, 11
#define BUTTON_PIN      2   
#define RED_PIN         5   
#define GREEN_PIN       6
#define BLUE_PIN        3
#define DUMMY_PIN       7

#define BRIGHTNESS      255
#define NUM_LEDS        1
#define BUTTON_SHORT_PRESS	50
#define BUTTON_LONG_PRESS	2000

volatile bool effect_switch = false;
volatile bool button_pressed = false;
volatile uint64_t button_press_time = 0;
uint8_t effect_index = 0;
CRGB leds[NUM_LEDS];

ScriptManager::ScriptElement script[] = {
    {EffectManager::EFFECT_OFF, 0, 0, 0, true},

    {EffectManager::EFFECT_SOLID_RED, 0, 0, 0, true},
    {EffectManager::EFFECT_SOLID_GREEN, 0, 0 , 0, true},
    {EffectManager::EFFECT_SOLID_BLUE, 0, 0, 0, true},
    {EffectManager::EFFECT_SOLID_CYAN, 0, 0, 0, true},
    {EffectManager::EFFECT_SOLID_MAGENTA, 0, 0, 0, true},
    {EffectManager::EFFECT_SOLID_YELLOW, 0, 0, 0, true},

    {EffectManager::EFFECT_RAINBOW_FILL_FAST, 0, 0, 0, true},
};

const uint8_t SCRIPT_LENGTH = sizeof(script) / sizeof(ScriptManager::ScriptElement);

ScriptManager scriptManager(leds, NUM_LEDS, true);

void buttonISR(void);


void setup(void)
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);
    
    FastLED.addLeds<NEOPIXEL, DUMMY_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    scriptManager.loadScript(script, SCRIPT_LENGTH);

    scriptManager.loadEffectEeprom();
    
    Serial.begin(115200);
}

void loop(void)
{
    if(effect_switch)
    {
        effect_switch = false;

        scriptManager.incrementScript();

        analogWrite(RED_PIN, 0);
        analogWrite(GREEN_PIN, 0);
        analogWrite(BLUE_PIN, 0);
        
        delay(250);
    }

    scriptManager.runScript();

    analogWrite(RED_PIN, leds[0].r);
    analogWrite(GREEN_PIN, leds[0].g);
    analogWrite(BLUE_PIN, leds[0].b);
}

void buttonISR(void)
{
	// If the button has been pressed, record the time
	if(digitalRead(BUTTON_PIN) == LOW)
	{
		button_pressed = true;
		button_press_time = millis();
	}
	// If the button was released
	else if(button_pressed && digitalRead(BUTTON_PIN) == HIGH)
	{
		button_pressed = false;

		// Increment effect with short button press
		if(millis() - button_press_time >= BUTTON_SHORT_PRESS)
		{
			effect_switch = true;
		}
	}
}
