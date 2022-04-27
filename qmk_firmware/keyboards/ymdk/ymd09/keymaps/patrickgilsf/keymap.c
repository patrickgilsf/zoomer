// Copyright 2022 Patrick Gilligan (@patrickgilsf)
// SPDX-License-Identifier: GPL-2.0-or-later

/*
This is my 9 key YMDK that I use to control Zoom.
*/

#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include "print.h"
#include "timer.h"

#define _MAC 0
#define _PC 1
#define _UTIL 2

/*
HSV Values
RGB_RED == (0,255,255);
RGB_ORANGE == (28,255,255);
RGB_YELLOW == (43,255,255);
RGB_GREEN == (85,255,127);
Gunmetal Grey = (60,18,145);
Windows Blue = (144,216,237);
Light Red = (0,127,255);
White = (0,0,255)
Purple = (245,84,204)
*/

enum custom_keycodes {
    macMicMute = SAFE_RANGE,
    macVidMute,
    macShareScreen,
    macGalleryToggle,
    macGalleryLeft,
    macGalleryRight,
    macFullscreen,
    macChat,
    macMini,
    winMicMute,
    winVidMute,
    winShareScreen,
    winGalleryToggle,
    winGalleryLeft,
    winGalleryRight,
    winFullscreen,
    winChat,
    winMini,
    utilOne,
    utilTwo,
    utilThree,
    utilFour,
    utilFive,
    utilSix,
    utilSeven,
    utilEight,
    flashKeeb
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_MAC] = LAYOUT(macMicMute, macVidMute, macShareScreen,
               macGalleryToggle, macGalleryLeft, macGalleryRight,
               macFullscreen, macChat, macMini),

  [_PC] = LAYOUT(winMicMute, winVidMute, winShareScreen,
               winGalleryToggle, winGalleryLeft, winGalleryRight,
               winFullscreen, winChat, winMini),

  [_UTIL] = LAYOUT(utilOne, utilTwo, utilThree,
               utilFour, utilFive, utilSix,
               utilSeven, utilEight, flashKeeb)
};

/*
Legend:
--Layer 0 - [Mac OS Zoom shortcuts]
macMicMute - Mute Mic/Toggle Keyboard Light;
macVidMute - Mute Video;
macShareScreen - Raise/Lower Hand;
macGalleryToggle - Speaker/Gallery View Toggle
macGalleryLeft - Prev Page of Gallery
macGalleryRight - Next Page of Gallery
macFullscreen - Full Screen
macChat - Show/Hide Chat; // If macFullscreen is down, up to layer 1.
macMini - Minimal Window;
--Layer 1 - [PC Zoom Shortcuts]
winMicMute - Mute Mic/Toggle Keyboard Light;
winVidMute - Mute Video; // If winMicMute is down, back to layer 0
winShareScreen - Raise/Lower Hand;
winGalleryToggle - Speaker/Gallery View Toggle
winGalleryLeft - Prev Page of Gallery
winGalleryRight - Next Page of Gallery
winFullscreen - Full Screen
winChat - Show/Hide Chat; // If macFullscreen is down, down to layer 0.
winMini - Minimal Window; // If macFullscreen is down, up to layer 2
-- Layer 2 -
utilOne -
utilTwo -
utilThree -
utilFour -
utilFive -
utilSix -
utilSeven -
utilEight - If utilSeven is down, back to layer 1
flashKeeb - If utilOne && utilSeven are down, puts it in setup mode
*/

/* LEDs mapped out
2 1 0
5 4 3
8 7 6
*/

//boolean values allow me to make one key press contingent on a different keypress
//_MAC and _PC
bool mic_Muted = 1;
bool vid_Muted = 1;
bool screen_is_sharing = 0;
//_MAC
bool mac_screen_share_press = 0;
bool mac_Full_Shift = 0;
bool mac_chat_open = 0;
//_PC
bool win_screen_share_press = 0;
bool win_Full_Shift = 0;
bool win_chat_open = 0;
//_UTIL
bool flash_one = 0;
bool flash_two = 0;
bool kp_27 = 0;

//https://docs.qmk.fm/#/feature_rgblight?id=defining-lighting-layers
const rgblight_segment_t PROGMEM micMuteLED[] = RGBLIGHT_LAYER_SEGMENTS(
    {2, 1, 0,255,255}
);
const rgblight_segment_t PROGMEM vidMuteLED[] = RGBLIGHT_LAYER_SEGMENTS(
    {1, 1, 0,255,255}
);
const rgblight_segment_t PROGMEM chatLED[] = RGBLIGHT_LAYER_SEGMENTS(
    {7, 1, HSV_YELLOW}
);
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    micMuteLED,
    vidMuteLED,
    chatLED
);

//these are the initialization functions
void keyboard_post_init_user(void) {
rgblight_sethsv_noeeprom(60,80,145);
rgblight_layers = my_rgb_layers;
rgblight_enable_noeeprom();
rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
layer_state_set_user(_MAC);
set_single_persistent_default_layer(_MAC);
debug_enable=1;
}

//raw HID - connects with my node script
void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (data[1] == 0x01) {
        // rgblight_sethsv_at(0,255,255, 2);
        // rgblight_sethsv_at(0,255,255, 1);
        mic_Muted = 1;
        vid_Muted = 1;
        rgblight_set_layer_state(0, mic_Muted = 1);
        rgblight_set_layer_state(1, vid_Muted = 1);
        print("Mic Muted Video Stop");
    } else if (data[1] == 0x02) { //need to add a query of which layer you are on for PC
        // rgblight_sethsv_at(0,255,255, 2);
        // rgblight_sethsv_at(60,18,145, 1);
        mic_Muted = 1;
        vid_Muted = 0;
        rgblight_set_layer_state(0, mic_Muted = 1);
        rgblight_set_layer_state(1, vid_Muted = 0);
        print("Mic Muted Video Start");
    } else if (data[1] == 0x03) {
        // rgblight_sethsv_at(60,18,145, 2);
        // rgblight_sethsv_at(0,255,255, 1);
        mic_Muted = 0;
        vid_Muted = 1;
        rgblight_set_layer_state(0, mic_Muted = 0);
        rgblight_set_layer_state(1, vid_Muted = 1);
        print("Mic Unmuted Video Stop");
    } else if (data[1] == 0x04) {
        // rgblight_sethsv_at(60,18,145,2);
        // rgblight_sethsv_at(60,18,145,1);
        mic_Muted = 0;
        vid_Muted = 0;
        rgblight_set_layer_state(0, mic_Muted = 0);
        rgblight_set_layer_state(1, vid_Muted = 0);
        print("Mic Unmuted Video Start");
    }
}

//layer control
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        default:
        break;
        case _MAC:
            print("on mac layer");
            rgblight_sethsv_noeeprom(60,80,145);
            if (mic_Muted == 1) {
                rgblight_set_layer_state(0, mic_Muted = 1);
            } else if (mic_Muted == 0) {
                rgblight_set_layer_state(0, mic_Muted = 0);
            }
            if (vid_Muted == 1) {
                rgblight_set_layer_state(1, vid_Muted = 1);
            } else if (vid_Muted == 0) {
                rgblight_set_layer_state(1, vid_Muted = 0);
            }
        break;
        case _PC:
            print("on pc layer");
            rgblight_sethsv_noeeprom(144,216,237);
            if (mic_Muted == 1) {
                rgblight_set_layer_state(0, mic_Muted = 1);
            } else if (mic_Muted == 0) {
                rgblight_set_layer_state(0, mic_Muted = 0);
            }
            if (vid_Muted == 1) {
                rgblight_set_layer_state(1, vid_Muted = 1);
            } else if (vid_Muted == 0) {
                rgblight_set_layer_state(1, vid_Muted = 0);
            }
        break;
        case _UTIL:
        print("on util layer");
            rgblight_sethsv_noeeprom(85,255,127);
            if (mic_Muted == 1) {
                rgblight_set_layer_state(0, mic_Muted = 1);
            } else if (mic_Muted == 0) {
                rgblight_set_layer_state(0, mic_Muted = 0);
            }
            if (vid_Muted == 1) {
                rgblight_set_layer_state(1, vid_Muted = 1);
            } else if (vid_Muted == 0) {
                rgblight_set_layer_state(1, vid_Muted = 0);
            }
        break;
        };
return state;
}

//timer for press and hold
static uint16_t macSSTimer;
static uint16_t winSSTimer;

//custom keys
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
///////////mac layer/////////////////////////////////////
    case macMicMute: //Mic Mute
        if (record->event.pressed) {
        } else {
            if (mic_Muted == 0) {
                mic_Muted = 1;
                rgblight_set_layer_state(0, mic_Muted = 1);
                tap_code16(LSFT(LGUI(KC_A)));
            }   else if (mic_Muted == 1) {
                    mic_Muted = 0;
                    rgblight_set_layer_state(0, mic_Muted = 0);
                    tap_code16(LSFT(LGUI(KC_A)));
                };
        };
        break;
    case macVidMute: // Video Mute
        if (record->event.pressed) {
        } else {
            if (vid_Muted == 0) {
                vid_Muted = 1;
                rgblight_set_layer_state(1, vid_Muted = 1);
                tap_code16(LSFT(LGUI(KC_V)));
            } else if (vid_Muted == 1) {
                vid_Muted = 0;
                rgblight_set_layer_state(1, vid_Muted = 0);
                tap_code16(LSFT(LGUI(KC_V)));
            };
        };
        break;
    case macShareScreen:
        if (record->event.pressed) {
            macSSTimer = timer_read();
            mac_screen_share_press = 1; //matrix scan is listening for these events
        } else {
            mac_screen_share_press = 0;
            // if (screen_is_sharing == 1) { //deprecated idea
            //     tap_code16(LSFT(LGUI(KC_S)));
            //     screen_is_sharing = 0;
            //     rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
            // }
        }
        return false;
    case macGalleryToggle:
        if (record->event.pressed) {
        }   else {
                tap_code16(LSFT(LGUI(KC_W)));
        }
        break;
    case macGalleryLeft:
        if (record->event.pressed) {
        }   else {
                tap_code16(LCTL(KC_P));
        }
        break;
    case macGalleryRight:
        if (record->event.pressed) {
        }   else {
                tap_code16(LCTL(KC_N));
        }
        break;
    case macFullscreen:
        if (record->event.pressed) {
            mac_Full_Shift = 1;
        }   else {
                if (mac_Full_Shift == 1) {
                    tap_code16(LSFT(LGUI(KC_F)));
                    mac_Full_Shift = 0;
                };
                mac_Full_Shift = 0;
            }
        break;
    case macChat:
        if (record->event.pressed) {
        }   else {
                if (mac_Full_Shift == 0) {
                    if (mac_chat_open == 0) {
                        mac_chat_open = 1;
                        rgblight_set_layer_state(2, mac_chat_open = 1);
                        tap_code16(LSFT(LGUI(KC_H)));
                        // sethsv(HSV_YELLOW, (LED_TYPE *)&led[7]);
                        // rgblight_set();
                    }   else if (mac_chat_open == 1) {
                            mac_chat_open = 0;
                            rgblight_set_layer_state(2, mac_chat_open = 0);
                            tap_code16(LSFT(LGUI(KC_H)));
                            // sethsv(60,80,145, (LED_TYPE *)&led[7]);
                            // rgblight_set();
                        }
                }
                mac_Full_Shift = 0;
            }
        break;
    case macMini:
         if (record->event.pressed) {
            if (mac_Full_Shift == 1) {
                layer_move(_PC);
                mac_Full_Shift = 0;
            }
        }   else {
                if (layer_state_is(_MAC)) {
                    SEND_STRING(SS_LSFT(SS_LGUI("m")));
                }
            }
        break;
///////////windows layer/////////////////////////////////////
    case winMicMute:
        if (record->event.pressed) {
        } else {
            if (mic_Muted == 0) {
                mic_Muted = 1;
                rgblight_set_layer_state(0, mic_Muted = 1);
                tap_code16(LALT(KC_A));
            }   else if (mic_Muted == 1) {
                    mic_Muted = 0;
                    rgblight_set_layer_state(0, mic_Muted = 0);
                    tap_code16(LALT(KC_A));
                };
        };
        break;
    case winVidMute:
        if (record->event.pressed) {
        } else {
            if (vid_Muted == 0) {
                vid_Muted = 1;
                rgblight_set_layer_state(1, vid_Muted = 1);
                tap_code16(LALT(KC_V));
            } else if (vid_Muted == 1) {
                vid_Muted = 0;
                rgblight_set_layer_state(1, vid_Muted = 0);
                tap_code16(LALT(KC_V));
            };
        };
        break;
    case winShareScreen:
        if (record->event.pressed) {
            win_screen_share_press = 1; //matrix scan is listening for these events
            winSSTimer = timer_read();
        }   else {

                }
        break;
    case winGalleryToggle:
        if (record->event.pressed) {
            }   else {
                    tap_code16(LALT(KC_F2));
                }
        break;
    case winGalleryLeft:
        if (record->event.pressed) {
            }   else {
                    tap_code(KC_PGDOWN);
                }
        break;
    case winGalleryRight:
        if (record->event.pressed) {
            }   else {
                    tap_code16(KC_PGUP);
                }
        break;
    case winFullscreen:
        if (record->event.pressed) {
            win_Full_Shift = 1;
        }   else {
                if (win_Full_Shift == 1) {
                    tap_code16(LALT(KC_F));
                    win_Full_Shift = 0;
                };
                win_Full_Shift = 0;
            };
        break;
    case winChat:
        if (record->event.pressed) {
            if (win_Full_Shift == 1) {
                layer_move(_MAC);
                mac_Full_Shift = 1;
            }
        }   else {
                if (win_Full_Shift == 0) {
                    if (win_chat_open == 0) {
                        win_chat_open = 1;
                        rgblight_set_layer_state(2, mac_chat_open = 1);
                        tap_code16(LALT(KC_H));
                        // sethsv(HSV_YELLOW, (LED_TYPE *)&led[7]);
                        // rgblight_set();
                    } else if (win_chat_open == 1) {
                        win_chat_open = 0;
                        rgblight_set_layer_state(2, mac_chat_open = 0);
                        tap_code16(LALT(KC_H));
                        // sethsv(144,216,237, (LED_TYPE *)&led[7]);
                        // rgblight_set();
                    }
                }
                win_Full_Shift = 0;
            }
        break;
    case winMini:
        if (record->event.pressed) {
            if (win_Full_Shift) {
                layer_move(_UTIL);
            }
        }   else {
            }
        break;
///////////utility layer/////////////////////////////////////
    case utilOne:
        if (record->event.pressed) {
            flash_one = 1;
        }   else {
            flash_one = 0;
        }
        break;
    case utilTwo:
    case utilThree:
    case utilFour:
    case utilFive:
    case utilSix:
    case utilSeven:
        if (record->event.pressed) {
            flash_two = 1;
            win_Full_Shift = 1;
        }   else {
            flash_two = 0;
        }
        break;
    case utilEight:
        if (record->event.pressed) {
            if (flash_two) {
                layer_move(_PC);
                win_Full_Shift = 1;
            }
        }   else {
            }
        break;
    case flashKeeb:
        if (record->event.pressed) {
            kp_27 = 1;
            if (flash_one && flash_two) {
                rgblight_sethsv_noeeprom(0,0,255);
                rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
                reset_keyboard();
            }
       }   else {
            kp_27 = 0;
        }
        break;
    }
    return true;
};

//scans for my press and hold (screen share)...wish there was a better way, but alas
void matrix_scan_user(void) {
    if (mac_screen_share_press == 1 && timer_elapsed(macSSTimer) > 1500) {
        print("mac sharing");
        tap_code16(LSFT(LGUI(KC_S)));
        screen_is_sharing = 1;
        // rgblight_mode(RGBLIGHT_MODE_BREATHING);
        mac_screen_share_press = 0;
    };
    if (win_screen_share_press == 1 && timer_elapsed(winSSTimer) > 1500) {
        print("win sharing");
        tap_code16(LALT(LSFT(KC_S)));
        win_screen_share_press = 0;
    }
};
