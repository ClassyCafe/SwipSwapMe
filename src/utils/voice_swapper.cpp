#include "utils/voice_swapper.h"
#include "utils/logger.h"
#include "utils/voice_info.h"

void VoiceSwapper_acquireVoice(AXVoice *voice) {
    for (int32_t i = 0; i < VOICE_INFO_MAX; i++) {
        if (gVoiceInfos[i].voice == nullptr) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] acquireVoice in slot %d for %08X!\n", i, voice);
            gVoiceInfos[i].voice = voice;
            break;
        }
    }
}

void VoiceSwapper_freeVoice(AXVoice *voice) {
    for (int32_t i = 0; i < VOICE_INFO_MAX; i++) {
        if (gVoiceInfos[i].voice == voice) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] freeVoice in slot %d for %08X!\n", i, voice);
            gVoiceInfos[i].voice = nullptr;
            break;
        }
    }
}

void VoiceSwapper_setMix(AXVoice *voice, AXDeviceType device, AXVoiceDeviceMixData *mix) {
    for (int32_t i = 0; i < VOICE_INFO_MAX; i++) {
        if (gVoiceInfos[i].voice == voice) {
            DEBUG_FUNCTION_LINE("[VoiceSwapper] setMix in slot %d for %08X!\n", i, voice);
            if (device == AX_DEVICE_TYPE_TV) {
                memcpy(gVoiceInfos[i].mixTV, mix, sizeof(gVoiceInfos[i].mixTV));
            } else if (device == AX_DEVICE_TYPE_DRC) {
                memcpy(gVoiceInfos[i].mixDRC, mix, sizeof(gVoiceInfos[i].mixDRC));
            }
            break;
        }
    }
}

void VoiceSwapper_swapAll() {
    for (int32_t i = 0; i < VOICE_INFO_MAX; i++) {
        memset(gVoiceInfos, 0, sizeof(gVoiceInfos));
        if (gVoiceInfos[i].voice == nullptr)
            continue;
        DEBUG_FUNCTION_LINE("[VoiceSwapper] swapping slot %d, voice %08X!\n", i, gVoiceInfos[i].voice);
        AXVoiceDeviceMixData buffer[24];
        memcpy(buffer, gVoiceInfos[i].mixTV, sizeof(gVoiceInfos[i].mixTV));
        memcpy(gVoiceInfos[i].mixTV, gVoiceInfos[i].mixDRC, sizeof(gVoiceInfos[i].mixTV));
        memcpy(gVoiceInfos[i].mixDRC, buffer, sizeof(gVoiceInfos[i].mixTV));
    }
}
