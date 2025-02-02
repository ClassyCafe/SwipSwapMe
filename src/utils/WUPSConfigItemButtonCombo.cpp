#include "WUPSConfigItemButtonCombo.h"
#include "utils/input.h"
#include <coreinit/debug.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vpad/input.h>
#include <wups.h>

std::string getButtonChar(VPADButtons value) {
    std::string combo;
    if (value & VPAD_BUTTON_A) {
        return "\ue000";
    }
    if (value & VPAD_BUTTON_B) {
        return "\ue001";
    }
    if (value & VPAD_BUTTON_X) {
        return "\ue002";
    }
    if (value & VPAD_BUTTON_Y) {
        return "\ue003";
    }
    if (value & VPAD_BUTTON_L) {
        return "\ue083";
    }
    if (value & VPAD_BUTTON_R) {
        return "\ue084";
    }
    if (value & VPAD_BUTTON_ZL) {
        return "\ue085";
    }
    if (value & VPAD_BUTTON_ZR) {
        return "\ue086";
    }
    if (value & VPAD_BUTTON_UP) {
        return "\ue079";
    }
    if (value & VPAD_BUTTON_DOWN) {
        return "\ue07A";
    }
    if (value & VPAD_BUTTON_LEFT) {
        return "\ue07B";
    }
    if (value & VPAD_BUTTON_RIGHT) {
        return "\ue07C";
    }
    if (value & VPAD_BUTTON_STICK_L) {
        return "\ue081";
    }
    if (value & VPAD_BUTTON_STICK_R) {
        return "\ue082";
    }
    if (value & VPAD_BUTTON_PLUS) {
        return "\ue045";
    }
    if (value & VPAD_BUTTON_MINUS) {
        return "\ue046";
    }
    if (value & VPAD_BUTTON_TV) {
        return "\ue089";
    }
    return "?";
}

std::string getComboAsString(uint32_t value) {
    std::string combo;
    if (value & VPAD_BUTTON_A) {
        combo += getButtonChar(VPAD_BUTTON_A).append("+");
    }
    if (value & VPAD_BUTTON_B) {
        combo += getButtonChar(VPAD_BUTTON_B).append("+");
    }
    if (value & VPAD_BUTTON_X) {
        combo += getButtonChar(VPAD_BUTTON_X).append("+");
    }
    if (value & VPAD_BUTTON_Y) {
        combo += getButtonChar(VPAD_BUTTON_Y).append("+");
    }
    if (value & VPAD_BUTTON_L) {
        combo += getButtonChar(VPAD_BUTTON_L).append("+");
    }
    if (value & VPAD_BUTTON_R) {
        combo += getButtonChar(VPAD_BUTTON_R).append("+");
    }
    if (value & VPAD_BUTTON_ZL) {
        combo += getButtonChar(VPAD_BUTTON_ZL).append("+");
    }
    if (value & VPAD_BUTTON_ZR) {
        combo += getButtonChar(VPAD_BUTTON_ZR).append("+");
    }
    if (value & VPAD_BUTTON_UP) {
        combo += getButtonChar(VPAD_BUTTON_UP).append("+");
    }
    if (value & VPAD_BUTTON_DOWN) {
        combo += getButtonChar(VPAD_BUTTON_DOWN).append("+");
    }
    if (value & VPAD_BUTTON_LEFT) {
        combo += getButtonChar(VPAD_BUTTON_LEFT).append("+");
    }
    if (value & VPAD_BUTTON_RIGHT) {
        combo += getButtonChar(VPAD_BUTTON_RIGHT).append("+");
    }
    if (value & VPAD_BUTTON_STICK_L) {
        combo += getButtonChar(VPAD_BUTTON_STICK_L).append("+");
    }
    if (value & VPAD_BUTTON_STICK_R) {
        combo += getButtonChar(VPAD_BUTTON_STICK_R).append("+");
    }
    if (value & VPAD_BUTTON_PLUS) {
        combo += getButtonChar(VPAD_BUTTON_PLUS).append("+");
    }
    if (value & VPAD_BUTTON_MINUS) {
        combo += getButtonChar(VPAD_BUTTON_MINUS).append("+");
    }
    if (value & VPAD_BUTTON_TV) {
        combo += getButtonChar(VPAD_BUTTON_TV).append("+");
    }
    if (combo.ends_with("+")) {
        combo.pop_back();
    }
    return combo;
}

int32_t WUPSConfigItemButtonCombo_getCurrentValueDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemButtonCombo *) context;
    snprintf(out_buf, out_size, "%s", getComboAsString(item->value).c_str());
    return 0;
}

bool WUPSConfigItemButtonCombo_callCallback(void *context) {
    auto *item = (ConfigItemButtonCombo *) context;
    if (item->callback != nullptr) {
        ((ButtonComboValueChangedCallback) (item->callback))(item, item->value);
        return true;
    }
    return false;
}

void checkForHold(ConfigItemButtonCombo *item) {
    uint32_t lastHold        = 0;
    uint32_t holdFor         = 0;
    uint32_t holdForTarget   = item->holdDurationInMs >> 3;
    uint32_t holdAbortTarget = item->abortButtonHoldDurationInMs >> 3;

    auto mask = VPAD_BUTTON_A | VPAD_BUTTON_B | VPAD_BUTTON_X | VPAD_BUTTON_Y | VPAD_BUTTON_L | VPAD_BUTTON_R |
                VPAD_BUTTON_ZL | VPAD_BUTTON_ZR | VPAD_BUTTON_UP | VPAD_BUTTON_DOWN | VPAD_BUTTON_LEFT | VPAD_BUTTON_RIGHT |
                VPAD_BUTTON_STICK_L | VPAD_BUTTON_STICK_R | VPAD_BUTTON_PLUS | VPAD_BUTTON_MINUS | VPAD_BUTTON_TV;

    KPADStatus kpad_data{};
    KPADError kpad_error;

    while (true) {
        uint32_t buttonsHold     = 0;
        VPADReadError vpad_error = VPAD_READ_UNINITIALIZED;
        VPADStatus vpad_data;
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &vpad_error);
        if (vpad_error == VPAD_READ_SUCCESS) {
            buttonsHold = vpad_data.hold;
        }

        for (int i = 0; i < 4; i++) {
            memset(&kpad_data, 0, sizeof(kpad_data));
            if (KPADReadEx((KPADChan) i, &kpad_data, 1, &kpad_error) > 0) {
                if (kpad_error == KPAD_ERROR_OK && kpad_data.extensionType != 0xFF) {
                    if (kpad_data.extensionType == WPAD_EXT_CORE || kpad_data.extensionType == WPAD_EXT_NUNCHUK) {
                        buttonsHold |= remapWiiMoteButtons(kpad_data.hold);
                    } else {
                        buttonsHold |= remapClassicButtons(kpad_data.classic.hold);
                    }
                }
            }
        }

        buttonsHold &= mask;

        if (buttonsHold == lastHold) {
            if (buttonsHold != 0) {
                holdFor++;
            }
        } else {
            holdFor = 0;
        }
        lastHold = buttonsHold;

        if (holdFor >= holdAbortTarget && lastHold == item->abortButton) {
            break;
        }

        if (holdFor >= holdForTarget) {
            item->value = lastHold;
            break;
        }
        OSSleepTicks(OSMillisecondsToTicks(8));
    }
}

void WUPSConfigItemButtonCombo_onButtonPressed(void *context, WUPSConfigButtons buttons) {
    auto *item = (ConfigItemButtonCombo *) context;
    if (item->state == BUTTON_COMBO_STATE_NONE) {
        if ((buttons & WUPS_CONFIG_BUTTON_A) == WUPS_CONFIG_BUTTON_A) {
            item->state = BUTTON_COMBO_STATE_PREPARE_FOR_HOLD;
        }
    }
}

bool WUPSConfigItemButtonCombo_isMovementAllowed(void *context) {
    return true;
}

int32_t WUPSConfigItemButtonCombo_getCurrentValueSelectedDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemButtonCombo *) context;
    if (item->state == BUTTON_COMBO_STATE_PREPARE_FOR_HOLD || item->state == BUTTON_COMBO_STATE_WAIT_FOR_HOLD) {
        if (item->state == BUTTON_COMBO_STATE_PREPARE_FOR_HOLD) {
            item->state = BUTTON_COMBO_STATE_WAIT_FOR_HOLD;
            snprintf(out_buf, out_size, "<Hold new combo for %dms; hold %s to abort>", item->holdDurationInMs, getButtonChar(item->abortButton).c_str());
            return 0;
        } else {
            checkForHold(item);
            item->state = BUTTON_COMBO_STATE_NONE;
        }
    }
    snprintf(out_buf, out_size, "(Press \ue000 to change) %s", getComboAsString(item->value).c_str());
    return 0;
}

void WUPSConfigItemButtonCombo_restoreDefault(void *context) {
    auto *item  = (ConfigItemButtonCombo *) context;
    item->value = item->defaultValue;
}

void WUPSConfigItemButtonCombo_onSelected(void *context, bool isSelected) {
}

void WUPSConfigItemButtonCombo_onDelete(void *context) {
    auto *item = (ConfigItemButtonCombo *) context;
    if (item->configId) {
        free(item->configId);
    }
    free(item);
}

extern "C" bool
WUPSConfigItemButtonComboAddToCategoryEx(WUPSConfigCategoryHandle cat, const char *configID, const char *displayName, uint32_t defaultComboInVPADButtons, uint32_t holdDurationInMs, VPADButtons abortButton, uint32_t abortButtonHoldDurationInMs, ButtonComboValueChangedCallback callback) {
    if (cat == 0) {
        return false;
    }
    auto *item = (ConfigItemButtonCombo *) malloc(sizeof(ConfigItemButtonCombo));
    if (item == nullptr) {
        OSReport("WUPSConfigItemButtonComboAddToCategoryEx: Failed to allocate memory for item data.\n");
        return false;
    }

    if (configID != nullptr) {
        item->configId = strdup(configID);
    } else {
        item->configId = nullptr;
    }

    item->abortButton                 = abortButton;
    item->abortButtonHoldDurationInMs = abortButtonHoldDurationInMs;
    item->holdDurationInMs            = holdDurationInMs;
    item->defaultValue                = defaultComboInVPADButtons;
    item->value                       = defaultComboInVPADButtons;
    item->callback                    = (void *) callback;
    item->state                       = BUTTON_COMBO_STATE_NONE;

    WUPSConfigCallbacks_t callbacks = {
            .getCurrentValueDisplay         = &WUPSConfigItemButtonCombo_getCurrentValueDisplay,
            .getCurrentValueSelectedDisplay = &WUPSConfigItemButtonCombo_getCurrentValueSelectedDisplay,
            .onSelected                     = &WUPSConfigItemButtonCombo_onSelected,
            .restoreDefault                 = &WUPSConfigItemButtonCombo_restoreDefault,
            .isMovementAllowed              = &WUPSConfigItemButtonCombo_isMovementAllowed,
            .callCallback                   = &WUPSConfigItemButtonCombo_callCallback,
            .onButtonPressed                = &WUPSConfigItemButtonCombo_onButtonPressed,
            .onDelete                       = &WUPSConfigItemButtonCombo_onDelete};

    if (WUPSConfigItem_Create(&item->handle, configID, displayName, callbacks, item) < 0) {
        OSReport("WUPSConfigItemButtonComboAddToCategoryEx: Failed to create config item.\n");
        free(item);
        return false;
    }

    if (WUPSConfigCategory_AddItem(cat, item->handle) < 0) {
        OSReport("WUPSConfigItemButtonComboAddToCategoryEx: Failed to add item to category.\n");
        WUPSConfigItem_Destroy(item->handle);
        return false;
    }
    return true;
}

extern "C" bool
WUPSConfigItemButtonComboAddToCategory(WUPSConfigCategoryHandle cat, const char *configID, const char *displayName, uint32_t defaultComboInVPADButtons, ButtonComboValueChangedCallback callback) {
    return WUPSConfigItemButtonComboAddToCategoryEx(cat, configID, displayName, defaultComboInVPADButtons, 2000, VPAD_BUTTON_B, 250, callback);
}