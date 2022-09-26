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
#pragma once

#define VOICE_INFO_MAX 100

#include <cstdint>
#include <sndcore2/voice.h>

typedef struct _VoiceInfo {
    AXVoice *voice;                  /**< Pointer to the voice */
    AXVoiceDeviceMixData mixTV[24];  /**< Mix to the TV */
    AXVoiceDeviceMixData mixDRC[24]; /**< Mix of the DRC */
} VoiceInfo;
