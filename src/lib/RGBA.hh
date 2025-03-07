#ifndef __RGBA_HH
#define __RGBA_HH

#include <string>

using string = std::string;
struct RGBA {
private:
    void def();
public:
    static const char hex_chars[];
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    RGBA(int v);
    RGBA(int r, int g, int b, int a);
    RGBA(const char* hexstr);

    int rgba();
    int bgra();
    int rgb();
    int bgr();
    string hex();
};

#endif