/****************************************************************************
 * Copyright (C) 2017,2018 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "main.h"
#include "retain_vars.hpp"
#include "utils/WUPSConfigItemButtonCombo.h"
#include "utils/logger.h"
#include <coreinit/cache.h>
#include <string>
#include <vpad/input.h>
#include <wups.h>
#include <wups/config.h>
#include <wups/config/WUPSConfigItemBoolean.h>

// Mandatory plugin information.
WUPS_PLUGIN_NAME("SwipSwapMe");
WUPS_PLUGIN_DESCRIPTION("Swaps the GamePad and TV screen when pressing a certain button combination (TV button is the default)");
WUPS_PLUGIN_VERSION(VERSION_FULL);
WUPS_PLUGIN_AUTHOR("Maschell");
WUPS_PLUGIN_LICENSE("GPL");

// FS Access
WUPS_USE_WUT_DEVOPTAB();

WUPS_USE_STORAGE("SwipSwapMe");

#define ENABLED_CONFIG_STRING      "enabled"
#define TV_REMOTE_CONFIG_STRING    "tvRemoteDisabled"
#define BUTTON_COMBO_CONFIG_STRING "buttonCombo"

// Gets called once the loader exists.
INITIALIZE_PLUGIN() {
    initLogging();
    //memset(gVoiceInfos, 0, sizeof(gVoiceInfos));
    gButtonCombo = VPAD_BUTTON_TV;
    OSMemoryBarrier();

    // Open storage to read values
    WUPSStorageError storageRes = WUPS_OpenStorage();
    if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to open storage %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
    } else {
        // Try to get value from storage
        if ((storageRes = WUPS_GetBool(nullptr, ENABLED_CONFIG_STRING, &gEnabled)) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing.
            if (WUPS_StoreBool(nullptr, ENABLED_CONFIG_STRING, gEnabled) != WUPS_STORAGE_ERROR_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to store value");
            }
        } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get value %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
        }

        // Try to get value from storage
        if ((storageRes = WUPS_GetBool(nullptr, TV_REMOTE_CONFIG_STRING, &gTVRemoteDisabled)) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing.
            if (WUPS_StoreBool(nullptr, TV_REMOTE_CONFIG_STRING, gTVRemoteDisabled) != WUPS_STORAGE_ERROR_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to store value");
            }
        } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get value %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
        }

        // Try to get value from storage
        if ((storageRes = WUPS_GetInt(nullptr, BUTTON_COMBO_CONFIG_STRING, reinterpret_cast<int32_t *>(&gButtonCombo))) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing.
            if (WUPS_StoreInt(nullptr, BUTTON_COMBO_CONFIG_STRING, (int32_t) gButtonCombo) != WUPS_STORAGE_ERROR_SUCCESS) {
                DEBUG_FUNCTION_LINE_ERR("Failed to store value");
            }
        } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get value %s (%d)", WUPS_GetStorageStatusStr(storageRes), storageRes);
        }

        // Close storage
        if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to close storage");
        }
    }
}

void boolItemCallback(ConfigItemBoolean *item, bool newValue) {
    if (item && item->configId) {
        DEBUG_FUNCTION_LINE("New value in %s changed: %d", item->configId, newValue);
        if (std::string_view(item->configId) == ENABLED_CONFIG_STRING) {
            gEnabled = newValue;
            WUPS_StoreBool(nullptr, item->configId, gEnabled);
        }
        if (std::string_view(item->configId) == TV_REMOTE_CONFIG_STRING) {
            gTVRemoteDisabled = newValue;
            WUPS_StoreBool(nullptr, item->configId, gTVRemoteDisabled);
        }
    }
}

void buttonComboItemChanged(ConfigItemButtonCombo *item, uint32_t newValue) {
    if (item && item->configId) {
        DEBUG_FUNCTION_LINE("New value in %s changed: %d", item->configId, newValue);
        if (std::string_view(item->configId) == BUTTON_COMBO_CONFIG_STRING) {
            gButtonCombo = newValue;
            WUPS_StoreInt(nullptr, item->configId, (int32_t) gButtonCombo);
        }
    }
}

WUPS_GET_CONFIG() {
    // We open the storage, so we can persist the configuration the user did.
    if (WUPS_OpenStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to open storage");
        return 0;
    }

    WUPSConfigHandle config;
    WUPSConfig_CreateHandled(&config, "SwipSwapMe");

    WUPSConfigCategoryHandle setting;
    WUPSConfig_AddCategoryByNameHandled(config, "Settings", &setting);

    WUPSConfigItemBoolean_AddToCategoryHandled(config, setting, ENABLED_CONFIG_STRING, "Enabled", gEnabled, &boolItemCallback);

    WUPSConfigItemButtonCombo_AddToCategoryHandled(config, setting, BUTTON_COMBO_CONFIG_STRING, "Button combo", gButtonCombo, &buttonComboItemChanged);

    WUPSConfigItemBoolean_AddToCategoryHandled(config, setting, TV_REMOTE_CONFIG_STRING, "Disable TV remote", gTVRemoteDisabled, &boolItemCallback);

    return config;
}

WUPS_CONFIG_CLOSED() {
    // Save all changes
    if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to close storage");
    }
}

// Called whenever an application was started.
ON_APPLICATION_START() {
    initLogging();
}

ON_APPLICATION_REQUESTS_EXIT() {
    deinitLogging();
}
