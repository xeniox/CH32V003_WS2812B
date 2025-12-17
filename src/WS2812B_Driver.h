// Include necessary libraries
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Define GPIO pin constants (port << 4 | pin_number)
#define PA1  0x01
#define PA2  0x02
#define PC1  0x11
#define PC2  0x12
#define PC4  0x14
#define PD4  0x34

// Maximum number of independent LED channels
#define MAX_LED_CHANNELS 4

// Structure to hold configuration and state for a single LED channel
typedef struct {
    uint8_t gpio_pin;              // GPIO pin identifier
    GPIO_TypeDef* port;            // Pointer to GPIO port (GPIOA, GPIOB, etc.)
    uint8_t pin_num;               // Pin number (0-15)
    uint32_t pin_mask;             // Bitmask for pin (1 << pin_num)
    uint16_t led_count;            // Number of LEDs on this channel
    uint8_t (*led_buffer)[3];      // Pointer to LED color buffer [led_count][3]
    uint8_t brightness;            // Brightness level (0-255)
    uint8_t active;                // 1 if configured, 0 if not
} WS2812_Channel_t;

// Array of LED channels
static WS2812_Channel_t ws2812_channels[MAX_LED_CHANNELS] = {0};
static uint8_t num_channels = 0;

// Forward declarations
static uint8_t WS2812_GetPortFromPin(uint8_t gpio_pin, GPIO_TypeDef** port, uint8_t* pin_num);
static void WS2812_SendBit(WS2812_Channel_t* channel, uint8_t bit);
static void WS2812_SendColor(WS2812_Channel_t* channel, uint8_t red, uint8_t green, uint8_t blue);
static void WS2812_SendChannel(WS2812_Channel_t* channel);
static void WS2812_SendAll(void);

// Map GPIO pin to port and pin number
static uint8_t WS2812_GetPortFromPin(uint8_t gpio_pin, GPIO_TypeDef** port, uint8_t* pin_num) {
    switch(gpio_pin) {
        case PA1:
            *port = GPIOA;
            *pin_num = 1;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
            break;
        case PA2:
            *port = GPIOA;
            *pin_num = 2;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
            break;
        case PC1:
            *port = GPIOC;
            *pin_num = 1;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            break;
        case PC2:
            *port = GPIOC;
            *pin_num = 2;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            break;
        case PC4:
            *port = GPIOC;
            *pin_num = 4;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            break;
        case PD4:
            *port = GPIOD;
            *pin_num = 4;
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
            break;
        default:
            return 0; // Invalid pin
    }
    return 1;
}

// Configure a new LED channel with independent buffer
static uint8_t WS2812_ConfigureChannel(uint8_t channel_idx, uint8_t gpio_pin, uint16_t led_count_param, uint8_t bright_level) {
    if (channel_idx >= MAX_LED_CHANNELS) return 0;
    if (led_count_param == 0) return 0;
    
    GPIO_TypeDef* port = NULL;
    uint8_t pin_num = 0;
    
    // Get port and pin from GPIO identifier
    if (!WS2812_GetPortFromPin(gpio_pin, &port, &pin_num)) {
        return 0;
    }
    
    // Allocate buffer for this channel
    uint8_t (*buffer)[3] = (uint8_t (*)[3])malloc(led_count_param * 3);
    if (!buffer) return 0;
    
    // Initialize buffer to off (all zeros)
    memset(buffer, 0, led_count_param * 3);
    
    // Store channel configuration
    ws2812_channels[channel_idx].gpio_pin = gpio_pin;
    ws2812_channels[channel_idx].port = port;
    ws2812_channels[channel_idx].pin_num = pin_num;
    ws2812_channels[channel_idx].pin_mask = 1 << pin_num;
    ws2812_channels[channel_idx].led_count = led_count_param;
    ws2812_channels[channel_idx].led_buffer = buffer;
    ws2812_channels[channel_idx].brightness = bright_level;
    ws2812_channels[channel_idx].active = 1;
    
    // Configure pin as output (50MHz, push-pull)
    uint32_t pin_config = (GPIO_Speed_50MHz | GPIO_Mode_Out_PP) << (pin_num * 4);
    uint32_t pin_mask_config = 0xF << (pin_num * 4);
    
    port->CFGLR = (port->CFGLR & ~pin_mask_config) | pin_config;
    
    // Initial state is low
    port->BCR = ws2812_channels[channel_idx].pin_mask;
    
    if (channel_idx >= num_channels) {
        num_channels = channel_idx + 1;
    }
    
    return 1;
}

// Utility
int Map_Range(int value, int inMin, int inMax, int outMin, int outMax) {
    // Make sure the input value is within the input range
    value = value < inMin ? inMin : value;
    value = value > inMax ? inMax : value;
    // Map the value from the input range to the output range
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

// Send a single bit on a specific channel
static void WS2812_SendBit(WS2812_Channel_t* channel, uint8_t bit) {
    if (bit) {
        // Send a 1 bit (high for ~800ns, low for ~400ns)
        channel->port->BSHR = channel->pin_mask;
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        
        channel->port->BCR = channel->pin_mask;
    } else {
        // Send a 0 bit (high for ~400ns, low for ~400ns)
        channel->port->BSHR = channel->pin_mask;
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");
        
        channel->port->BCR = channel->pin_mask;
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
        __asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
    }
}

// Send a single color for a single LED on a specific channel
static void WS2812_SendColor(WS2812_Channel_t* channel, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t g = Map_Range(green, 0, 255, 0, (channel->brightness - 100));
    uint8_t r = Map_Range(red, 0, 255, 0, channel->brightness);
    uint8_t b = Map_Range(blue, 0, 255, 0, (channel->brightness - 100));
    
    // Send green (MSB)
    for (int i = 7; i >= 0; i--) {
        WS2812_SendBit(channel, (g >> i) & 1);
    }
    // Send red
    for (int i = 7; i >= 0; i--) {
        WS2812_SendBit(channel, (r >> i) & 1);
    }
    // Send blue (LSB)
    for (int i = 7; i >= 0; i--) {
        WS2812_SendBit(channel, (b >> i) & 1);
    }
    
    Delay_Us(150);
}

// Send the entire buffer for a single channel
static void WS2812_SendChannel(WS2812_Channel_t* channel) {
    if (!channel->active || !channel->led_buffer) return;
    
    for (uint16_t i = 0; i < channel->led_count; i++) {
        WS2812_SendColor(channel, 
                        channel->led_buffer[i][0],  // Red
                        channel->led_buffer[i][1],  // Green
                        channel->led_buffer[i][2]); // Blue
    }
    
    Delay_Us(150); // Frame separator
}

// Send data to all configured channels
static void WS2812_SendAll(void) {
    for (uint8_t i = 0; i < num_channels; i++) {
        if (ws2812_channels[i].active) {
            WS2812_SendChannel(&ws2812_channels[i]);
        }
    }
}

// Legacy single-channel functions (for backward compatibility if needed)
// Send a single bit, Check mark/space ratio of the data on C4 with an oscilloscope
void LED_SendBit(uint8_t bit){
    // This function is deprecated - use WS2812_SendBit with channel instead
}

