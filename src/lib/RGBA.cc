#include "RGBA.hh"

const char RGBA::hex_chars[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
};

void RGBA::def() {
    r = 0xff;
    g = 0xff;
    b = 0xff;
    a = 0xff;
}
RGBA::RGBA(int v) {
    a = v & 0xff;
    b = (v & 0xff00) >> 4 * 2;
    g = (v & 0xff0000) >> 4 * 4;
    r = (v & 0xff000000) >> 4 * 6;
}
RGBA::RGBA(int r, int g, int b, int a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
RGBA::RGBA(const char* hexstr) {
    auto ch2v = [](char ch) {
        int test;

        test = ch - '0';
        if (0 <= test && test <= 9) {
            return test;
        }

        test = ch - 'A' + 10;
        if (10 <= test && test <= 15) {
            return test;
        }

        test = ch - 'a' + 10;
        if (10 <= test && test <= 15) {
            return test;
        }

        return -1;
    };

    if (*hexstr == '#') {
        hexstr++;
    }
    int len = strlen(hexstr);
    if (len < 6) {
        def();
        return;
    }

    int v4[4] = {0};
    int vi = 0;
    for (int u = 0; u < len - 1; u++) {
        int h = ch2v(hexstr[0]);
        int l = ch2v(hexstr[1]);
        if (h == -1 || l == -1) {
            if (vi < 3) {
                def();
                return;
            } else {
                break;
            }
        }

        v4[vi++] = 0 | h << 4 | l;
        hexstr += 2;
    }

    r = v4[0];
    g = v4[1];
    b = v4[2];
    if (vi == 4) {
        a = v4[3];
    }
    return;
}

int RGBA::rgba() {
    return r << 6 * 4 | g << 4 * 4 | b << 2 * 4 | a;
}
int RGBA::bgra() {
    return b << 6 * 4 | g << 4 * 4 | r << 2 * 4 | a;
}
int RGBA::rgb() {
    return r << 4 * 4 | g << 2 * 4 | b;
}
int RGBA::bgr() {
    return b << 4 * 4 | g << 2 * 4 | r;
}
string RGBA::hex() {
    string str("#");
    str += hex_chars[r >> 4];
    str += hex_chars[r & 0xf];
    str += hex_chars[g >> 4];
    str += hex_chars[g & 0xf];
    str += hex_chars[b >> 4];
    str += hex_chars[b & 0xf];
    str += hex_chars[a >> 4];
    str += hex_chars[a & 0xf];
    return str;
}
