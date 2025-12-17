// LED Animation Functions for multi-channel WS2812B driver
// These functions accept a channel parameter to specify which LED strip to control
// Animation functions are now non-blocking - call them repeatedly in your main loop

// Improved timing system for non-blocking animations
static uint32_t animation_tick_counter = 0;

// Timing function that provides more reasonable timing for animations
static uint32_t get_animation_ticks(void) {
    // Increment tick counter more frequently for smoother animations
    // This provides ~1ms resolution timing based on loop iterations
    animation_tick_counter++;
    return animation_tick_counter;
}

// Animation state variables (one per channel)
static uint8_t rainbow_cycle_pos[MAX_LED_CHANNELS] = {0};
static uint8_t theater_chase_pos[MAX_LED_CHANNELS] = {0};
static uint8_t color_wipe_pos[MAX_LED_CHANNELS] = {0};
static uint8_t pulse_brightness[MAX_LED_CHANNELS] = {0};
static uint8_t pulse_direction[MAX_LED_CHANNELS] = {0}; // 0 = up, 1 = down
static uint8_t rainbows_pos[MAX_LED_CHANNELS] = {0};

// Helper function to get a specific channel
static WS2812_Channel_t* LED_GetChannel(uint8_t channel_idx) {
    if (channel_idx < MAX_LED_CHANNELS && ws2812_channels[channel_idx].active) {
        return &ws2812_channels[channel_idx];
    }
    return NULL;
}

// Turn off all LEDs on the specified channel
void LED_OFF(uint8_t channel_idx) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;  // Red
        ch->led_buffer[i][1] = 0;  // Green
        ch->led_buffer[i][2] = 0;  // Blue
    }
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Fill with red color
void LED_RED(uint8_t channel_idx, uint8_t brightness) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = brightness;  // Red
        ch->led_buffer[i][1] = 0;           // Green
        ch->led_buffer[i][2] = 0;           // Blue
    }
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Fill with green color
void LED_GREEN(uint8_t channel_idx, uint8_t brightness) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;           // Red
        ch->led_buffer[i][1] = brightness;  // Green
        ch->led_buffer[i][2] = 0;           // Blue
    }
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Fill with blue color
void LED_BLUE(uint8_t channel_idx, uint8_t brightness) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;           // Red
        ch->led_buffer[i][1] = 0;           // Green
        ch->led_buffer[i][2] = brightness;  // Blue
    }
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Fill entire strip with a custom RGB color
void LED_FILL(uint8_t channel_idx, uint16_t red, uint16_t green, uint16_t blue) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = (uint8_t)(red > 255 ? 255 : red);
        ch->led_buffer[i][1] = (uint8_t)(green > 255 ? 255 : green);
        ch->led_buffer[i][2] = (uint8_t)(blue > 255 ? 255 : blue);
    }
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Wheel function for rainbow effects (returns pointer to 3-byte RGB array)
uint8_t * Wheel(uint8_t WheelPos) {
    static uint8_t c[3];
    
    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }
    return c;
}

// Rainbow cycle effect (non-blocking - call repeatedly)
void LED_RAINBOW_CYCLE(uint8_t channel_idx, uint16_t speed) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        uint8_t* color = Wheel((i + rainbow_cycle_pos[channel_idx]) & 255);
        ch->led_buffer[i][0] = color[0];
        ch->led_buffer[i][1] = color[1];
        ch->led_buffer[i][2] = color[2];
    }
    
    WS2812_SendAll();
    
    rainbow_cycle_pos[channel_idx] = (rainbow_cycle_pos[channel_idx] + 1) & 255;
}

// Theater chase effect (non-blocking - call repeatedly)
void LED_THEATER_CHASE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    // Clear all pixels
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;
        ch->led_buffer[i][1] = 0;
        ch->led_buffer[i][2] = 0;
    }
    
    // Turn every third pixel on based on current position
    for (uint16_t i = 0; i < ch->led_count; i++) {
        if ((i + theater_chase_pos[channel_idx]) % 3 == 0) {
            ch->led_buffer[i][0] = red;
            ch->led_buffer[i][1] = green;
            ch->led_buffer[i][2] = blue;
        }
    }
    
    WS2812_SendAll();
    
    theater_chase_pos[channel_idx] = (theater_chase_pos[channel_idx] + 1) % 3;
}

// Color wipe effect (non-blocking - call repeatedly)
void LED_COLOUR_WIPE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    if (color_wipe_pos[channel_idx] < ch->led_count) {
        ch->led_buffer[color_wipe_pos[channel_idx]][0] = red;
        ch->led_buffer[color_wipe_pos[channel_idx]][1] = green;
        ch->led_buffer[color_wipe_pos[channel_idx]][2] = blue;
        
        WS2812_SendAll();
        
        color_wipe_pos[channel_idx]++;
    } else {
        // Reset for next wipe
        color_wipe_pos[channel_idx] = 0;
    }
}

// Single pixel control
void LED_SINGLE_PIXEL(uint8_t channel_idx, uint16_t position, uint8_t red, uint8_t green, uint8_t blue) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch || position >= ch->led_count) return;
    
    // Clear all pixels
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;
        ch->led_buffer[i][1] = 0;
        ch->led_buffer[i][2] = 0;
    }
    
    // Set the specified pixel
    ch->led_buffer[position][0] = red;
    ch->led_buffer[position][1] = green;
    ch->led_buffer[position][2] = blue;
    
    //WS2812_SendChannel(ch);
    WS2812_SendAll();
}

// Pulse effect (non-blocking - call repeatedly)
void LED_PULSE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    uint8_t r = (uint16_t)red * pulse_brightness[channel_idx] / 255;
    uint8_t g = (uint16_t)green * pulse_brightness[channel_idx] / 255;
    uint8_t b = (uint16_t)blue * pulse_brightness[channel_idx] / 255;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = r;
        ch->led_buffer[i][1] = g;
        ch->led_buffer[i][2] = b;
    }
    
    WS2812_SendAll();
    
    if (pulse_direction[channel_idx] == 0) { // Pulsing up
        pulse_brightness[channel_idx]++;
        if (pulse_brightness[channel_idx] >= 255) {
            pulse_direction[channel_idx] = 1; // Start pulsing down
        }
    } else { // Pulsing down
        pulse_brightness[channel_idx]--;
        if (pulse_brightness[channel_idx] <= 0) {
            pulse_direction[channel_idx] = 0; // Start pulsing up again
        }
    }
}

// Rainbow effect with configurable width (non-blocking - call repeatedly)
void LED_RAINBOWS(uint8_t channel_idx, uint16_t speed, uint16_t width) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch || width == 0) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        uint8_t* color = Wheel((uint8_t)((i * 256 / width + rainbows_pos[channel_idx]) & 255));
        ch->led_buffer[i][0] = color[0];
        ch->led_buffer[i][1] = color[1];
        ch->led_buffer[i][2] = color[2];
    }
    
    WS2812_SendAll();
    
    rainbows_pos[channel_idx] = (rainbows_pos[channel_idx] + 1) & 255;
}

// Flash RGB colors (non-blocking - call repeatedly)
// Note: speed parameter is in animation ticks (loop iterations), not milliseconds
// Typical values: 10-50 for fast flashing, 100-500 for slower transitions
// THIS FUNCTION CURRENTLY CAUSES TIMING ISSUES - USE WITH CAUTION
void LED_RGB_FLASH(uint8_t channel_idx, uint16_t speed, uint8_t brightness) {
    static uint32_t last_update[MAX_LED_CHANNELS] = {0};
    static uint8_t flash_state[MAX_LED_CHANNELS] = {0};
    
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    uint32_t current_time = get_animation_ticks();
    if (current_time - last_update[channel_idx] < speed) return;
    
    last_update[channel_idx] = current_time;
    
    // Clear all pixels first
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;
        ch->led_buffer[i][1] = 0;
        ch->led_buffer[i][2] = 0;
    }
    
    // Set color based on state
    switch (flash_state[channel_idx]) {
        case 0: // Red
            for (uint16_t i = 0; i < ch->led_count; i++) {
                ch->led_buffer[i][0] = brightness;
            }
            break;
        case 1: // Green
            for (uint16_t i = 0; i < ch->led_count; i++) {
                ch->led_buffer[i][1] = brightness;
            }
            break;
        case 2: // Blue
            for (uint16_t i = 0; i < ch->led_count; i++) {
                ch->led_buffer[i][2] = brightness;
            }
            break;
    }
    
    WS2812_SendAll();
    
    // Cycle through colors: Red -> Green -> Blue -> Red...
    flash_state[channel_idx] = (flash_state[channel_idx] + 1) % 3;
}

// Multi-channel API helper functions

// Configure a new LED channel - directly use WS2812_ConfigureChannel from driver
// Example: WS2812_ConfigureChannel(0, PC4, 10, 255);

// Send data to a specific channel
void LED_SendChannelData(uint8_t channel_idx) {
    if (channel_idx < num_channels && ws2812_channels[channel_idx].active) {
        WS2812_SendChannel(&ws2812_channels[channel_idx]);
    }
}

// Send data to all channels
void LED_SendAllChannels(void) {
    WS2812_SendAll();
}

// Get buffer for a specific channel
uint8_t (*LED_GetChannelBuffer(uint8_t channel_idx))[3] {
    if (channel_idx < num_channels && ws2812_channels[channel_idx].active) {
        return ws2812_channels[channel_idx].led_buffer;
    }
    return NULL;
}

// Get LED count for a specific channel
uint16_t LED_GetChannelLedCount(uint8_t channel_idx) {
    if (channel_idx < num_channels && ws2812_channels[channel_idx].active) {
        return ws2812_channels[channel_idx].led_count;
    }
    return 0;
}