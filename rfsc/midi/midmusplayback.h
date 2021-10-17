// This is PROPRIETARY CODE, do not modify, reuse, or share.
// All Rights Reserved.
// Reading this code for personal education and curiosity is ENCOURAGED!
// See LICENSE.md for details

#ifndef RFS2_MIDMUSPLAYBACK_H_
#define RFS2_MIDMUSPLAYBACK_H_

#include "compileconfig.h"

#include <stdint.h>

typedef struct midmussong midmussong;
typedef struct midmusplayback midmusplayback;

#define MIDI_DEFAULT_SF2 "rfslua/sf2/GeneralUserGSv1.471.sf2"


midmusplayback *midmusplayback_Create(midmussong *song);

void midmusplayback_Destroy(midmusplayback *playback);

#endif  // RFS2_MIDMUSPLAYBACK_H_