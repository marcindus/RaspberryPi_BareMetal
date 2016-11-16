/*-------------------------------------------------------------------------
Copyright (c) 2016 AZO typesylph@gmail.com
  referred to David Welch https://github.com/dwelch67

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------*/

#include "rp_baremetal.h"

//-------------------------------------------------------------------------
void c_irq_handler(void)
{
}

//-------------------------------------------------------------------------
typedef struct azo_pcmdata {
	uint32_t size;
	uint32_t channels;
	uint32_t samplerate;
	uint32_t bitswidth;
	uint32_t loopcount;
	uint32_t endless;
	void* data;
} azo_pcmdata_t;

//-------------------------------------------------------------------------
uint16_t azo_sin_1k_48khz16bit[48] = {
	0x0000, 0x10B5, 0x2120, 0x30FB, 0x3FFF, 0x4DEB, 0x5A82, 0x658C, 0x6ED9, 0x7641, 0x7BA3, 0x7EE7, 0x7FFF, 0x7EE7, 0x7BA3, 0x7641, 
	0x6ED9, 0x658C, 0x5A82, 0x4DEB, 0x4000, 0x30FB, 0x2120, 0x10B5, 0x0000, 0xEF4B, 0xDEE0, 0xCF05, 0xC001, 0xB215, 0xA57E, 0x9A74, 
	0x9127, 0x89BF, 0x845D, 0x8119, 0x8000, 0x8119, 0x845D, 0x89BF, 0x9127, 0x9A74, 0xA57E, 0xB215, 0xC000, 0xCF05, 0xDEE0, 0xEF4B,
};

uint16_t azo_sin_1k_384khz16bit[384] = {
	0x0000, 0x0218, 0x0430, 0x0647, 0x085F, 0x0A75, 0x0C8B, 0x0EA0, 0x10B5, 0x12C8, 0x14D9, 0x16EA, 0x18F8, 0x1B05, 0x1D10, 0x1F19,
	0x2120, 0x2325, 0x2528, 0x2727, 0x2924, 0x2B1F, 0x2D16, 0x2F0A, 0x30FB, 0x32E9, 0x34D3, 0x36BA, 0x389C, 0x3A7B, 0x3C56, 0x3E2D,
	0x3FFF, 0x41CE, 0x4397, 0x455C, 0x471C, 0x48D8, 0x4A8E, 0x4C3F, 0x4DEB, 0x4F92, 0x5133, 0x52CF, 0x5465, 0x55F5, 0x577F, 0x5904,
	0x5A82, 0x5BFA, 0x5D6C, 0x5ED7, 0x603C, 0x619A, 0x62F2, 0x6442, 0x658C, 0x66CF, 0x680B, 0x6940, 0x6A6D, 0x6B93, 0x6CB2, 0x6DCA,
	0x6ED9, 0x6FE2, 0x70E2, 0x71DB, 0x72CC, 0x73B5, 0x7497, 0x7570, 0x7641, 0x770A, 0x77CB, 0x7884, 0x7935, 0x79DD, 0x7A7D, 0x7B14,
	0x7BA3, 0x7C29, 0x7CA8, 0x7D1D, 0x7D8A, 0x7DEE, 0x7E4A, 0x7E9D, 0x7EE7, 0x7F29, 0x7F62, 0x7F92, 0x7FB9, 0x7FD8, 0x7FEE, 0x7FFB,
	0x7FFF, 0x7FFB, 0x7FEE, 0x7FD8, 0x7FB9, 0x7F92, 0x7F62, 0x7F29, 0x7EE7, 0x7E9D, 0x7E4A, 0x7DEE, 0x7D8A, 0x7D1D, 0x7CA8, 0x7C29,
	0x7BA3, 0x7B14, 0x7A7D, 0x79DD, 0x7935, 0x7884, 0x77CB, 0x770A, 0x7641, 0x7570, 0x7497, 0x73B5, 0x72CC, 0x71DB, 0x70E2, 0x6FE2,
	0x6ED9, 0x6DCA, 0x6CB2, 0x6B93, 0x6A6D, 0x6940, 0x680B, 0x66CF, 0x658C, 0x6442, 0x62F2, 0x619A, 0x603C, 0x5ED7, 0x5D6C, 0x5BFA,
	0x5A82, 0x5904, 0x577F, 0x55F5, 0x5465, 0x52CF, 0x5133, 0x4F92, 0x4DEB, 0x4C3F, 0x4A8E, 0x48D8, 0x471C, 0x455C, 0x4397, 0x41CE,
	0x4000, 0x3E2D, 0x3C56, 0x3A7B, 0x389C, 0x36BA, 0x34D3, 0x32E9, 0x30FB, 0x2F0A, 0x2D16, 0x2B1F, 0x2924, 0x2727, 0x2528, 0x2325,
	0x2120, 0x1F19, 0x1D10, 0x1B05, 0x18F8, 0x16EA, 0x14D9, 0x12C8, 0x10B5, 0x0EA0, 0x0C8B, 0x0A75, 0x085F, 0x0647, 0x0430, 0x0218,
	0x0000, 0xFDE8, 0xFBD0, 0xF9B9, 0xF7A1, 0xF58B, 0xF375, 0xF160, 0xEF4B, 0xED38, 0xEB27, 0xE916, 0xE708, 0xE4FB, 0xE2F0, 0xE0E7,
	0xDEE0, 0xDCDB, 0xDAD8, 0xD8D9, 0xD6DC, 0xD4E1, 0xD2EA, 0xD0F6, 0xCF05, 0xCD17, 0xCB2D, 0xC946, 0xC764, 0xC585, 0xC3AA, 0xC1D3,
	0xC001, 0xBE32, 0xBC69, 0xBAA4, 0xB8E4, 0xB728, 0xB572, 0xB3C1, 0xB215, 0xB06E, 0xAECD, 0xAD31, 0xAB9B, 0xAA0B, 0xA881, 0xA6FC,
	0xA57E, 0xA406, 0xA294, 0xA129, 0x9FC4, 0x9E66, 0x9D0E, 0x9BBE, 0x9A74, 0x9931, 0x97F5, 0x96C0, 0x9593, 0x946D, 0x934E, 0x9236,
	0x9127, 0x901E, 0x8F1E, 0x8E25, 0x8D34, 0x8C4B, 0x8B69, 0x8A90, 0x89BF, 0x88F6, 0x8835, 0x877C, 0x86CB, 0x8623, 0x8583, 0x84EC,
	0x845D, 0x83D7, 0x8358, 0x82E3, 0x8276, 0x8212, 0x81B6, 0x8163, 0x8119, 0x80D7, 0x809E, 0x806E, 0x8047, 0x8028, 0x8012, 0x8005,
	0x8000, 0x8005, 0x8012, 0x8028, 0x8047, 0x806E, 0x809E, 0x80D7, 0x8119, 0x8163, 0x81B6, 0x8212, 0x8276, 0x82E3, 0x8358, 0x83D7,
	0x845D, 0x84EC, 0x8583, 0x8623, 0x86CB, 0x877C, 0x8835, 0x88F6, 0x89BF, 0x8A90, 0x8B69, 0x8C4B, 0x8D34, 0x8E25, 0x8F1E, 0x901E,
	0x9127, 0x9236, 0x934E, 0x946D, 0x9593, 0x96C0, 0x97F5, 0x9931, 0x9A74, 0x9BBE, 0x9D0E, 0x9E66, 0x9FC4, 0xA129, 0xA294, 0xA406,
	0xA57E, 0xA6FC, 0xA881, 0xAA0B, 0xAB9B, 0xAD31, 0xAECD, 0xB06E, 0xB215, 0xB3C1, 0xB572, 0xB728, 0xB8E4, 0xBAA4, 0xBC69, 0xBE32,
	0xC000, 0xC1D3, 0xC3AA, 0xC585, 0xC764, 0xC946, 0xCB2D, 0xCD17, 0xCF05, 0xD0F6, 0xD2EA, 0xD4E1, 0xD6DC, 0xD8D9, 0xDAD8, 0xDCDB,
	0xDEE0, 0xE0E7, 0xE2F0, 0xE4FB, 0xE708, 0xE916, 0xEB27, 0xED38, 0xEF4B, 0xF160, 0xF375, 0xF58B, 0xF7A1, 0xF9B9, 0xFBD0, 0xFDE8,
};

uint8_t azo_sin_1k_96khz24bit[288] = {
	0x00, 0x00, 0x00, 0x21, 0x5F, 0x08, 0x15, 0xB5, 0x10, 0xB8, 0xF8, 0x18, 0xFB, 0x20, 0x21,
	0xED, 0x24, 0x29, 0xC5, 0xFB, 0x30, 0xEA, 0x9C, 0x38, 0xFF, 0xFF, 0x3F, 0xEC, 0x1C, 0x47,
	0xE4, 0xEB, 0x4D, 0x71, 0x65, 0x54, 0x79, 0x82, 0x5A, 0x49, 0x3C, 0x60, 0x9A, 0x8C, 0x65,
	0x98, 0x6D, 0x6A, 0xEB, 0xD9, 0x6E, 0xB9, 0xCC, 0x72, 0xAF, 0x41, 0x76, 0x01, 0x35, 0x79,
	0x75, 0xA3, 0x7B, 0x5F, 0x8A, 0x7D, 0xAA, 0xE7, 0x7E, 0xD7, 0xB9, 0x7F, 0xFF, 0xFF, 0x7F,
	0xD7, 0xB9, 0x7F, 0xAA, 0xE7, 0x7E, 0x5F, 0x8A, 0x7D, 0x75, 0xA3, 0x7B, 0x01, 0x35, 0x79,
	0xAF, 0x41, 0x76, 0xB9, 0xCC, 0x72, 0xEB, 0xD9, 0x6E, 0x98, 0x6D, 0x6A, 0x9A, 0x8C, 0x65,
	0x49, 0x3C, 0x60, 0x79, 0x82, 0x5A, 0x71, 0x65, 0x54, 0xE4, 0xEB, 0x4D, 0xEC, 0x1C, 0x47,
	0x00, 0x00, 0x40, 0xEA, 0x9C, 0x38, 0xC5, 0xFB, 0x30, 0xED, 0x24, 0x29, 0xFB, 0x20, 0x21,
	0xB8, 0xF8, 0x18, 0x15, 0xB5, 0x10, 0x21, 0x5F, 0x08, 0x00, 0x00, 0x00, 0xDF, 0xA0, 0xF7,
	0xEB, 0x4A, 0xEF, 0x48, 0x07, 0xE7, 0x05, 0xDF, 0xDE, 0x13, 0xDB, 0xD6, 0x3B, 0x04, 0xCF,
	0x16, 0x63, 0xC7, 0x01, 0x00, 0xC0, 0x14, 0xE3, 0xB8, 0x1C, 0x14, 0xB2, 0x8F, 0x9A, 0xAB,
	0x87, 0x7D, 0xA5, 0xB7, 0xC3, 0x9F, 0x66, 0x73, 0x9A, 0x68, 0x92, 0x95, 0x15, 0x26, 0x91,
	0x47, 0x33, 0x8D, 0x51, 0xBE, 0x89, 0xFF, 0xCA, 0x86, 0x8B, 0x5C, 0x84, 0xA1, 0x75, 0x82,
	0x56, 0x18, 0x81, 0x29, 0x46, 0x80, 0x00, 0x00, 0x80, 0x29, 0x46, 0x80, 0x56, 0x18, 0x81,
	0xA1, 0x75, 0x82, 0x8B, 0x5C, 0x84, 0xFF, 0xCA, 0x86, 0x51, 0xBE, 0x89, 0x47, 0x33, 0x8D,
	0x15, 0x26, 0x91, 0x68, 0x92, 0x95, 0x66, 0x73, 0x9A, 0xB7, 0xC3, 0x9F, 0x87, 0x7D, 0xA5,
	0x8F, 0x9A, 0xAB, 0x1C, 0x14, 0xB2, 0x14, 0xE3, 0xB8, 0x00, 0x00, 0xC0, 0x16, 0x63, 0xC7,
	0x3B, 0x04, 0xCF, 0x13, 0xDB, 0xD6, 0x05, 0xDF, 0xDE, 0x48, 0x07, 0xE7, 0xEB, 0x4A, 0xEF,
	0xDF, 0xA0, 0xF7,
};

uint8_t azo_sin_1k_192khz24bit[576] = {
	0x00, 0x00, 0x00, 0x23, 0x30, 0x04, 0x21, 0x5F, 0x08, 0xD3, 0x8B, 0x0C, 0x15, 0xB5, 0x10,
	0xC2, 0xD9, 0x14, 0xB8, 0xF8, 0x18, 0xD5, 0x10, 0x1D, 0xFB, 0x20, 0x21, 0x0C, 0x28, 0x25,
	0xED, 0x24, 0x29, 0x87, 0x16, 0x2D, 0xC5, 0xFB, 0x30, 0x95, 0xD3, 0x34, 0xEA, 0x9C, 0x38,
	0xBA, 0x56, 0x3C, 0xFF, 0xFF, 0x3F, 0xBA, 0x97, 0x43, 0xEC, 0x1C, 0x47, 0xA1, 0x8E, 0x4A,
	0xE4, 0xEB, 0x4D, 0xCC, 0x33, 0x51, 0x71, 0x65, 0x54, 0xF3, 0x7F, 0x57, 0x79, 0x82, 0x5A,
	0x2F, 0x6C, 0x5D, 0x49, 0x3C, 0x60, 0x01, 0xF2, 0x62, 0x9A, 0x8C, 0x65, 0x5C, 0x0B, 0x68,
	0x98, 0x6D, 0x6A, 0xA8, 0xB2, 0x6C, 0xEB, 0xD9, 0x6E, 0xCB, 0xE2, 0x70, 0xB9, 0xCC, 0x72,
	0x2F, 0x97, 0x74, 0xAF, 0x41, 0x76, 0xC3, 0xCB, 0x77, 0x01, 0x35, 0x79, 0x05, 0x7D, 0x7A,
	0x75, 0xA3, 0x7B, 0x00, 0xA8, 0x7C, 0x5F, 0x8A, 0x7D, 0x54, 0x4A, 0x7E, 0xAA, 0xE7, 0x7E,
	0x36, 0x62, 0x7F, 0xD7, 0xB9, 0x7F, 0x74, 0xEE, 0x7F, 0xFF, 0xFF, 0x7F, 0x74, 0xEE, 0x7F,
	0xD7, 0xB9, 0x7F, 0x36, 0x62, 0x7F, 0xAA, 0xE7, 0x7E, 0x54, 0x4A, 0x7E, 0x5F, 0x8A, 0x7D,
	0x00, 0xA8, 0x7C, 0x75, 0xA3, 0x7B, 0x05, 0x7D, 0x7A, 0x01, 0x35, 0x79, 0xC3, 0xCB, 0x77,
	0xAF, 0x41, 0x76, 0x2F, 0x97, 0x74, 0xB9, 0xCC, 0x72, 0xCB, 0xE2, 0x70, 0xEB, 0xD9, 0x6E,
	0xA8, 0xB2, 0x6C, 0x98, 0x6D, 0x6A, 0x5C, 0x0B, 0x68, 0x9A, 0x8C, 0x65, 0x01, 0xF2, 0x62,
	0x49, 0x3C, 0x60, 0x2F, 0x6C, 0x5D, 0x79, 0x82, 0x5A, 0xF3, 0x7F, 0x57, 0x71, 0x65, 0x54,
	0xCC, 0x33, 0x51, 0xE4, 0xEB, 0x4D, 0xA1, 0x8E, 0x4A, 0xEC, 0x1C, 0x47, 0xBA, 0x97, 0x43,
	0x00, 0x00, 0x40, 0xBA, 0x56, 0x3C, 0xEA, 0x9C, 0x38, 0x95, 0xD3, 0x34, 0xC5, 0xFB, 0x30,
	0x87, 0x16, 0x2D, 0xED, 0x24, 0x29, 0x0C, 0x28, 0x25, 0xFB, 0x20, 0x21, 0xD5, 0x10, 0x1D,
	0xB8, 0xF8, 0x18, 0xC2, 0xD9, 0x14, 0x15, 0xB5, 0x10, 0xD3, 0x8B, 0x0C, 0x21, 0x5F, 0x08,
	0x23, 0x30, 0x04, 0x00, 0x00, 0x00, 0xDD, 0xCF, 0xFB, 0xDF, 0xA0, 0xF7, 0x2D, 0x74, 0xF3,
	0xEB, 0x4A, 0xEF, 0x3E, 0x26, 0xEB, 0x48, 0x07, 0xE7, 0x2B, 0xEF, 0xE2, 0x05, 0xDF, 0xDE,
	0xF4, 0xD7, 0xDA, 0x13, 0xDB, 0xD6, 0x79, 0xE9, 0xD2, 0x3B, 0x04, 0xCF, 0x6B, 0x2C, 0xCB,
	0x16, 0x63, 0xC7, 0x46, 0xA9, 0xC3, 0x01, 0x00, 0xC0, 0x46, 0x68, 0xBC, 0x14, 0xE3, 0xB8,
	0x5F, 0x71, 0xB5, 0x1C, 0x14, 0xB2, 0x34, 0xCC, 0xAE, 0x8F, 0x9A, 0xAB, 0x0D, 0x80, 0xA8,
	0x87, 0x7D, 0xA5, 0xD1, 0x93, 0xA2, 0xB7, 0xC3, 0x9F, 0xFF, 0x0D, 0x9D, 0x66, 0x73, 0x9A,
	0xA4, 0xF4, 0x97, 0x68, 0x92, 0x95, 0x58, 0x4D, 0x93, 0x15, 0x26, 0x91, 0x35, 0x1D, 0x8F,
	0x47, 0x33, 0x8D, 0xD1, 0x68, 0x8B, 0x51, 0xBE, 0x89, 0x3D, 0x34, 0x88, 0xFF, 0xCA, 0x86,
	0xFB, 0x82, 0x85, 0x8B, 0x5C, 0x84, 0x00, 0x58, 0x83, 0xA1, 0x75, 0x82, 0xAC, 0xB5, 0x81,
	0x56, 0x18, 0x81, 0xCA, 0x9D, 0x80, 0x29, 0x46, 0x80, 0x8C, 0x11, 0x80, 0x00, 0x00, 0x80,
	0x8C, 0x11, 0x80, 0x29, 0x46, 0x80, 0xCA, 0x9D, 0x80, 0x56, 0x18, 0x81, 0xAC, 0xB5, 0x81,
	0xA1, 0x75, 0x82, 0x00, 0x58, 0x83, 0x8B, 0x5C, 0x84, 0xFB, 0x82, 0x85, 0xFF, 0xCA, 0x86,
	0x3D, 0x34, 0x88, 0x51, 0xBE, 0x89, 0xD1, 0x68, 0x8B, 0x47, 0x33, 0x8D, 0x35, 0x1D, 0x8F,
	0x15, 0x26, 0x91, 0x58, 0x4D, 0x93, 0x68, 0x92, 0x95, 0xA4, 0xF4, 0x97, 0x66, 0x73, 0x9A,
	0xFF, 0x0D, 0x9D, 0xB7, 0xC3, 0x9F, 0xD1, 0x93, 0xA2, 0x87, 0x7D, 0xA5, 0x0D, 0x80, 0xA8,
	0x8F, 0x9A, 0xAB, 0x34, 0xCC, 0xAE, 0x1C, 0x14, 0xB2, 0x5F, 0x71, 0xB5, 0x14, 0xE3, 0xB8,
	0x46, 0x68, 0xBC, 0x00, 0x00, 0xC0, 0x46, 0xA9, 0xC3, 0x16, 0x63, 0xC7, 0x6B, 0x2C, 0xCB,
	0x3B, 0x04, 0xCF, 0x79, 0xE9, 0xD2, 0x13, 0xDB, 0xD6, 0xF4, 0xD7, 0xDA, 0x05, 0xDF, 0xDE,
	0x2B, 0xEF, 0xE2, 0x48, 0x07, 0xE7, 0x3E, 0x26, 0xEB, 0xEB, 0x4A, 0xEF, 0x2D, 0x74, 0xF3,
	0xDF, 0xA0, 0xF7, 0xDD, 0xCF, 0xFB,
};

uint32_t azo_sin_1k_48khz32bit[48] = {
	0x00000000, 0x10B5150F, 0x2120FB83, 0x30FBC54D,
	0x3FFFFFFF, 0x4DEBE4FE, 0x5A827999, 0x658C9A2D,
	0x6ED9EBA1, 0x7641AF3C, 0x7BA3751D, 0x7EE7AA4B,
	0x7FFFFFFF, 0x7EE7AA4B, 0x7BA3751D, 0x7641AF3C,
	0x6ED9EBA1, 0x658C9A2D, 0x5A827999, 0x4DEBE4FE,
	0x40000000, 0x30FBC54D, 0x2120FB83, 0x10B5150F,
	0x00000000, 0xEF4AEAF1, 0xDEDF047D, 0xCF043AB3,
	0xC0000001, 0xB2141B02, 0xA57D8667, 0x9A7365D3,
	0x9126145F, 0x89BE50C4, 0x845C8AE3, 0x811855B5,
	0x80000000, 0x811855B5, 0x845C8AE3, 0x89BE50C4,
	0x9126145F, 0x9A7365D3, 0xA57D8667, 0xB2141B02,
	0xC0000000, 0xCF043AB3, 0xDEDF047D, 0xEF4AEAF1,
};

uint32_t azo_sin_1k_96khz32bit[96] = {
	0x00000000, 0x085F2136, 0x10B5150F, 0x18F8B83C,
	0x2120FB83, 0x2924EDAB, 0x30FBC54D, 0x389CEA71,
	0x3FFFFFFF, 0x471CECE6, 0x4DEBE4FE, 0x54657194,
	0x5A827999, 0x603C496C, 0x658C9A2D, 0x6A6D98A4,
	0x6ED9EBA1, 0x72CCB9DA, 0x7641AF3C, 0x793501A8,
	0x7BA3751D, 0x7D8A5F3F, 0x7EE7AA4B, 0x7FB9D758,
	0x7FFFFFFF, 0x7FB9D758, 0x7EE7AA4B, 0x7D8A5F3F,
	0x7BA3751D, 0x793501A8, 0x7641AF3C, 0x72CCB9DA,
	0x6ED9EBA1, 0x6A6D98A4, 0x658C9A2D, 0x603C496C,
	0x5A827999, 0x54657194, 0x4DEBE4FE, 0x471CECE6,
	0x40000000, 0x389CEA71, 0x30FBC54D, 0x2924EDAB,
	0x2120FB83, 0x18F8B83C, 0x10B5150F, 0x085F2136,
	0x00000000, 0xF7A0DECA, 0xEF4AEAF1, 0xE70747C4,
	0xDEDF047D, 0xD6DB1255, 0xCF043AB3, 0xC763158F,
	0xC0000001, 0xB8E3131A, 0xB2141B02, 0xAB9A8E6C,
	0xA57D8667, 0x9FC3B694, 0x9A7365D3, 0x9592675C,
	0x9126145F, 0x8D334626, 0x89BE50C4, 0x86CAFE58,
	0x845C8AE3, 0x8275A0C1, 0x811855B5, 0x804628A8,
	0x80000000, 0x804628A8, 0x811855B5, 0x8275A0C1,
	0x845C8AE3, 0x86CAFE58, 0x89BE50C4, 0x8D334626,
	0x9126145F, 0x9592675C, 0x9A7365D3, 0x9FC3B694,
	0xA57D8667, 0xAB9A8E6C, 0xB2141B02, 0xB8E3131A,
	0xC0000000, 0xC763158F, 0xCF043AB3, 0xD6DB1255,
	0xDEDF047D, 0xE70747C4, 0xEF4AEAF1, 0xF7A0DECA,
};

uint32_t azo_sin_1k_384khz32bit[384] = {
	0x00000000, 0x02182427, 0x0430238F, 0x0647D97C,
	0x085F2136, 0x0A75D60E, 0x0C8BD35E, 0x0EA0F48C,
	0x10B5150F, 0x12C8106E, 0x14D9C245, 0x16EA0646,
	0x18F8B83C, 0x1B05B40E, 0x1D10D5C1, 0x1F19F97B,
	0x2120FB83, 0x2325B847, 0x25280C5D, 0x2727D485,
	0x2924EDAB, 0x2B1F34EB, 0x2D168791, 0x2F0AC320,
	0x30FBC54D, 0x32E96C09, 0x34D3957D, 0x36BA2013,
	0x389CEA71, 0x3A7BD381, 0x3C56BA70, 0x3E2D7EB0,
	0x3FFFFFFF, 0x41CE1E64, 0x4397BA32, 0x455CB40C,
	0x471CECE6, 0x48D84609, 0x4A8EA111, 0x4C3FDFF3,
	0x4DEBE4FE, 0x4F9292DB, 0x5133CC94, 0x52CF758E,
	0x54657194, 0x55F5A4D2, 0x577FF3DA, 0x590443A6,
	0x5A827999, 0x5BFA7B81, 0x5D6C2F99, 0x5ED77C89,
	0x603C496C, 0x619A7DCD, 0x62F201AC, 0x6442BD7D,
	0x658C9A2D, 0x66CF811F, 0x680B5C33, 0x694015C2,
	0x6A6D98A4, 0x6B93D02E, 0x6CB2A836, 0x6DCA0D14,
	0x6ED9EBA1, 0x6FE2313B, 0x70E2CBC6, 0x71DBA9AA,
	0x72CCB9DA, 0x73B5EBD0, 0x74972F91, 0x757075AC,
	0x7641AF3C, 0x770ACDEC, 0x77CBC3F1, 0x78848413,
	0x793501A8, 0x79DD3098, 0x7A7D055B, 0x7B1474FD,
	0x7BA3751D, 0x7C29FBEE, 0x7CA80038, 0x7D1D7957,
	0x7D8A5F3F, 0x7DEEAA79, 0x7E4A5426, 0x7E9D55FC,
	0x7EE7AA4B, 0x7F294BFC, 0x7F62368F, 0x7F92661D,
	0x7FB9D758, 0x7FD8878D, 0x7FEE74A2, 0x7FFB9D15,
	0x7FFFFFFF, 0x7FFB9D15, 0x7FEE74A2, 0x7FD8878D,
	0x7FB9D758, 0x7F92661D, 0x7F62368F, 0x7F294BFC,
	0x7EE7AA4B, 0x7E9D55FC, 0x7E4A5426, 0x7DEEAA79,
	0x7D8A5F3F, 0x7D1D7957, 0x7CA80038, 0x7C29FBEE,
	0x7BA3751D, 0x7B1474FD, 0x7A7D055B, 0x79DD3098,
	0x793501A8, 0x78848413, 0x77CBC3F1, 0x770ACDEC,
	0x7641AF3C, 0x757075AC, 0x74972F91, 0x73B5EBD0,
	0x72CCB9DA, 0x71DBA9AA, 0x70E2CBC6, 0x6FE2313B,
	0x6ED9EBA1, 0x6DCA0D14, 0x6CB2A836, 0x6B93D02E,
	0x6A6D98A4, 0x694015C2, 0x680B5C33, 0x66CF811F,
	0x658C9A2D, 0x6442BD7D, 0x62F201AC, 0x619A7DCD,
	0x603C496C, 0x5ED77C89, 0x5D6C2F99, 0x5BFA7B81,
	0x5A827999, 0x590443A6, 0x577FF3DA, 0x55F5A4D2,
	0x54657194, 0x52CF758E, 0x5133CC94, 0x4F9292DB,
	0x4DEBE4FE, 0x4C3FDFF3, 0x4A8EA111, 0x48D84609,
	0x471CECE6, 0x455CB40C, 0x4397BA32, 0x41CE1E64,
	0x40000000, 0x3E2D7EB0, 0x3C56BA70, 0x3A7BD381,
	0x389CEA71, 0x36BA2013, 0x34D3957D, 0x32E96C09,
	0x30FBC54D, 0x2F0AC320, 0x2D168791, 0x2B1F34EB,
	0x2924EDAB, 0x2727D485, 0x25280C5D, 0x2325B847,
	0x2120FB83, 0x1F19F97B, 0x1D10D5C1, 0x1B05B40E,
	0x18F8B83C, 0x16EA0646, 0x14D9C245, 0x12C8106E,
	0x10B5150F, 0x0EA0F48C, 0x0C8BD35E, 0x0A75D60E,
	0x085F2136, 0x0647D97C, 0x0430238F, 0x02182427,
	0x00000000, 0xFDE7DBD9, 0xFBCFDC71, 0xF9B82684,
	0xF7A0DECA, 0xF58A29F2, 0xF3742CA2, 0xF15F0B74,
	0xEF4AEAF1, 0xED37EF92, 0xEB263DBB, 0xE915F9BA,
	0xE70747C4, 0xE4FA4BF2, 0xE2EF2A3F, 0xE0E60685,
	0xDEDF047D, 0xDCDA47B9, 0xDAD7F3A3, 0xD8D82B7B,
	0xD6DB1255, 0xD4E0CB15, 0xD2E9786F, 0xD0F53CE0,
	0xCF043AB3, 0xCD1693F7, 0xCB2C6A83, 0xC945DFED,
	0xC763158F, 0xC5842C7F, 0xC3A94590, 0xC1D28150,
	0xC0000001, 0xBE31E19C, 0xBC6845CE, 0xBAA34BF4,
	0xB8E3131A, 0xB727B9F7, 0xB5715EEF, 0xB3C0200D,
	0xB2141B02, 0xB06D6D25, 0xAECC336C, 0xAD308A72,
	0xAB9A8E6C, 0xAA0A5B2E, 0xA8800C26, 0xA6FBBC5A,
	0xA57D8667, 0xA405847F, 0xA293D067, 0xA1288377,
	0x9FC3B694, 0x9E658233, 0x9D0DFE54, 0x9BBD4283,
	0x9A7365D3, 0x99307EE1, 0x97F4A3CD, 0x96BFEA3E,
	0x9592675C, 0x946C2FD2, 0x934D57CA, 0x9235F2EC,
	0x9126145F, 0x901DCEC5, 0x8F1D343A, 0x8E245656,
	0x8D334626, 0x8C4A1430, 0x8B68D06F, 0x8A8F8A54,
	0x89BE50C4, 0x88F53214, 0x88343C0F, 0x877B7BED,
	0x86CAFE58, 0x8622CF68, 0x8582FAA5, 0x84EB8B03,
	0x845C8AE3, 0x83D60412, 0x8357FFC8, 0x82E286A9,
	0x8275A0C1, 0x82115587, 0x81B5ABDA, 0x8162AA04,
	0x811855B5, 0x80D6B404, 0x809DC971, 0x806D99E3,
	0x804628A8, 0x80277873, 0x80118B5E, 0x800462EB,
	0x80000000, 0x800462EB, 0x80118B5E, 0x80277873,
	0x804628A8, 0x806D99E3, 0x809DC971, 0x80D6B404,
	0x811855B5, 0x8162AA04, 0x81B5ABDA, 0x82115587,
	0x8275A0C1, 0x82E286A9, 0x8357FFC8, 0x83D60412,
	0x845C8AE3, 0x84EB8B03, 0x8582FAA5, 0x8622CF68,
	0x86CAFE58, 0x877B7BED, 0x88343C0F, 0x88F53214,
	0x89BE50C4, 0x8A8F8A54, 0x8B68D06F, 0x8C4A1430,
	0x8D334626, 0x8E245656, 0x8F1D343A, 0x901DCEC5,
	0x9126145F, 0x9235F2EC, 0x934D57CA, 0x946C2FD2,
	0x9592675C, 0x96BFEA3E, 0x97F4A3CD, 0x99307EE1,
	0x9A7365D3, 0x9BBD4283, 0x9D0DFE54, 0x9E658233,
	0x9FC3B694, 0xA1288377, 0xA293D067, 0xA405847F,
	0xA57D8667, 0xA6FBBC5A, 0xA8800C26, 0xAA0A5B2E,
	0xAB9A8E6C, 0xAD308A72, 0xAECC336C, 0xB06D6D25,
	0xB2141B02, 0xB3C0200D, 0xB5715EEF, 0xB727B9F7,
	0xB8E3131A, 0xBAA34BF4, 0xBC6845CE, 0xBE31E19C,
	0xC0000000, 0xC1D28150, 0xC3A94590, 0xC5842C7F,
	0xC763158F, 0xC945DFED, 0xCB2C6A83, 0xCD1693F7,
	0xCF043AB3, 0xD0F53CE0, 0xD2E9786F, 0xD4E0CB15,
	0xD6DB1255, 0xD8D82B7B, 0xDAD7F3A3, 0xDCDA47B9,
	0xDEDF047D, 0xE0E60685, 0xE2EF2A3F, 0xE4FA4BF2,
	0xE70747C4, 0xE915F9BA, 0xEB263DBB, 0xED37EF92,
	0xEF4AEAF1, 0xF15F0B74, 0xF3742CA2, 0xF58A29F2,
	0xF7A0DECA, 0xF9B82684, 0xFBCFDC71, 0xFDE7DBD9,
};

//------------------------------------------------------------------------
void pcm_init(const uint32_t samplerate, const uint32_t bitswidth) {
	volatile uint32_t value, c;

	if(samplerate == 0) {
		return;
	}
	switch(samplerate) {
	case 8000:
	case 16000:
	case 32000:
	case 44100:
	case 48000:
	case 96000:
	case 192000:
	case 384000:
		break;
	default:
		return;
	}
	switch(bitswidth) {
	case 16:
	case 24:
	case 32:
		break;
	default:
		return;
	}

	/* GPIO setting */
	value = GET32(BCM283X_GPIO_GPFSEL1);
	value &= ~(7 << 24); /* GPIO18 */
	value |=   4 << 24 ; /* ALT0 PCM_CLK */
	value &= ~(7 << 27); /* GPIO19 */
	value |=   4 << 27 ; /* ALT0 PCM_FS */
	PUT32(BCM283X_GPIO_GPFSEL1, value);
	value = GET32(BCM283X_GPIO_GPFSEL2);
	value &= ~(7 <<  3); /* GPIO21 */
	value |=   4 <<  3 ; /* ALT0 PCM_DOUT */
	PUT32(BCM283X_GPIO_GPFSEL2, value);
	PUT32(BCM283X_GPIO_GPPUD, 0);						/* disable pullUD */
	for(c = 0; c < 150; c++) value = GET32(BCM283X_GPIO_GPFSEL1);		/* wait 150 cycles */
	PUT32(BCM283X_GPIO_GPPUDCLK0, (1 << 18) | (1 << 19) | (1 << 21));	/* GPIO18/19/21 pullUD disable */
	for(c = 0; c < 150; c++) value = GET32(BCM283X_GPIO_GPFSEL1);		/* wait 150 cycles */
	PUT32(BCM283X_GPIO_GPPUDCLK0, 0);					/* remove the clock */

	/* Clock setting */
	/*
		0: GND
		1: oscillator 19.2 MHz
		2: testdebug0
		3: testdebug1
		4: PLLA 650 MHz per
		*: PLLB 400 MHz
		5: PLLC 200 MHz per (changes with overclock settings to 1000 MHz)
		6: PLLD 500 MHz per
		7: HDMI auxiliary 216 MHz

		GPCLK0, GPCLK1, and GPCLK2.
		Don't use GPCLK1 (it's probably used for the Ethernet clock).
	*/
	/* BITCLK(BCK,BCLK,BICK) = 32 clocks * 2 channels(stereo) = 64fs */
	/*    8kHz: oscillator 19.2MHz / (   8kHz * 64fs) = 37.5   : I = 37 : F = (0.5   * 4096) = 2048 */
	/*   16kHz: oscillator 19.2MHz / (  16kHz * 64fs) = 18.75  : I = 18 : F = (0.75  * 4096) = 3072 */
	/*   32kHz: oscillator 19.2MHz / (  32kHz * 64fs) =  9.375 : I =  9 : F = (0.375 * 4096) = 1536 */
	/* 44.1kHz: oscillator 19.2MHz / (44.1kHz * 64fs) =  6.802 : I =  6 : F = (0.802 * 4096) = 3284 */
	/*   48kHz: oscillator 19.2MHz / (  48kHz * 64fs) =  6.25  : I =  6 : F = (0.25  * 4096) = 1024 */
	/*   96kHz: oscillator 19.2MHz / (  96kHz * 64fs) =  3.125 : I =  3 : F = (0.125 * 4096) =  512 */
	/*  192kHz:       PLLD 500 MHz / ( 192kHz * 64fs) = 40.690 : I = 40 : F = (0.690 * 4096) = 2826 */
	/*  384kHz:       PLLD 500 MHz / ( 384kHz * 64fs) = 20.345 : I = 20 : F = (0.345 * 4096) = 1413 */
	switch(samplerate) {
	case 8000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x25800);	/* Div by I=37:F=2048 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 16000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x12C00);	/* Div by I=18:F=3072 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 32000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x9600);		/* Div by I= 9:F=1536 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 44100:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x6CD4);		/* Div by I= 6:F=3284 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 48000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x6400);		/* Div by I= 6:F=1024 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 96000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x21);		/* Disable Clock Generator, oscillator */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x3200);		/* Div by I= 3:F= 512 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x211);		/* Enable Clock Generator, 1stage MASH, oscillator */
		break;
	case 192000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x26);		/* Disable Clock Generator, PLLD */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x28B0A);	/* Div by I=40:F=2826 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x216);		/* Enable Clock Generator, 1stage MASH, PLLD */
		break;
	case 384000:
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD +  0x26);		/* Disable Clock Generator, PLLD */
		PUT32(BCM283X_CM_PCMDIV, BCM283X_CM_PASSWORD + 0x14585);	/* Div by I=20:F=1413 */
		PUT32(BCM283X_CM_PCMCTL, BCM283X_CM_PASSWORD + 0x216);		/* Enable Clock Generator, 1stage MASH, PLLD */
		break;
	}

	/* PCM init */
	/* MODE_A Register */
	/*
		bit24     FTXP : Transmit Frame Packed Mode
		 0 = TX FIFO word is written into a single channel.
		 1 = TX FIFO word is split into 2 16 bit words and used to fill both data channels in the same frame.
		bit22     CLKI : Clock Invert this logically inverts the PCM_CLK signal.
		 0  = Outputs change on rising edge of clock, inputs are sampled on falling edge.
		 1* = Outputs change on falling edge of clock, inputs are sampled on rising edge.
		bit21      FSM : Frame Sync Invert This logically inverts the frame sync signal.
		 0* = Master mode.
		 1  = Slave mode.
		bit20      FSI : Frame Sync Invert This logically inverts the frame sync signal.
		 0* = FS is normally low and goes high to indicate frame sync.
		 1  = FS is normally high and goes low to indicate frame sync.
		bit19:10  FLEN : Frame Length - Sets the frame length to (FLEN+1) clocks.
		 Used only when FSM == 0.
		bit 9: 0 FSLEN : Frame Sync Length - Sets the frame sync length to (FSLEN) clocks.
		 Used only when FSM == 0.
		 PCM_FS will remain permanently active if FSLEN >= FLEN.
		 0  = frame sync pulse is off.
	*/
	switch(bitswidth) {
	case 16:
		PUT32(BCM283X_PCM_MODE_A, 0x140FC20);	/* FTXP=1, CLKI=1, FLEN=64-1, FSLEN=32 */
		break;
	case 24:
	case 32:
		PUT32(BCM283X_PCM_MODE_A, 0x040FC20);	/* FTXP=0, CLKI=1, FLEN=64-1, FSLEN=32 */
		break;
	}
	/* TXC_A Register */
	/*
		bit31    CH1WEX : Channel 1 Width Extension Bit
		 This is the MSB of the channel 1 width (CH1WID). It allows widths greater than 24 bits
		bit30     CH1EN : Channel 1 Enable
		bit29:20 CH1POS : Channel 1 Position
		 0  = indicates the first clock of frame.
		bit19:16 CH1WID : Channel 1 Width
		 width of channel 1 in bit clocks.
		 This field has been extended with the CH1WEX bit giving a total width of (CH1WEX*16)+CH1WID+8.
		 The Maximum supported width is 32 bits.
		 0  = 8 bits wide
		bit15    CH2WEX : Channel 2 Width Extension Bit
		bit14     CH2EN : Channel 2 Enable
		bit13: 4 CH2POS : Channel 2 Position
		bit 3: 0 CH2WID : Channel 2 Width
	*/
	switch(bitswidth) {
	case 16:
		PUT32(BCM283X_PCM_TXC_A, 0x40184218);	/* CH1WEX=0, CH1EN=CH2EN=1, CH1POS=1, CH2POS=33, CH1WID=CH2WID=8 */
		break;
	case 24:
		PUT32(BCM283X_PCM_TXC_A, 0xC010C210);	/* CH1WEX=1, CH1EN=CH2EN=1, CH1POS=1, CH2POS=33, CH1WID=CH2WID=0 */
		break;
	case 32:
//		PUT32(BCM283X_PCM_TXC_A, 0xC008C208);	/* CH1WEX=1, CH1EN=CH2EN=1, CH1POS=0, CH2POS=32, CH1WID=CH2WID=8 */ /* Left-Justified, not I2S */
		PUT32(BCM283X_PCM_TXC_A, 0xC018C218);	/* CH1WEX=1, CH1EN=CH2EN=1, CH1POS=1, CH2POS=33, CH1WID=CH2WID=8 */
		break;
	}
	/* CS_A Register */
	/*
		bit24      SYNC : PCM Clock sync helper.
		 a software synchronisation mechanism to allow the software to detect when 2 PCM clocks have occurred.
		bit19       TXD : Indicates that the TX FIFO can accept data
		 0 = TX FIFO is full and so cannot accept more data.
		 1 = TX FIFO has space for at least 1 sample.
		bit17       TXW : Indicates that the TX FIFO needs Writing
		 0 = TX FIFO is at least TXTHR full.
		 1 = TX FIFO is less then TXTHR full.
		bit13    TXSYNC : TX FIFO Sync
		bit 9     DMAEN : DMA DREQ Enable
		 0* = Don't generate DMA DREQ requests.
		 1  = Generates a TX DMA DREQ requests whenever the TX FIFO level is lower than TXREQ
		bit 6: 5  TXTHR : TX FIFO threshold at which point the TXW flag is set
		 00  = set when the TX FIFO is empty
		 01  = set when the TX FIFO is less than full
		 10  = set when the TX FIFO is less than full
		 11* = set when the TX FIFO is full but for one sample
		bit 3     TXCLR : Clear the TX FIFO
		bit 2      TXON : Enable transmission
		bit 0        EN : Enable the PCM Audio Interface
	*/
//	PUT32(BCM283X_PCM_CS_A, 0x100206D);	/* SYNC=1, TXSYNC=1, TXTHR=3, TXCLR=1, TXON=1, EN=1 */
	PUT32(BCM283X_PCM_CS_A, 0x206D);	/* SYNC=0, TXSYNC=1, TXTHR=3, TXCLR=1, TXON=1, EN=1 */

	/* TXCLR + SYNC and wait */
	while(GET32(BCM283X_PCM_CS_A) & ((1 << 3) | (1 << 24)));
}
//------------------------------------------------------------------------
void pcm_play(azo_pcmdata_t* pcmdata) {
	uint32_t loc;
	uint32_t nowloopcount;
	uint32_t value;

	if(pcmdata->data == 0) {
		return;
	}

	do {
		for(nowloopcount = 0; nowloopcount < pcmdata->loopcount; nowloopcount++) {
			for(loc = 0; loc < pcmdata->size; loc += pcmdata->bitswidth / 8 * pcmdata->channels) {
				/* FIFO full wait */
				while((GET32(BCM283X_PCM_CS_A) & (1 << 19)) == 0);
				/* FIFO write */
				if(pcmdata->channels == 1) {
					switch(pcmdata->bitswidth) {
					case 16:
						value = ((uint16_t*)(pcmdata->data + loc))[0];
						PUT32(BCM283X_PCM_FIFO_A, (value << 16) | value);	/* Channel 1&2 */
						break;
					case 24:
						value = ((uint32_t)((uint8_t*)(pcmdata->data + loc))[0]) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[1]) << 8) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[2]) << 16);
						PUT32(BCM283X_PCM_FIFO_A, value);	/* Channel 1 */
						/* FIFO full wait */
						while((GET32(BCM283X_PCM_CS_A) & (1 << 19)) == 0);
						PUT32(BCM283X_PCM_FIFO_A, value);	/* Channel 2 */
						break;
					case 32:
						value = ((uint32_t*)(pcmdata->data + loc))[0];
						PUT32(BCM283X_PCM_FIFO_A, value);	/* Channel 1 */
						/* FIFO full wait */
						while((GET32(BCM283X_PCM_CS_A) & (1 << 19)) == 0);
						PUT32(BCM283X_PCM_FIFO_A, value);	/* Channel 2 */
						break;
					}
				} else {
					switch(pcmdata->bitswidth) {
					case 16:
						PUT32(BCM283X_PCM_FIFO_A, ((uint32_t)((uint16_t*)(pcmdata->data + loc))[0] << 16) | ((uint16_t*)(pcmdata->data + loc))[1]);	/* Channel 1&2 */
						break;
					case 24:
						PUT32(BCM283X_PCM_FIFO_A, ((uint32_t)((uint8_t*)(pcmdata->data + loc))[0]) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[1]) << 8) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[2]) << 16));	/* Channel 1 */
						/* FIFO full wait */
						while((GET32(BCM283X_PCM_CS_A) & (1 << 19)) == 0);
						PUT32(BCM283X_PCM_FIFO_A, ((uint32_t)((uint8_t*)(pcmdata->data + loc))[3]) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[4]) << 8) | ((uint32_t)(((uint8_t*)(pcmdata->data + loc))[5]) << 16));	/* Channel 2 */
						break;
					case 32:
						PUT32(BCM283X_PCM_FIFO_A, ((uint32_t*)(pcmdata->data + loc))[0]);	/* Channel 1 */
						/* FIFO full wait */
						while((GET32(BCM283X_PCM_CS_A) & (1 << 19)) == 0);
						PUT32(BCM283X_PCM_FIFO_A, ((uint32_t*)(pcmdata->data + loc))[1]);	/* Channel 2 */
						break;
					}
				}
			}
		}
	} while(pcmdata->endless);
}
//------------------------------------------------------------------------
int notmain(unsigned int earlypc)
{
	azo_pcmdata_t pcmdata = {
		96,
		1,
		48000,
		16,
		1,
		1,
		azo_sin_1k_48khz16bit
	};

/*	azo_pcmdata_t pcmdata = {
		192,
		1,
		48000,
		32,
		1,
		1,
		azo_sin_1k_48khz32bit
	};
*/
/*	azo_pcmdata_t pcmdata = {
		384,
		1,
		96000,
		32,
		1,
		1,
		azo_sin_1k_96khz32bit
	};
*/
/*	azo_pcmdata_t pcmdata = {
		768,
		1,
		384000,
		16,
		1,
		1,
		azo_sin_1k_384khz16bit
	};
*/
/*	azo_pcmdata_t pcmdata = {
		1536,
		1,
		384000,
		32,
		1,
		1,
		azo_sin_1k_384khz32bit
	};
*/
/*	azo_pcmdata_t pcmdata = {
		576,
		1,
		192000,
		24,
		1,
		1,
		azo_sin_1k_192khz24bit
	};
*/
/*	azo_pcmdata_t pcmdata = {
		288,
		1,
		96000,
		24,
		1,
		1,
		azo_sin_1k_96khz24bit
	};
*/

	pcm_init(pcmdata.samplerate, pcmdata.bitswidth); 
	pcm_play(&pcmdata);

	while(1);

	return(0);
}

//-------------------------------------------------------------------------
