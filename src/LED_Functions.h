// LED Animation Functions for multi-channel WS2812B driver
// These functions accept a channel parameter to specify which LED strip to control

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
    WS2812_SendChannel(ch);
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
    WS2812_SendChannel(ch);
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
    WS2812_SendChannel(ch);
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
    WS2812_SendChannel(ch);
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
    WS2812_SendChannel(ch);
}

// Flash RGB colors
void LED_RGB_FLASH(uint8_t channel_idx, uint16_t speed, uint8_t brightness) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    // Red
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = brightness;
        ch->led_buffer[i][1] = 0;
        ch->led_buffer[i][2] = 0;
    }
    WS2812_SendChannel(ch);
    Delay_Ms(speed);
    
    // Green
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;
        ch->led_buffer[i][1] = brightness;
        ch->led_buffer[i][2] = 0;
    }
    WS2812_SendChannel(ch);
    Delay_Ms(speed);
    
    // Blue
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = 0;
        ch->led_buffer[i][1] = 0;
        ch->led_buffer[i][2] = brightness;
    }
    WS2812_SendChannel(ch);
    Delay_Ms(speed);
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

// Rainbow cycle effect
void LED_RAINBOW_CYCLE(uint8_t channel_idx, uint16_t speed) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint8_t j = 0; j < 256; j++) {
        for (uint16_t i = 0; i < ch->led_count; i++) {
            uint8_t* color = Wheel((i + j) & 255);
            ch->led_buffer[i][0] = color[0];
            ch->led_buffer[i][1] = color[1];
            ch->led_buffer[i][2] = color[2];
        }
        WS2812_SendChannel(ch);
        Delay_Ms(speed);
    }
}

// Theater chase effect
void LED_THEATER_CHASE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint8_t q = 0; q < 3; q++) {
        for (uint16_t i = 0; i < ch->led_count; i = i + 3) {
            // Turn every third pixel on
            for (uint16_t j = 0; j < ch->led_count; j++) {
                if ((i + j) % 3 == q) {
                    ch->led_buffer[i + j][0] = red;
                    ch->led_buffer[i + j][1] = green;
                    ch->led_buffer[i + j][2] = blue;
                } else {
                    ch->led_buffer[i + j][0] = 0;
                    ch->led_buffer[i + j][1] = 0;
                    ch->led_buffer[i + j][2] = 0;
                }
            }
            WS2812_SendChannel(ch);
            Delay_Ms(speed);
        }
    }
}

// Color wipe effect
void LED_COLOUR_WIPE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    for (uint16_t i = 0; i < ch->led_count; i++) {
        ch->led_buffer[i][0] = red;
        ch->led_buffer[i][1] = green;
        ch->led_buffer[i][2] = blue;
        WS2812_SendChannel(ch);
        Delay_Ms(speed);
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
    
    WS2812_SendChannel(ch);
}

// Pulse effect
void LED_PULSE(uint8_t channel_idx, uint8_t red, uint8_t green, uint8_t blue, uint16_t speed) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch) return;
    
    // Pulse up
    for (uint8_t brightness = 0; brightness < 255; brightness++) {
        uint8_t r = (uint16_t)red * brightness / 255;
        uint8_t g = (uint16_t)green * brightness / 255;
        uint8_t b = (uint16_t)blue * brightness / 255;
        
        for (uint16_t i = 0; i < ch->led_count; i++) {
            ch->led_buffer[i][0] = r;
            ch->led_buffer[i][1] = g;
            ch->led_buffer[i][2] = b;
        }
        WS2812_SendChannel(ch);
        Delay_Ms(speed);
    }
    
    // Pulse down
    for (uint8_t brightness = 255; brightness > 0; brightness--) {
        uint8_t r = (uint16_t)red * brightness / 255;
        uint8_t g = (uint16_t)green * brightness / 255;
        uint8_t b = (uint16_t)blue * brightness / 255;
        
        for (uint16_t i = 0; i < ch->led_count; i++) {
            ch->led_buffer[i][0] = r;
            ch->led_buffer[i][1] = g;
            ch->led_buffer[i][2] = b;
        }
        WS2812_SendChannel(ch);
        Delay_Ms(speed);
    }
}

// Rainbow effect with configurable width
void LED_RAINBOWS(uint8_t channel_idx, uint16_t speed, uint16_t width) {
    WS2812_Channel_t* ch = LED_GetChannel(channel_idx);
    if (!ch || width == 0) return;
    
    for (uint8_t j = 0; j < 256; j++) {
        for (uint16_t i = 0; i < ch->led_count; i++) {
            uint8_t* color = Wheel((uint8_t)((i * 256 / width + j) & 255));
            ch->led_buffer[i][0] = color[0];
            ch->led_buffer[i][1] = color[1];
            ch->led_buffer[i][2] = color[2];
        }
        WS2812_SendChannel(ch);
        Delay_Ms(speed);
    }
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