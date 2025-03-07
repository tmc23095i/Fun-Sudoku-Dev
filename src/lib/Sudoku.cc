#include <iostream>
#include <random>
#include <bit>
#include "Sudoku.hh"
using namespace std;

void Sudoku::uncommit(int index) {
    setDigit(index, 0);

    Sudoku temp;
    for (int i = 0; i < 81; i++) {
        int d = this->digit(index);
        if (d != 0) {
            temp.commit(i, d);
        }
    }

    *this = std::move(temp);
};

bool Sudoku::checkAnswer(const Sudoku* a) {
    char str_a[82];
    a->toString(str_a);

    Sudoku theAns(this);
    char str_theAns[82];
    while (theAns.crack()) {
        theAns.toString(str_theAns);
        if (strcmp(str_theAns, str_a) == 0) {
            return true;
        }
    }
    return false;
}

bool Sudoku::makeGame() {
    auto dbg = [&](Sudoku* _this, int index, int digit, string title = string()) {
        index = (index < 0 || index > 80) ? 0 : index;
        digit = (digit < 1 || digit > 9) ? 0 : digit;
        string str;
        if (index < 0 || index > 80 || digit < 1 || digit > 9) {
            str = _this->toPrettyString();
        } else {
            str = _this->toPrettyString(index);
        }

        cout << title << endl;

        int c, r, nll;
        _this->i2rcb(index, r, c, nll);
        cout << "[c" << c + 1 << ", r" << r + 1 << "]" << index << "@" << digit;

        int ml = _this->marklist(index);
        bitset<5> l5(ml >> 4);
        bitset<4> r4(ml & 0b1111);
        cout << ": 0b" << l5.to_string() << " " << r4.to_string() << endl;

        cout << str.c_str() << endl
             << endl;
        return 0;
    };

    // 检查当前数独是否无解
    Sudoku* temp = new Sudoku(this->toString());
    if (temp->crack() == false) {
        return false;
    }

    // 记录答案字串
    char ans[90];
    temp->toString(ans);

    // 用于生成题目数独的字串
    char making[90];
    temp->toString(making);

    // 随机要置零的格子数量
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(40, 55);
    int ERASED_COUNT = dist(gen);

    // 随机一个已提交的格子索引, 并返回, 没有则返回-1,
    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<int> dist2(0, 80);
    auto todoIndex = [&]() {
        int ri = dist2(gen2);

        for (int i = ri; 0 <= i; i--) {
            if (temp->digit(i) != 0) {
                return i;
            }
        }
        for (int i = ri + 1; i <= 80; i++) {
            if (temp->digit(i) != 0) {
                return i;
            }
        }
        // dbg(should_index, -1);
        return -1; // 不会出现的情况, 因为只置零一半左右的格子
    };

    constexpr int MAX_NONOK = 5;
    int non_ok = 0;
    constexpr int MAX_BACK = 200;
    int back_cnt = 0;
    bool flag;
    class Cells {
    public:
        Cells()
            : index(0)
            , digit(0)
            , n(0) {
        }
        void push(int i, int d) {
            index[n] = i;
            digit[n] = d;
            n++;
        }
        void pop(int &i, int &d) {
            n--;
            i = index[n];
            d = digit[n];
        }
        void at(int ii, int &i, int &d) {
            i = index[ii];
            d = digit[ii];
        }
        int count() {
            return n;
        }
    private:
        char index[81];
        char digit[81];
        int n;
    } cs;

    while (true) {
        dbg(temp, -1, -1, "循环开始");
        int index = todoIndex();
        int digit = making[index] - '0'; // digit char
        // 置零格子
        making[index] = '0';
        temp->clear();
        temp->load(making);
        dbg(temp, index, digit, "去掉一个格子");
        // 测试格子是否需要玩家试错才能填出来
        temp->fill();
        dbg(temp, index, digit, "填充");
        bool ok = temp->isFull(); // 推理可得, 无需试错
        if (ok) {

            non_ok = 0;
            // 记录格子
            cs.push(index, digit);
            // 判断是否已达数量
            if (cs.count() == ERASED_COUNT) {
                temp->clear();
                temp->load(making);
                flag = true;
                break;
            }
        } else {
            // 恢复数独
            making[index] = digit + '0';
            // 连续获得不合适格子时, 回溯上个格子
            non_ok++;
            if (non_ok == MAX_NONOK) {
                non_ok = 0;
                if (cs.count() > 0) {
                    cs.pop(index, digit);
                    making[index] = digit + '0';
                } else {
                    // 没有格子回溯则直接重新选, 能在这里纯属运气差
                }
                if (++back_cnt == MAX_BACK) {
                    flag = false;
                    break;
                }
            }
        }
        // 格子可用时恢复数独到已置零此格子; 格子不可用时恢复数独到置零格子前或回溯到上个格子前
        temp->clear();
        temp->load(making);
        dbg(temp, index, digit, "循环结束: 合格格子则去掉格子的数独; 不合格则去掉前的数独");
    }

    if (flag == true) {
        *this = *temp;
    }
    delete temp;
    return flag;
};

Sudoku Sudoku::random() {
    Sudoku sdk;
    auto dbg = [&](int index, int digit, string title = string()) {
        cout << title << endl;

        int c, r, nll;
        sdk.i2rcb(index, r, c, nll);
        cout << "[c" << c + 1 << ", r" << r + 1 << "]" << index << "@" << digit;

        int ml = sdk.marklist(index);
        bitset<5> l5(ml >> 4);
        bitset<4> r4(ml & 0b1111);
        cout << ": 0b" << l5.to_string() << " " << r4.to_string() << endl;

        cout << sdk.toPrettyString().c_str() << endl
             << endl;
        return 0;
    };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 9);
    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<int> dist2(0, 80);
    // 随机一个1~9的数字
    auto ran1_9 = [&]() {
        return dist(gen);
    };
    // 随机一个1~9的数字
    auto ran0_80 = [&]() {
        return dist2(gen2);
    };
    // 提供一个希望的索引值, 返回合法的索引值, 合法规则是优先相等, 其次是小于的最大值, 最后是大于的最小值, 没有找到则返回-1.
    auto validIndex = [&](int should_index) {
        for (int i = should_index; 0 <= i; i--) {
            if (sdk.digit(i) == 0) {
                return i;
            }
        }
        for (int i = should_index + 1; i <= 80; i++) {
            if (sdk.digit(i) == 0) {
                return i;
            }
        }
        // dbg(should_index, -1);
        return -1;
    };
    // 提供一个希望的数字, 返回合法的数字, 合法规则是优先相等, 其次是小于的最大值, 最后是大于的最小值, 没有则返回-1.
    auto validDigit = [&](int index, int digit) {
        for (int d = digit; 1 <= d; d--) {
            if (sdk.isMarked(index, d) == false) {
                return d;
            }
        }
        for (int d = digit + 1; d <= 9; d++) {
            if (sdk.isMarked(index, d) == false) {
                return d;
            }
        }
        // dbg(index, digit);
        return -1;
    };

    while (true) {
        constexpr int RANDOM_COUTN = 15;
        class Cells {
        public:
            Cells()
                : index(0)
                , digit(0)
                , n(0) {
            }
            void push(int i, int d) {
                index[n] = i;
                digit[n] = d;
                n++;
            }
            void pop(int &i, int &d) {
                n--;
                i = index[n];
                d = digit[n];
            }
            void at(int ii, int &i, int &d) {
                i = index[ii];
                d = digit[ii];
            }
            int count() {
                return n;
            }
        private:
            char index[81];
            char digit[81];
            int n;
        } cs;
        int digit;
        int index;
        while (true) {
            // commit random cell
            index = validIndex(ran0_80());
            if (index == -1) {
                break;
            }
            digit = validDigit(index, ran1_9());
            if (digit != -1) {
                if (sdk.commit(index, digit) == true) {
                    cs.push(index, digit);
                    if (cs.count() == RANDOM_COUTN || cs.count() == 81) {
                        break;
                    } else {
                        continue;
                    }
                }
            }

            // invalid sudoku and go back
            // dbg(index, digit);
            int csi, csd;
            cs.pop(csi, csd);
            sdk.reload();
            for (int i = 0; i < cs.count(); i++) {
                cs.at(i, csi, csd);
                sdk.commit(csi, csd);
            }
            continue;
        }

        // 填满答案
        if (sdk.crack() == false) {
            continue;
        }
        return sdk;
    }
};

string Sudoku::toPrettyString(int gazingIndex, int gazingStyle) {
    char str[521];
    toPrettyString(str, gazingIndex, gazingStyle);
    return string(str);
}
void Sudoku::toPrettyString(char* result, int gazedIndex, int gazedStyle) {
    // 准备格式所需的字符
    static const char CH_SPACE = ' ';
    static const char CH_VTC = '|';
    static const char CH_HRZ = '-';
    static const char CH_CORNER = '+';
    static const char CH_UNCOMMITTED = ' ';
    static const int PRETTY_SIZE = 40 * 4 * 3 + 40 + 1; // 输出缓冲区至少需要521个字节

    if (result == nullptr) {
        return;
    }

    // 准备注视样式
    char ch_gl = CH_SPACE;
    char ch_gr = CH_SPACE;
    if (0 <= gazedIndex && gazedIndex <= 80) {
        auto setGazingStyle = [](int gi, char &gl, char &gr) {
            static const char gls[] = "<[(>_*|-=~";
            static const char grs[] = ">])<_*|-=~";
            static const int max = strlen(gls) - 1;
            gi = (0 <= gi) ? gi : 0;
            gi = (gi <= max) ? gi : max;

            gl = gls[gi];
            gr = grs[gi];
        };
        setGazingStyle(gazedStyle, ch_gl, ch_gr);
    }

    char* pretty = new char[PRETTY_SIZE];
    int pretty_cursor = 0;
    auto push = [&](char ch) {
        pretty[pretty_cursor++] = ch;
    };

    { // 制作美丽字串
        char* sudoku_str = new char[82];
        toString(sudoku_str);

        int i = 0;
        char ch_1, ch_2, ch_3;
        // print lines
        for (int ln = 0; ln < 13; ln++) {
            // confirm if the current line is border line
            bool border_line = false;
            if (ln % 4 == 0) {
                border_line = true;
            }

            // print the left border of one line
            ch_1 = CH_VTC;
            if (border_line == true) {
                ch_1 = CH_CORNER;
            }
            push(' ');
            push(ch_1);

            // print 3 blos of one line
            for (int bl = 0; bl < 3; bl++) {
                // print left border of one blo
                ch_1 = CH_SPACE;
                if (border_line == true) {
                    ch_1 = CH_HRZ;
                }
                push(ch_1);

                //  print 3 cells of one blo
                for (int cl = 0; cl < 3; cl++) {
                    if (border_line == true) {
                        ch_1 = CH_HRZ;
                        ch_2 = CH_HRZ;
                        ch_3 = CH_HRZ;
                    } else {
                        ch_1 = CH_SPACE;
                        ch_3 = CH_SPACE;
                        if (i == gazedIndex) {
                            ch_1 = ch_gl;
                            ch_3 = ch_gr;
                        }
                        ch_2 = sudoku_str[i];
                        if (ch_2 == '0') {
                            ch_2 = CH_UNCOMMITTED;
                        }
                        i++;
                    }
                    push(ch_1);
                    push(ch_2);
                    push(ch_3);
                }
                // print the right border of one blo
                ch_1 = CH_SPACE;
                ch_2 = CH_VTC;
                if (border_line == true) {
                    ch_1 = CH_HRZ;
                    ch_2 = CH_CORNER;
                }
                push(ch_1);
                push(ch_2);
            }
            // print one line finished
            push(' ');
            push('\n');
        }
        // print \0
        push('\0');
        delete[] sudoku_str;
    }

    // 返回结果
    strcpy(result, pretty);
    delete[] pretty;
    return;

    /* style like this
    "
     +-----------+-----------+-----------+ \n
     |  2  6  3  |  5  1  7  |  8  9  4  | \n
     |  7  8  1  |  9  4  2  |  3  5  6  | \n
     |  9  4  5  |  8  3  6  |  2  7  1  | \n
     +-----------+-----------+-----------+ \n
     |  5  1  8  |  6  9  4  |  7  3  2  | \n
     |  4  3  9  |  7  2  5  |  1  6  8  | \n
     |  6  2  7  |  1  8  3  |  5  4  9  | \n
     +-----------+-----------+-----------+ \n
     |  1  7  4  |  3  6  8  |  9  2  5  | \n
     |  3  9  2  |  4  5  1  |  6  8  7  | \n
     |  8  5  6  |  2  7  9  |  4  1  3  | \n
     +-----------+-----------+-----------+ \n
    0x0000000147004330 " +-----------+-----------+-----------+ \n |  2  6  3  |  5  1  7  |  8  9  4  | \n |  7  8  1  |  9  4  2  |  3  5  6  | \n |  9  4  5  |  8  3  6  |  2  7  1  | \n +-----------+-----------+-----------+ \n |  5  1  8  |  6  9  4  |  7  3  2  | \n |  4  3  9  |  7  2  5  |  1  6  8  | \n |  6  2  7  |  1  8  3  |  5  4  9  | \n +-----------+-----------+-----------+ \n |  1  7  4  |  3  6  8  |  9  2  5  | \n |  3  9  2  |  4  5  1  |  6  8  7  | \n |  8  5  6  |  2  7  9  |  4  1  3  | \n +-----------+-----------+-----------+ \n"
    "
    */
}

bool Sudoku::load(const char* str) {
    // 字串不足81
    int len = strlen(str);
    if (len < 81) {
        return false;
    }

    // 从字串读取数字
    Sudoku* temp = new Sudoku(*this);
    int committed = 0;
    for (int i = 0; i < len; i++) {
        int d = str[i] - '0';
        if (0 <= d && d <= 9) {
            if (d != 0) {
                // 提交非零数字
                if (temp->commit(committed, d) == false) {
                    // 提交失败, 试图提交不可用的数字
                    break;
                }
            }
            temp->setOrg(committed, d);
            if (++committed == 81) {
                // 数独加载完毕
                break;
            }
        }
    }

    // 成功载入数独
    if (committed == 81) {
        *this = *temp;
        return true;
    }

    // 载入失败, 数字不足81, 恢复数独
    delete temp;
    return false;
}

void Sudoku::toString(char str[82], char zero) const {
    for (int i = 0; i < 81; i++) {
        str[i] = digit(i) + '0';
        if (str[i] == '0') {
            str[i] = zero;
        }
    }
    str[81] = 0;
}
string Sudoku::toString(char zero) const {
    char str[82];
    toString(str);
    return string(str);
}
void Sudoku::orgString(char str[82]) {
    for (int i = 0; i < 81; i++) {
        str[i] = orgDigit(i) + '0';
    }
    str[81] = 0;
}

void Sudoku::clear() {
    for (int i = 0; i < 81; i++) {
        bits[i] = 0;
    }
    count = 0;
}

void Sudoku::_uncommit(int index) {
    // 此格子没被提交过
    int digit = this->digit(index);
    if (digit == 0) {
        return;
    }

    // 取消提交
    setDigit(index, 0);
    count--;

    // 取消我对同组格子的此数字占用标记
    int x, y, z;
    i2rcb(index, x, y, z);
    auto unmarkMate = [&](int mi) {
        // 跳过自己
        if (mi == index) {
            return;
        }

        // 此格子的同组格子们没有占用这个数字, 即这个格子的数字是被我占用的
        if (false == digitOccurredInMate(mi, digit)) {
            unmark(mi, digit);
        }
    };
    for (int i = 0; i < 9; i++) {
        unmarkMate(x);
        unmarkMate(y);
        unmarkMate(z);

        x += 9;
        y += 1;
        z += ((i == 2 || i == 5) ? 7 : 1);
    }
}

bool Sudoku::digitOccurredInMate(int index, int digit) {
    // 获取同组首格子的索引
    int x, y, z;
    i2rcb(index, x, y, z);

    // 检查同组格子的此数字占用情况, 被占用时返回真
    auto locked = [&](int mi) {
        // 跳过自己
        if (mi == index) {
            return false;
        }
        // 指定的同组格子已占用此数字
        int md = this->digit(mi);
        if (md == digit) {
            return true;
        }
        // 未占用此数字
        return false;
    };
    for (int i = 0; i < 9; i++) {
        // 检测到被已占用
        if (locked(x) || locked(y) || locked(z)) {
            return true;
        }

        x += 9;
        y += 1;
        z += ((i == 2 || i == 5) ? 7 : 1);
    }

    return false;
}

// 将数字填入指定的格子中, 数字必须是[1, 9], 否则返回假
// 如果数字和当前数独存在冲突, 则无法填入, 将返回假
// 重复提交相同的数字时, 直接返回真
bool Sudoku::commit(int index, int digit) {
    // 同样的数字重复提交
    if (digit == this->digit(index)) {
        return true;
    }

    // 数字非法, 无法提交
    if (digit <= 0 || digit > 9) {
        return false;
    }

    // 是不可用的数字, 返回假
    if (isDigitAvailable(index, digit) == false) {
        return false;
    }

    // 写入数字
    setDigit(index, digit);
    setMarklist(index, ~SOLEBIT[digit] & MARKLIST_MASK >> MARKLIST_SHIFT);
    count++;

    // 获取同组首格子的索引
    int x = 0,
        y = 0, z = 0;
    i2fi(index, x, y, z);
    //  为同组格子标记此数字不可用
    auto markMate = [&](int mi) {
        if (mi == index) {
            return;
        }
        mark(mi, digit);
    };
    for (int i = 0; i < 9; i++) {
        markMate(x);
        markMate(y);
        markMate(z);

        x += 9;
        y += 1;
        z += ((i == 2 || i == 5) ? 7 : 1);
    }

    return true;
}

int Sudoku::isMarkSole(int index) {

    switch (~marklist(index)) {
    case SOLEBIT[1]: return 1;
    case SOLEBIT[2]: return 2;
    case SOLEBIT[3]: return 3;
    case SOLEBIT[4]: return 4;
    case SOLEBIT[5]: return 5;
    case SOLEBIT[6]: return 6;
    case SOLEBIT[7]: return 7;
    case SOLEBIT[8]: return 8;
    case SOLEBIT[9]: return 9;
    default:
        return 0;
    }
}

bool Sudoku::allMatesUnavailable(int index, int digit) {
    int x, y, z;
    i2fi(index, x, y, z);
    bool bingo;

    bingo = true;
    for (int i = 0; i < 9; i++) {
        if (x != i) {
            if (isDigitAvailable(x, digit)) {
                // 同组的其他格子可用这个数字
                bingo = false;
                break;
            }
        }
        x += 9;
    }
    if (bingo) {
        return true;
    }

    bingo = true;
    for (int i = 0; i < 9; i++) {
        if (y != i) {
            if (isDigitAvailable(y, digit)) {
                // 同组的其他格子可用这个数字
                bingo = false;
                return false;
            }
        }
        y++;
    }
    if (bingo) {
        return true;
    }

    bingo = true;
    for (int i = 0; i < 9; i++) {
        if (z != i) {
            if (isDigitAvailable(z, digit)) {
                // 同组的其他格子可用这个数字
                bingo = false;
                return false;
            }
        }
        z += ((i == 2 || i == 5) ? 7 : 1);
    }
    if (bingo) {
        return true;
    }

    return false;
}

bool Sudoku::detectBy(int plan, char result[82]) {
    int count = 0;                    // 探测到可提交数字的格子数量(同一个格子探测到不同的数字或多次相同数字时, 计数只+1)
    auto record = [&](int i, int d) { // 记录一个探测到的可提交数字
        // 记录首次探测
        result[i] = d;
        count++;
        return true;
    };
    auto planA = [&](int i) {
        int d = isMarkSole(i);
        if (d != 0) {
            record(i, d);
        }
    };
    auto planB = [&](int i) {
        for (int ad = 1; ad <= 9; ad++) {
            // 跳过不可用数字
            if (isDigitAvailable(i, ad) == false) {
                continue;
            }

            int xi, yi, zi;
            i2fi(i, xi, yi, zi);
            for (int g = 0; g < 3; g++) {
                // 获取此组格子首地址
                int gi = 0;
                int gf = 0;
                switch (g) {
                case 0:
                    gi = yi;
                    gf = 1;
                    break;
                case 1:
                    gi = xi;
                    gf = 9;
                    break;
                case 2:
                    gi = zi;
                    gf = 1;
                    break;
                }
                // 检查此数字在此组的可用格子数
                int enabled = 0;
                for (int u = 0; u < 9 && enabled < 2; u++) {
                    enabled += isDigitAvailable(gi, ad);
                    gi += gf;
                    gi += (g == 2 && (u == 2 || u == 5)) ? 6 : 0;
                }
                if (enabled == 1) {
                    record(i, ad);
                    break;
                }
            }
        }
    };

    // 检查此格子的标记, 是否只有一个可用数字
    result[81] = -99;
    for (int i = 0; i < 81; i++) {
        // 初始化结果数组
        result[i] = 0;
        // 检查格子合法
        if (isMarkFull(i)) {
            return false;
        }
        // 检查未提交的格子是否有数字可提交
        int d = digit(i);
        if (d == 0) {
            switch (plan) {
            case 1:
                planA(i);
                break;
            case 2:
                planB(i);
                break;
            default:
                planA(i);
                planB(i);
            }
        }
    }

    // 记录探测到的可提交数字数量
    result[81] = count;
    return true;
}
bool Sudoku::detect(char result[82], bool willCommit) {
    // 调用者不需要可提交数字的数组
    bool freed = false;
    if (result == nullptr) {
        result = new char[82];
        freed = true;
    }

    bool valid = detectBy(0, result);
    if (valid) {
        // 提交所有探测到的数字
        if (willCommit == true) {
            commit(result);
        }
    }

    if (freed == true) {
        delete[] result;
    }
    return valid;
}

int Sudoku::commit(char digits[81]) {
    int count = 0;
    for (int i = 0; i < 81; i++) {
        int d = digits[i];
        if (1 <= d && d <= 9) {
            count++;
            bool b = commit(i, d);
            if (b == false) {
                // ...无法提交这个“可提交数字”时
                // 因为参数是合法数字数组, 所以应该不会出现这种情况, 如若出现异常问题, 可来此处检查、改正
                count--;
            }
        }
    }
    return count;
}

int Sudoku::getConflicts(int index, int conflicts[8 + 8 + 4]) {
    int count = 0;
    int d = digit(index);

    int x, y, z;
    i2fi(index, x, y, z);
    auto check = [&](int i) {
        if (d == digit(i)) {
            conflicts[count++] = i;
        }
    };
    for (int i = 0; i < 9; i++) {
        // 检查冲突的格子
        if (x != index) {
            check(x);
        }
        if (y != index) {
            check(y);
        }
        if (z != index) {
            check(z);
        }

        // 迭代同组格子索引
        x += 9;
        y++;
        z += (i == 2 || i == 5) ? 7 : 1;
    }

    // 查重并修正
    for (int i = 0; i < count; i++) { // 后续雷同项以-1标记
        for (int ii = i + 1; ii < count; ii++) {
            if (conflicts[i] == conflicts[ii]) {
                conflicts[ii] = -1;
            }
        }
    }
    // 删除-1间隙
    int ct = 0;
    for (int i = 0; i < count; i++) {
        if (conflicts[i] != -1) {
            conflicts[ct++] = conflicts[i];
            continue;
        }
        do {
            if (++i == count) {
                break;
            }
            if (conflicts[i] != -1) {
                break;
            }
        } while (true);
    }
    // 用-1标记后续项目无效
    for (int i = ct; i < count; i++) {
        conflicts[i] = -1;
    }
    count = ct;

    return count;
}

// 非法时返回假
bool Sudoku::fill() {
    int total = 0;
    char toCommit[82];
    bool valid;
    int plan = 1;
    /*     for (int p = 1; p <= 2; p++) {
            while (true) {
                valid = detectBy(plan, toCommit);
                if (valid == false) {
                    return false;
                }
                int count = toCommit[81];
                if (count > 0) {
                    commit(toCommit);
                    total += count;
                    continue;
                }
                if (plan == 1) {
                    plan = 2;
                    continue;
                }
                break;
            }
        } */

    while (true) {
        valid = detectBy(plan, toCommit);
        if (valid == false) {
            return false;
        }
        int count = toCommit[81];
        // switch plan
        if (count == 0) {
            // 探测方式1无结果时切换到方式2
            if (plan == 1) {
                plan = 2;
                continue;
            } else {
                // 方式2无结果时结果探测
                break;
            }
        }
        // commit
        commit(toCommit);
        total += count;
        if (plan == 2) {
            // 方式2探测到并提交后, 换回方式1
            plan = 1;
        }
    }

    return true;
}
bool Sudoku::crack() {
    int try_index = -1;
    int try_digit = -1;

    // 0. 填充
    auto S0_FillIn = [this]() {
        return this->fill();
    };
    // 1. 答案出现
    auto S1_IfAnswer = [this]() {
        return isFull();
    };
    // 2. 深入格子
    auto S2_NextCell = [this]() { // 寻找首个未提交的格子, 可用数最少的格子优先, 返回其索引, 未找到则返回-1
        int index;
        int count = 999; // 一个格子的可用数数量
        for (int i = 80; i >= 0; i--) {

            // 跳过已提交的格子
            if (digit(i) > 0) {
                continue;
            }
            // 获取可用数字数量
            int c = std::popcount(~marklist(i));
            if (c > count) {
                continue;
            }
            // 发现目前可用数最少的格子, 记录
            bool saved = false;
            for (int to = 1; to < tryingCount(); to++) {
                int ti, td;
                trying(to, ti, td);
                if (ti == i) {
                    saved = true;
                    break;
                }
            }
            if (saved == false) {
                count = c;
                index = i;
            }
        }
        // 没有下个格子了
        if (count == 999) {
            index = -1;
        }
        return index;
    };
    // 3. 平行数字
    auto S3_NextDigit = [this](int index, int currDigit) { // 获取指定格子的下一个可用数字, 没有则返回-1
        currDigit = (currDigit < 1) ? 1 : currDigit + 1;
        for (int d = currDigit; d <= 9; d++) {
            // 跳过不可用的数字
            if (isDigitAvailable(index, d) == false) {
                continue;
            }
            return d;
        }
        return -1;
    };
    // -- 撤销尝试
    // 恢复数独到最后一次trying前, 并输出最后一次trying
    auto backtrack = [this, try_index, try_digit, S0_FillIn](int &ti, int &td) {
        //  恢复数独到起始状态, 但是保留数独数独中的尝试进度
        auto reloadButTrying = [&]() {
            count = 0;
            for (int i = 0; i < 81; i++) {
                // 备份要保留的数据
                int d = orgDigit(i);
                int ti, td;
                trying(i, ti, td);

                // 重置bits
                bits[i] = 0;

                // 恢复数据
                setOrg(i, d);
                setTrying(i, ti, td);
            }

            // 重新提交数独
            for (int i = 0; i < 81; i++) {
                int d = orgDigit(i);
                commit(i, d);
            }
        };

        int cnt = tryingCount();
        if (cnt <= 0) {
            return false;
        }
        trying(cnt, ti, td);

        reloadButTrying();
        setTryingCount(--cnt);
        for (int i = 1; i <= cnt; i++) {
            int ti, td;
            trying(i, ti, td);

            S0_FillIn();
            commit(ti, td);
        }
        S0_FillIn();

        return true;
    };
    // 6. 撤销数字
    auto S6_UndoDigit = [backtrack](int &ti, int &td) {
        int tti, ttd;
        if (backtrack(tti, ttd)) {
            ti = tti;
            td = ttd;
            return true;
        }
        return false;
    };
    // 7. 尝试数字
    auto S7_TryDigit = [this](int i, int d) {
        commit(i, d);
        pushTrying(i, d);
    };

    // 9. 回溯格子
    auto S9_PrevCell = [backtrack](int &ti, int &td) {
        int tti, ttd;
        if (backtrack(tti, ttd) && backtrack(tti, ttd)) {
            ti = tti;
            td = ttd;
            return true;
        }
        return false;
    };
    // 10. go s3

    int tn = tryingCount();
    if (1 <= tn && tn <= 81) {
        trying(tryingCount(), try_index, try_digit);
    }
    if (S1_IfAnswer() == true) {
        // 数独已经是答案, 控制函数返回一真一假
        if (tn == 0) {
            setTryingCount(99);
            return true;
        }
        if (tn == 99) {
            setTryingCount(0);
            return false;
        }
        goto next_answer;
    }
    goto main;

succeed:
    return true;
failed:
    return false;

main:
    do {
        if (S0_FillIn() == true) {
            if (S1_IfAnswer() == true) {
                goto succeed;
            }
            try_index = S2_NextCell();
            try_digit = 0;
        } else {
        next_answer:
            if (S6_UndoDigit(try_index, try_digit) == false) {
                goto failed;
            }
        }

        do {
            try_digit = S3_NextDigit(try_index, try_digit);
            if (try_digit == -1) {
                if (S9_PrevCell(try_index, try_digit) == false) {
                    reload();
                    goto failed;
                }
                continue;
            }
            S7_TryDigit(try_index, try_digit);
            break;
        } while (true);

    } while (true);
}
void Sudoku::reload() {
    count = 0;
    for (int i = 0; i < 81; i++) {
        int d = orgDigit(i);
        bits[i] = 0;
        setOrg(i, d);
    }
    for (int i = 0; i < 81; i++) {
        int d = orgDigit(i);
        commit(i, d);
    }
}

// CLASS
//    /

Sudoku::~Sudoku() {
    dest();
}

Sudoku::Sudoku() {
    init();
}

Sudoku::Sudoku(string str)
    : Sudoku() {
    load(str.c_str());
}

Sudoku::Sudoku(const Sudoku &obj)
    : Sudoku() {
    copyOf(obj);
}

Sudoku::Sudoku(const Sudoku* obj)
    : Sudoku() {
    copyOf(*obj);
}

void Sudoku::copyOf(const Sudoku &src) {
    // 跳过自赋值
    if (this == &src) {
        return;
    }

    this->count = src.count;
    memcpy(this->bits, src.bits, 81 * sizeof(bits[0]));
    memcpy(this->datas, src.datas, 81 * sizeof(datas[0]));

    return;
}

Sudoku &Sudoku::operator=(const Sudoku &src) {
    copyOf(src);
    return *this;
}

void Sudoku::init() {
    count = 0;
    bits = new int[81]();
    datas = new CellData[81]();
}

void Sudoku::dest() {
    if (bits != nullptr) {
        delete[] bits;
    }
    if (datas != nullptr) {
        delete[] datas;
    }
}
