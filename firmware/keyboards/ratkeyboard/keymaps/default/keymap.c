#include QMK_KEYBOARD_H
#include <string.h>

// variables in mcu
bool is_media_playing = true;  // tracking if media playing kinda
uint16_t current_speed = 250;  // speed for text movement (lower is faster)

// mapping for keys 
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_A,    KC_B,    KC_TRNS,  // re click stuff below, it does play and play rest of uh keys are keys
        KC_C,    KC_D
    )
};

// custom re press command to pause and play
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // check if its down
    if (record->event.key.row == 0 && record->event.key.col == 2) {
        if (record->event.pressed) {
            is_media_playing = !is_media_playing; // toggle animation
            tap_code(KC_MPLY);                   // send windows play/pause
        }
        return false;
    }
    return true;
}

// windows volume makes animation speed or slow
#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU); // send windows volume command
        
        // volup is faster
        if (current_speed > 50) {
            current_speed -= 15; 
        }
    } else {
        tap_code(KC_VOLD); // send volddown command
        
        // vol down slows
        if (current_speed < 800) {
            current_speed += 15;
        }
    }
    return false;
}
#endif

// oled
#ifdef OLED_ENABLE
const char rat_text[] = " i love rats <3";
uint8_t scroll_offset = 0;
uint16_t scroll_timer = 0;

bool oled_task_user(void) {
    oled_write_P(PSTR("Status: Active\n"), false);

    // move text if playing
    if (is_media_playing) {
        if (timer_elapsed(scroll_timer) > current_speed) {
            scroll_offset++;
            if (scroll_offset >= strlen(rat_text)) {
                scroll_offset = 0;
            }
            scroll_timer = timer_read();
        }
    }

    // looped string
    char display_buffer[17];
    uint8_t text_len = strlen(rat_text);
    for (uint8_t i = 0; i < 16; i++) {
        uint8_t text_index = (scroll_offset + i) % text_len;
        display_buffer[i] = rat_text[text_index];
    }
    display_buffer[16] = '\0';

    oled_write(display_buffer, false);
    return false;
}
#endif
