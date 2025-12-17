#include <debug.h>

#include <WS2812B_Driver.h>

// GPIO pin options: PA1, PA2, PC1, PC2, PC4, PD4

// Include the colour and animation functions
#include <LED_Functions.h>

int main(void) {

  Delay_Init();

  // Configure WS2812B channel(s)
  // GPIO pin options: PA1, PA2, PC1, PC2, PC4, PD4
  WS2812_ConfigureChannel(0, PC4, 10, 255);   // Initialize LED channel 0 with 10 LEDs on PC4 pin with brightness 255
  WS2812_ConfigureChannel(1, PC2, 10, 255);  // Initialize LED channel 1 with 10 LEDs on PC2 pin with brightness 255

  Delay_Ms(10);

  LED_OFF(0); // Turn off all LEDs on channel 0

  Delay_Ms(250);

  while(1){
    // Non-blocking animations - call repeatedly for continuous animation
    // Each function advances one frame when enough time has passed
    
    // Channel 0: Rainbow effect with configurable width
    LED_RAINBOWS(0, 10, 10); // channel, speed (ticks), width (LEDs per cycle)
    
    // Channel 1: Rainbow cycle effect  
    LED_RAINBOW_CYCLE(1, 100); // channel, speed (ticks)
    
    // Uncomment other animations to run simultaneously on different channels:
    //LED_OFF(0); // Turn off all LEDs on channel 0
    //LED_RED(1, 255); // fill with red colour at specified brightness 0-255 on channel 0
    //LED_BLUE(0, 255); // fill with blue colour at specified brightness 0-255 on channel 0
    //LED_GREEN(0, 255); // fill with green colour at specified brightness 0-255 on channel 0
    //LED_FILL(0, 255, 127, 0); // fill with a single colour (red, green, blue) 0-255 RGB on channel 0
    //LED_RAINBOWS(1, 10, 10); // variables are channel, delay speed in ticks and width (number of leds that cover one cycle)
    //LED_RAINBOW_CYCLE(1, 100); // rainbow cycle with channel and delay in ticks
    //LED_THEATER_CHASE(1, 0, 255, 0, 100); // theater chase with channel, RGB and delay in ticks
    //LED_COLOUR_WIPE(0, 0, 0, 255, 250); // colour wipe with channel, RGB and delay in ticks
    //LED_SINGLE_PIXEL(0, 1, 255, 0, 0); // single pixel with channel, position and RGB
    //LED_SINGLE_PIXEL(0, 2, 0, 255, 0); // single pixel with channel, position and RGB
    //LED_SINGLE_PIXEL(0, 3, 0, 0, 255); // single pixel with channel, position and RGB
    //LED_PULSE(0, 255, 0, 255, 25); // pulse with channel, RGB and delay in ticks

    //RGB FLASH - THIS FUNCTION MAY CAUSE TIMING ISSUES WITH OTHER ANIMATIONS
    //LED_RGB_FLASH(1, 500, 255); // RGB flash with channel, speed (ticks) and brightness 0-255
    
    // Small delay to prevent overwhelming the processor
    Delay_Ms(1);
  }
  return 0;
}
