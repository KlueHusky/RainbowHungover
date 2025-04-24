#define __AVR_ATmega328P__
#include <avr/io.h>
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <math.h>
#include "modes.h"

#ifndef MAIN_H
#define MAIN_H

/*
Power latch       PD1
Power button      PD0

Blue        OC1A  PB1
Red         OC1B  PB2
Green       OC2A  PB3
450nm       OC0A  PD6
420nm       OC2B  PD3
365nm       OC0B  PD5

Button0           PC0
Button1           PC1
Button2           PC2
Button3           PC3
Button4           PC4
*/

void configure_pwm_timers() {
    // Timer configuration
    TCCR0A = (0 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (0 << COM0A0) |
             (1 << COM0B1) | (0 << COM0B0);  // Mode 3, fast PWM, max at 0xFF
    TCCR0B = (0 << CS02) | (1 << CS01) | (0 << CS00);
    TIMSK0 = (1 << TOIE0);
    OCR0A = 0;
    OCR0B = 0;

    TCCR1A = (1 << WGM10) | (0 << WGM11) | (1 << COM1A1) | (0 << COM1A0) |
             (1 << COM1B1) | (0 << COM1B0);
    TCCR1B =
        (0 << WGM12) | (0 << WGM13) | (0 << CS12) | (1 << CS11) | (0 << CS10);
    OCR1A = 0;
    OCR1B = 0;

    TCCR2A = (1 << WGM20) | (0 << WGM21) | (1 << COM2A1) | (0 << COM2A0) |
             (1 << COM2B1) | (0 << COM2B0);
    TCCR2B = (0 << WGM22) | (0 << CS22) | (1 << CS21) | (0 << CS20);
    OCR2A = 0;
    OCR2B = 0;
}

void configure_inputs_outputs() {
    // Pin configuration
    DDRB = (1 << PB1) | (1 << PB2) | (1 << PB3);
    DDRC = (0 << PC0) | (0 << PC1) | (0 << PC2) | (0 << PC3) | (0 << PC4);
    DDRD = (0 << PD0) | (1 << PD1) | (0 << PD2) | (1 << PD3) | (1 << PD5) |
           (1 << PD6);

    // Pull-ups
    PORTC |= 0b00011111;
}

class IO {
    protected:
    uint8_t bit;
    uint8_t active_on;
    volatile uint8_t* _register;
    IO(volatile uint8_t* _register, uint8_t bit, uint8_t active_on) : _register(_register), bit(bit), active_on(active_on) {};
};


class Button : public IO {
    static const uint8_t debounce_delay = 20;
    uint8_t debounce_counter;
    bool press_flag;
    bool previous;
    public:
    Button(volatile uint8_t* pin_register, uint8_t bit, uint8_t active_on) : IO(pin_register, bit, active_on) {
        debounce_counter = 0;
        previous = instantaneous_value();
        press_flag = false;
    }

    bool instantaneous_value() {
        return (((*_register) & (1 << bit)) > 0) ^ !active_on;
    }

    bool pressed() {
        if (press_flag) {
            press_flag = false;
            return true;
        }
        return false;
    }

    bool is_pressed() {
        return previous;
    }

    void ms_tick() {
        bool new_value = instantaneous_value();
        if (previous != new_value && debounce_counter == 0) {
            if (new_value) {
                press_flag = true;
            }
            previous = new_value;
            debounce_counter = debounce_delay;
        }

        if (debounce_counter > 0) {
            debounce_counter--;
        }
    }
};

class Output : public IO {
    public:
    Output(volatile uint8_t* port_register, uint8_t bit, uint8_t active_on) : IO(port_register, bit, active_on) {}

    void set_output(bool enable) {
        if (!active_on) {
            enable = !enable;
        }
        *_register = (*_register) & ~(1 << bit) | (enable << bit);
    }
};



const float startup_power = 0.3f;
const float startup_power_increment_ms = 0.001f;
const float stop_power_decrement_ms = 0.002f;


const uint8_t default_color_index = 0;
const uint8_t default_mode_index = 0;

Button power_button(&PIND, PD0, 1);
Output power_lock(&PORTD, PD1, 1);

Button light_up(&PINC, PC1, 0);
Button light_down(&PINC, PC0, 0);
Button color_up(&PINC, PC4, 0);
Button mode(&PINC, PC3, 0);
Button color_down(&PINC, PC2, 0);

Fixed fixed_mode = Fixed();
Rainbow rainbow = Rainbow();
Flashing flashing = Flashing();
Breathing breathing = Breathing();
Epilepsy epilepsy = Epilepsy();

Mode* modes[] = {
    &fixed_mode,
    &rainbow,
    &flashing,
    &breathing,
    &epilepsy
};


const unsigned short N_MODES = sizeof(modes) / sizeof(Mode);

void update_buttons() {
    power_button.ms_tick();
    light_up.ms_tick();
    light_down.ms_tick();
    color_up.ms_tick();
    mode.ms_tick();
    color_down.ms_tick();
}



#endif // MAIN_H