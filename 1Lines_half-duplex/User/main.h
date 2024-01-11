#ifndef __MAIN_H
#define __MAIN_H

#define DMABUFNLEDS  64			// 64 leds. DMABUF needs 12 bytes/led to encode bit stream
#define DMABUFSPERSTRING 3

#define BITSPERRBGLED 24
#define BITSPERBYTE 2
#define BYTESPERCOLOR (8/2)
#define BYTESPERLED (BYTESPERCOLOR * 3)

#define DMABUFNBYTES  (DMABUFNLEDS * BYTESPERLED)

#define zero 0x8 //1000
#define one  0xE //1110
#define Reset 0,0,0,0
#define ColorOn (one<<4 | one),(one<<4 | one),(one<<4 | one),(one<<4 | one)
#define ColorOff (zero<<4 | zero),(zero<<4 | zero),(zero<<4 | zero),(zero<<4 | zero)
#define RESETCNTS 2

#pragma pack(push)
#pragma pack(1)
typedef struct grb{
	uint8_t g;
	uint8_t r;
	uint8_t b;
}GRB;
typedef struct COLORBITS{
	u8 grn[4];
	u8 red[4];
	u8 blue[4];
}COLORBITS;
#pragma pack(pop)

typedef enum {green,red,blue,yellow,magenta,white,black} ColorWheel;

#endif
