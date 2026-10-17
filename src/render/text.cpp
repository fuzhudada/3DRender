#include "text.h"

static const uint8_t* glyph(char c) {
    static const uint8_t zero[8]  = {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00};
    static const uint8_t one[8]   = {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00};
    static const uint8_t two[8]   = {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00};
    static const uint8_t three[8] = {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00};
    static const uint8_t four[8]  = {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00};
    static const uint8_t five[8]  = {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00};
    static const uint8_t six[8]   = {0x3C,0x66,0x60,0x7C,0x66,0x66,0x3C,0x00};
    static const uint8_t seven[8] = {0x7E,0x66,0x0C,0x18,0x18,0x18,0x18,0x00};
    static const uint8_t eight[8] = {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00};
    static const uint8_t nine[8]  = {0x3C,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00};

    static const uint8_t F[8] = {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00};
    static const uint8_t P[8] = {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00};
    static const uint8_t S[8] = {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00};
    static const uint8_t m[8] = {0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xC6,0x00};
    static const uint8_t s[8] = {0x00,0x00,0x3C,0x60,0x3C,0x06,0x7C,0x00};
    static const uint8_t colon[8] = {0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00};
    static const uint8_t dot[8]   = {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00};
    static const uint8_t blank[8] = {0,0,0,0,0,0,0,0};

    switch (c) {
        case '0': return zero;  case '1': return one;
        case '2': return two;   case '3': return three;
        case '4': return four;  case '5': return five;
        case '6': return six;   case '7': return seven;
        case '8': return eight; case '9': return nine;
        case 'F': return F;     case 'P': return P;
        case 'S': return S;     case 'm': return m;
        case 's': return s;     case ':': return colon;
        case '.': return dot;
        default:  return blank;
    }
}

void drawChar(std::vector<uint32_t>& fb, int w, int h, int x, int y, char c, uint32_t color, int scale) {
    const uint8_t* g = glyph(c);
    for(int i=0;i<8;i++){
        uint8_t bits = g[i];
        for(int j=0;j<8;j++){
            for(int k=0;k<scale;k++){
                for(int l=0;l<scale;l++){
                    int px = x + (7 - j) * scale + k;
                    int py = y + i * scale + l;
                    if((bits & (1 << j)) && px < w && py < h){
                        fb[py * w + px] = color;
                    }
                }
            }
        }
    }
}

void drawText(std::vector<uint32_t>& fb, int w, int h, int x, int y, const std::string& text, uint32_t color, int scale) {
    for(int i=0;i<text.size();i++){
        drawChar(fb, w, h, x + i * 8 * scale, y, text[i], color, scale);
    }
}
