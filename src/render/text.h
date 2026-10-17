#include <cstdint>
#include <vector>
#include <string>

static const uint8_t* glyph(char c);

void drawChar(std::vector<uint32_t>& fb, int w, int h, int x, int y, char c, uint32_t color, int scale);

void drawText(std::vector<uint32_t>& fb, int w, int h, int x, int y, const std::string& text, uint32_t color, int scale);
