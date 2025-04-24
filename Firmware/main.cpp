#include "main.h"

volatile unsigned char ovf_counter = 0;
volatile unsigned short ms_flag = false;

int main(void) {
    configure_pwm_timers();
    configure_inputs_outputs();

    _delay_ms(50);
    // Start the device
    power_lock.set_output(true);
    // Enable interrupts
    sei();

    int8_t mode_index = default_mode_index;
    int8_t target_mode_index = default_mode_index;
    Mode* current_mode = modes[mode_index];

    current_mode->start();

    while (1) {
        // Main routine
        if (ms_flag) {
            ms_flag = false;
            update_buttons();
            current_mode = modes[mode_index];

            // Color buttons
            if (color_up.pressed()) {
                current_mode->color_up();
            }
            else if(color_down.pressed()) {
                current_mode->color_down();
            }
            // power buttons
            if (light_down.is_pressed()) {
                current_mode->light_down();
            }
            else if(light_up.is_pressed()) {
                current_mode->light_up();
            }

            if (mode_index >= 0) {
                // Valid mode
                // This is where everything happens
                modes[mode_index]->ms_tick();  
            } else {
                // Stop
                power_lock.set_output(0);
            }

            // Power off
            if (power_button.pressed()) {
                target_mode_index = -1;
                current_mode->stop();
            }

            // Mode change
            if (target_mode_index == mode_index) {
                // Mode change is possible
                if (mode.pressed()) {
                    target_mode_index++;
                    if (target_mode_index == N_MODES) {
                        target_mode_index = 0;
                    }

                    current_mode->stop();
                }
            } else {
                // Wait for the mode to finish
                if (current_mode->stop_ok()) {
                    mode_index = target_mode_index;
                    // Start the new mode
                    modes[mode_index]->start();
                }
            }
        }
    }
}

ISR(TIMER0_OVF_vect) {
    // Interrupt runs at 1.98 kHz (no idea why)
    ovf_counter++;
    if (ovf_counter == 2) {
        ovf_counter = 0;
        // Run at 1kHz
        ms_flag = true;
    }
}