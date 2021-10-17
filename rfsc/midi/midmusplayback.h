// This is PROPRIETARY CODE, do not modify, reuse, or share.
// All Rights Reserved.
// Reading this code for personal education and curiosity is ENCOURAGED!
// See LICENSE.md for details

#ifndef RFS2_MIDMUSPLAYBACK_H_
#define RFS2_MIDMUSPLAYBACK_H_

#include "compileconfig.h"

#include <stdint.h>

typedef struct h3daudiodevice h3daudiodevice;
typedef struct midmussong midmussong;
typedef struct midmusplayback midmusplayback;

#define MIDI_DEFAULT_SF2 "rfslua/sf2/GeneralUserGSv1.471.sf2"


midmusplayback *midmusplayback_Create(midmussong *song);

void midmusplayback_Destroy(midmusplayback *playback);

uint64_t midmusplayback_StartAndAddToMixer(
    h3daudiodevice *dev,
    midmusplayback *playback, double volume,
    int loop,
    int destroyafterwards);

int _midmusplayback_ReadSamplesCallback(  // internal
    midmusplayback *playback,
    char *writebuf, int writebytes, int *haderror);

int _midmusplayback_StartSamplesCallback(  // internal
    void *userdata);

void _callback_CloseAfterReadingSamplesCallback(  // internal
    void *userdata);

#endif  // RFS2_MIDMUSPLAYBACK_H_
