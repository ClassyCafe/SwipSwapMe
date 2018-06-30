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
#ifndef _VOICE_SWAPPER_H_
#define _VOICE_SWAPPER_H_
#define VOICE_SWAP_LOG 0
#include "voice_info.h"
#include "common/c_retain_vars.h"

#include <stdio.h>
#include <string.h>

void VoiceSwapper_acquireVoice(void * voice);

void VoiceSwapper_freeVoice(void * voice);

void VoiceSwapper_setMix(void * voice,uint32_t device, void* mix);

void VoiceSwapper_swapAll();

#endif //_VOICE_SWAPPER_H_
