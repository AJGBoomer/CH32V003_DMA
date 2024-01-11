#include "debug.h"
#include "main.h"
#include "patterns.h"

// Patterns are in reverse. Pat[0] is the rear ...

// peakPat[] a worm with green tail light and red head light ;-)
const u16 peakPatSz = 16;
const uint8_t peakPat[] =
		  { 0,16,0,  // Red tail light
			0,0,4,
			0,0,8,
			0,0,16,
			0,0,32,
			0,0,64,
			0,0,128,
			0,0,255,
			0,0,255,
			0,0,128,
			0,0,64,
			0,0,32,
			0,0,16,
			0,0,8,
			0,0,4,
			255,0,0 };  // Green head light

const u16 testPatSz = 16;
const uint8_t testPat[] =
		  { 128,129,130,
			131,132,133,
			134,135,136,
			137,138,139,
			0,127,0,
			127,0,0,
			0,127,0,
			0,0,127,
			0,0,127,
			0,127,0,
			127,0,0,
			0,127,0,
			0,0,127,
			0,127,0,
			127,0,0,
			255,255,255 };

