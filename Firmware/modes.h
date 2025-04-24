#ifndef MODES_H
#define MODES_H

//#include "signals.h"
#include <stdlib.h>
#include <math.h>

void set_red(uint8_t x) {
    OCR1B = x;
}
void set_green(uint8_t x) {
     OCR2A = x;
}
void set_blue(uint8_t x) {
     OCR1A = x;
}
void set_blue_450(uint8_t x) {
     OCR0A = x;
}
void set_violet(uint8_t x) {
     OCR2B = x;
}
void set_uv_365(uint8_t x) {
     OCR0B = x;
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LIMIT(a,l,h) MAX(MIN(a, h), l)

uint16_t random_uint(uint16_t min, uint16_t max) {
    return (uint16_t)(min + (float)rand() / RAND_MAX * (max - min));
}
float random_float(float min, float max) {
    return (min + (float)rand() / RAND_MAX * (max - min));
}


const float gamma = 2.2f;

class Color {

    public:
    float red;
    float green;
    float blue;
    float blue_450;
    float violet;
    float uv_365;

    Color() : red(0), green(0), blue(0), blue_450(0), violet(0), uv_365(0) {};
    Color(
        float red,
        float green,
        float blue,
        float blue_450,
        float violet,
        float uv_365) : red(red),green(green),blue(blue),blue_450(blue_450),violet(violet),uv_365(uv_365) {
        }
    
    void apply(float power) {
        if (power < 0) power = 0.0f;
        if (power > 1) power = 1.0f;
        set_red((uint8_t)(powf(LIMIT(red, 0.0f, 1.0f) * power, gamma) * 0xFF));
        set_green((uint8_t)(powf(LIMIT(green, 0.0f, 1.0f) * power, gamma) * 0xFF));
        set_blue((uint8_t)(powf(LIMIT(blue, 0.0f, 1.0f) * power, gamma) * 0xFF));
        set_blue_450((uint8_t)(powf(LIMIT(blue_450, 0.0f, 1.0f) * power, gamma) * 0xFF));
        set_violet((uint8_t)(powf(LIMIT(violet, 0.0f, 1.0f) * power, gamma) * 0xFF));
        set_uv_365((uint8_t)(powf(LIMIT(uv_365, 0.0f, 1.0f) * power, gamma) * 0xFF));
    }

    Color operator+ (const Color & first) const
    {
        return Color(
            red + first.red,
            green + first.green,
            blue + first.blue,
            blue_450 + first.blue_450,
            violet + first.violet,
            uv_365 + first.uv_365
            );
    }

    void operator- (void)
    {
        red = -red;
        green = -green;
        blue = -blue;
        blue_450 = -blue_450;
        violet = -violet;
        uv_365 = -uv_365;

    }

    Color operator-(Color &rhs) {
        return Color(
            red - rhs.red,
            green - rhs.green,
            blue - rhs.blue,
            blue_450 - rhs.blue_450,
            violet - rhs.violet,
            uv_365 - rhs.uv_365
        );
    }

    Color operator* (const float ratio) const
    {
        return Color(
            ratio * red,
            ratio * green,
            ratio * blue,
            ratio * blue_450,
            ratio * violet,
            ratio * uv_365
            );
    }
};

// R, G, B, B450, V, UV
Color colors[] = {
    {1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 1},
    {0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 1},
};

const unsigned short N_COLORS = sizeof(colors) / sizeof(Color);

const float power_increment_start_stop = 1.0f / 2.0f / 200.0f;
const float power_increment_button = 1.0f / 2.0f / 500.0f;
const float min_power = 0.2f;
const float max_power = 1.0f;

class Mode {
    protected:
    enum State {STARTING, RUNNING, STOPPING, STOP};
    static State state;
    static float relative_power;
    static float relative_power_start_target;
    public:

    static void light_up() {
        relative_power += power_increment_button;
        if (relative_power > max_power) {
            relative_power = max_power;
        }
        if (state == RUNNING) {
            relative_power_start_target = relative_power;
        }
    }
    static void light_down() {
        relative_power -= power_increment_button;
        if (relative_power < min_power) {
            relative_power = min_power;
        }
        if (state == RUNNING) {
            relative_power_start_target = relative_power;
        }
    }

    Mode() {
    };
    
    virtual void ms_tick() {
        switch(state) {
            case STARTING:
                if (relative_power < relative_power_start_target) {
                    relative_power += power_increment_start_stop;
                    if (relative_power >= relative_power_start_target) {
                        relative_power = relative_power_start_target;
                        state = RUNNING;
                    }
                }
                break;
            case RUNNING:
                // Nothing
                break;
            case STOPPING:
                if (relative_power > 0.0f) {
                    relative_power -= power_increment_start_stop;
                    if (relative_power <= 0.0f) {
                        relative_power = 0.0f;
                        state = STOP;
                    }
                }
                break;
            case STOP:
                // Nothing
                break;
        }
    };
    void start() {
        state = STARTING;
    }
    bool stop_ok() {
        return state == STOP;
    }
    void stop() {
        state = STOPPING;
    }
    virtual void color_up() = 0;
    virtual void color_down() = 0;
};
Mode::State Mode::state = STOP;
float Mode::relative_power_start_target = 0.3f; // Initial value for power
float Mode::relative_power = 0.0f; // Start at 0

class Fixed : public Mode {
    protected:
    uint8_t color_index;

    Color color1;
    Color color2;
    public:
    Fixed() {
        color_index = 0;

        color1 = Color(1, 1, 0, 0, 0, 0);
        color2 = Color(0, 1, 0, 0, 0, 0);
    };

    void color_up() {
        if (color_index < N_COLORS - 1) {
            color_index++;
        }
    }

    void color_down() {
        if (color_index > 0) {
            color_index--;
        }
    }

    void ms_tick() {
        Mode::ms_tick();
        colors[color_index].apply(relative_power);
    }
};


class Rainbow : public Mode {
    protected:
    uint16_t new_color_delay;
    uint16_t new_color_counter = 0;
    Color new_color;
    Color current_color;
    uint8_t old_N = 0;
    Color color_offset;

    public:
    Rainbow() {
        new_color_delay = 2000;
        //make_random(&current_color);
        //current_color = colors[0];
    };

    void ms_tick() {
        Mode::ms_tick();
        // Choose a new color every xxx ms
        if (new_color_counter == 0) {
            new_color_counter = new_color_delay;
            new_color = colors[random_uint(0, N_COLORS-1)];
            color_offset = (new_color - current_color) * (1.0f / (float)new_color_delay);
        }
        else {
            new_color_counter--;
        }
        current_color = current_color + color_offset;

        current_color.apply(relative_power);
    }

    void color_up() {};
    void color_down() {};
};




class Flashing : public Mode {
    Color flash;

    float currentNoiseValue = 0;
    const float noiseChangeRate = 0.004; // Controls how fast the noise evolves

    float generateSmoothNoise() {
        // Generate a random small change
        float randomStep = (float)(rand() % 1000) / 1000.0 - 0.5; // A small random value between -0.5 and 0.5
        currentNoiseValue += randomStep * noiseChangeRate;
        
        // Clamp the value between 0 and 1 (or adjust based on the light intensity range you want)
        if (currentNoiseValue < 0) currentNoiseValue = 0;
        if (currentNoiseValue > 1) currentNoiseValue = 1;
        
        return currentNoiseValue;
    }
    
    Color base_color;
    public:

    Flashing() : base_color(0, 0, 0, 0.15, 1, 0) {
        flash = Color(1, 0, 0, 0.2, 0, 1);
    };

    void ms_tick() {
        Mode::ms_tick();

        float noise = generateSmoothNoise() * 20.0f;

        (base_color + (flash * noise)).apply(relative_power);
    }

    void color_up() {};
    void color_down() {};
};


class Breathing : public Fixed {
    float angle;
    float angle_increment;
    const float breath_per_minute;
    float breathing_power;
    public:
        Breathing() : breath_per_minute(10) {
            angle_increment = 2 * M_PI * breath_per_minute / 1000.0f / 60.0f;
            angle = 0.0f;
        }

    void ms_tick() {
        angle += angle_increment;
        if (angle > M_PI * 2) {
            angle -= M_PI * 2;
        }

        breathing_power = pow(M_E, sin(angle)) / M_E;

        Mode::ms_tick();
        colors[color_index].apply(relative_power * breathing_power);

    }
};

class Epilepsy : public Rainbow {
    public:
    Epilepsy() {
        new_color_delay = 25;
    }
};



#endif