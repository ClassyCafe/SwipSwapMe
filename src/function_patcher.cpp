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
#include "retain_vars.hpp"
#include "utils/input.h"
#include "utils/logger.h"
#include "utils/voice_swapper.h"
#include <coreinit/systeminfo.h>
#include <gx2/swap.h>
#include <padscore/wpad.h>
#include <vpad/input.h>
#include <wups.h>

static bool doSwap          = false;
static uint8_t swapCoolDown = 0;

DECL_FUNCTION(AXResult, AXSetVoiceDeviceMixOld, AXVoice *voice, AXDeviceType device, uint32_t id, AXVoiceDeviceMixData *mix) {

    if (doSwap) {
        device = !device;
    }

    DEBUG_FUNCTION_LINE("AXSetVoiceDeviceMixOld voice: %08X device: %d, mix: %08X\n", voice, device, mix);

    VoiceSwapper_setMix(voice, device, mix);
    return real_AXSetVoiceDeviceMixOld(voice, device, id, mix);
}

DECL_FUNCTION(AXResult, AXSetVoiceDeviceMix, AXVoice *voice, AXDeviceType device, uint32_t id, AXVoiceDeviceMixData *mix) {

    if (doSwap) {
        device = !device;
    }

    DEBUG_FUNCTION_LINE("AXSetVoiceDeviceMix voice: %08X device: %d, mix: %08X\n", voice, device, mix);

    VoiceSwapper_setMix(voice, device, mix);
    return real_AXSetVoiceDeviceMix(voice, device, id, mix);
}

DECL_FUNCTION(AXVoice *, AXAcquireVoiceExOld, uint32_t prio, AXVoiceCallbackExFn callback, uint32_t arg) {
    AXVoice *result = real_AXAcquireVoiceExOld(prio, callback, arg);

    DEBUG_FUNCTION_LINE("AXAcquireVoiceExOld result: %08X \n", result);

    VoiceSwapper_acquireVoice(result);
    return result;
}

DECL_FUNCTION(AXVoice *, AXAcquireVoiceEx, uint32_t priority, AXVoiceCallbackExFn callback, uint32_t arg) {
    AXVoice *result = real_AXAcquireVoiceEx(priority, callback, arg);

    DEBUG_FUNCTION_LINE("AXAcquireVoiceEx result: %08X \n", result);

    VoiceSwapper_acquireVoice(result);
    return result;
}

DECL_FUNCTION(void, AXFreeVoiceOld, AXVoice *voice) {

    DEBUG_FUNCTION_LINE("AXFreeVoiceOld voice: %08X \n", voice);

    VoiceSwapper_freeVoice(voice);
    real_AXFreeVoiceOld(voice);
}

DECL_FUNCTION(void, AXFreeVoice, AXVoice *voice) {

    DEBUG_FUNCTION_LINE("AXFreeVoice voice: %08X \n", voice);

    VoiceSwapper_freeVoice(voice);
    real_AXFreeVoice(voice);
}

DECL_FUNCTION(void, GX2CopyColorBufferToScanBuffer, const GX2ColorBuffer *colorBuffer, GX2ScanTarget scan_target) {
    if (doSwap) {
        if (scan_target == GX2_SCAN_TARGET_TV) {
            scan_target = GX2_SCAN_TARGET_DRC;
        } else if (scan_target == GX2_SCAN_TARGET_DRC) {
            scan_target = GX2_SCAN_TARGET_TV;
        }
    }
    real_GX2CopyColorBufferToScanBuffer(colorBuffer, scan_target);
}

/*
DECL(int32_t, AXSetDefaultMixerSelectOld, uint32_t s){
    int32_t result = real_AXSetDefaultMixerSelectOld(s);
    return result;
}*/

void swapVoices() {
    VoiceSwapper_swapAll();
    for (auto &gVoiceInfo : gVoiceInfos) {
        if (gVoiceInfo.voice == nullptr)
            continue;

        real_AXSetVoiceDeviceMix(gVoiceInfo.voice, AX_DEVICE_TYPE_TV, 0, gVoiceInfo.mixTV);
        real_AXSetVoiceDeviceMix(gVoiceInfo.voice, AX_DEVICE_TYPE_DRC, 0, gVoiceInfo.mixDRC);
        real_AXSetVoiceDeviceMixOld(gVoiceInfo.voice, AX_DEVICE_TYPE_TV, 0, gVoiceInfo.mixTV);
        real_AXSetVoiceDeviceMixOld(gVoiceInfo.voice, AX_DEVICE_TYPE_DRC, 0, gVoiceInfo.mixDRC);
    }
}

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffer, uint32_t buffer_size, VPADReadError *error) {
    VPADReadError real_error;
    int32_t result = real_VPADRead(chan, buffer, buffer_size, &real_error);
    if (gEnabled) {
        if (result > 0 && real_error == VPAD_READ_SUCCESS && (buffer[0].hold == gButtonCombo) && swapCoolDown == 0 && OSIsHomeButtonMenuEnabled()) {
            doSwap = !doSwap;
            swapVoices();
            swapCoolDown = 60;
        }
        if (swapCoolDown > 0) {
            swapCoolDown--;
        }
    }
    if (error) {
        *error = real_error;
    }

    return result;
}

DECL_FUNCTION(void, WPADRead, WPADChan chan, WPADStatusProController *data) {
    real_WPADRead(chan, data);

    if (gEnabled && swapCoolDown == 0 && OSIsHomeButtonMenuEnabled()) {
        if (data[0].err == 0) {
            if (data[0].extensionType != 0xFF) {
                bool doSwapWPAD = false;
                if (data[0].extensionType == WPAD_EXT_CORE || data[0].extensionType == WPAD_EXT_NUNCHUK) {
                    auto buttonCombo = remapVPADtoWiimote(gButtonCombo);
                    // button data is in the first 2 bytes for wiimotes
                    if (buttonCombo != 0 && ((uint16_t *) data)[0] == buttonCombo) {
                        doSwapWPAD = !doSwapWPAD;
                    }
                } else if (data[0].extensionType == WPAD_EXT_CLASSIC) {
                    auto buttonCombo = remapVPADtoClassic(gButtonCombo);
                    if (buttonCombo != 0 && (((uint32_t *) data)[10] & 0xFFFF) == buttonCombo) {
                        doSwapWPAD = !doSwapWPAD;
                    }
                } else if (data[0].extensionType == WPAD_EXT_PRO_CONTROLLER) {
                    auto buttonCombo = remapVPADtoPro(gButtonCombo);
                    if (buttonCombo != 0 && data[0].buttons == buttonCombo) {
                        doSwapWPAD = !doSwapWPAD;
                    }
                }
                if (doSwapWPAD) {
                    doSwap = !doSwap;
                    swapVoices();
                    swapCoolDown = 60;
                }
            }
        }
    }
}


WUPS_MUST_REPLACE(GX2CopyColorBufferToScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyColorBufferToScanBuffer);
WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);
WUPS_MUST_REPLACE(WPADRead, WUPS_LOADER_LIBRARY_PADSCORE, WPADRead);
WUPS_MUST_REPLACE(AXAcquireVoiceExOld, WUPS_LOADER_LIBRARY_SND_CORE, AXAcquireVoiceEx);
WUPS_MUST_REPLACE(AXFreeVoiceOld, WUPS_LOADER_LIBRARY_SND_CORE, AXFreeVoice);
WUPS_MUST_REPLACE(AXSetVoiceDeviceMixOld, WUPS_LOADER_LIBRARY_SND_CORE, AXSetVoiceDeviceMix);
// WUPS_MUST_REPLACE(AXSetDefaultMixerSelectOld, , WUPS_LOADER_LIBRARY_SND_CORE, AXSetDefaultMixerSelect),
WUPS_MUST_REPLACE(AXAcquireVoiceEx, WUPS_LOADER_LIBRARY_SNDCORE2, AXAcquireVoiceEx);
WUPS_MUST_REPLACE(AXFreeVoice, WUPS_LOADER_LIBRARY_SNDCORE2, AXFreeVoice);
WUPS_MUST_REPLACE(AXSetVoiceDeviceMix, WUPS_LOADER_LIBRARY_SNDCORE2, AXSetVoiceDeviceMix);
