#include "utils/voice_swapper.h"
#include "utils/logger.h"

void VoiceSwapper_acquireVoice(AXVoice *voice) {
    for (auto &gVoiceInfo : gVoiceInfos) {
        if (gVoiceInfo.voice == nullptr) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] acquireVoice in slot %d for %08X!\n", i, voice);
            gVoiceInfo.voice = voice;
            break;
        }
    }
}

void VoiceSwapper_freeVoice(AXVoice *voice) {
    for (auto &gVoiceInfo : gVoiceInfos) {
        if (gVoiceInfo.voice == voice) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] freeVoice in slot %d for %08X!\n", i, voice);
            gVoiceInfo.voice = nullptr;
            break;
        }
    }
}

void VoiceSwapper_setMix(AXVoice *voice, AXDeviceType device, AXVoiceDeviceMixData *mix) {
    for (auto &gVoiceInfo : gVoiceInfos) {
        if (gVoiceInfo.voice == voice) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] setMix in slot %d for %08X!\n", i, voice);
            if (device == AX_DEVICE_TYPE_TV) {
                memcpy(gVoiceInfo.mixTV, mix, sizeof(gVoiceInfo.mixTV));
            } else if (device == AX_DEVICE_TYPE_DRC) {
                memcpy(gVoiceInfo.mixDRC, mix, sizeof(gVoiceInfo.mixDRC));
            }
            break;
        }
    }
}

void VoiceSwapper_swapAll() {
    for (auto &gVoiceInfo : gVoiceInfos) {
        memset(gVoiceInfos, 0, sizeof(gVoiceInfos));
        if (gVoiceInfo.voice == nullptr)
            continue;
        DEBUG_FUNCTION_LINE("[VoiceSwapper] swapping slot %d, voice %08X!\n", i, gVoiceInfos[i].voice);
        AXVoiceDeviceMixData buffer[24];
        memcpy(buffer, gVoiceInfo.mixTV, sizeof(gVoiceInfo.mixTV));
        memcpy(gVoiceInfo.mixTV, gVoiceInfo.mixDRC, sizeof(gVoiceInfo.mixTV));
        memcpy(gVoiceInfo.mixDRC, buffer, sizeof(gVoiceInfo.mixTV));
    }
}
