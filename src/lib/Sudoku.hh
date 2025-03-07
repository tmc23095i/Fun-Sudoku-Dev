#ifndef __SUDOKU_HH
#define __SUDOKU_HH

#include <string>

class Sudoku {
public:
    // 获取格子所在的行(r)、列(c)、块(b)索引
    inline static void i2rcb(int i, int &r, int &c, int &b) {
        r = i / 9;
        c = i % 9;
        b = r / 3 * 3 + c / 3;
    };
    // 获取格子所在列(x)、行(y)、块(z)的首个格子索引
    inline static void i2fi(int i, int &x, int &y, int &z) {
        i2rcb(i, y, x, z);
        y *= 9;
        z = z % 3 * 3 + z / 3 * 27;
    };
    // 获取指定块的首个格子索引
    static int zAtB(int b) {
        const static int bi[9] = {0, 3, 6, 27, 30, 33, 54, 57, 60};
        return bi[b];
    };
    // 获取块内指定格子的索引, 参数b是块的索引, 参数bi是格子在块内的索引, 返回的是格子在数独中的索引
    static int iAtB(int b, int bi) {
        const static int offs[9] = {
            0,
            0 + 1,
            0 + 1 + 1,
            0 + 1 + 1 + 7,
            0 + 1 + 1 + 7 + 1,
            0 + 1 + 1 + 7 + 1 + 1,
            0 + 1 + 1 + 7 + 1 + 1 + 7,
            0 + 1 + 1 + 7 + 1 + 1 + 7 + 1,
            0 + 1 + 1 + 7 + 1 + 1 + 7 + 1 + 1};
        return (zAtB(b) + offs[bi]);
    };
public:
    bool checkAnswer(const Sudoku* a);
    // 空数独时返回真
    // 已提交的格子数为0来判断
    inline bool empty() {
        return count == 0;
    }

    // 检查格子是否全部提交, 完整数独时返回真, 否则假
    inline bool isFull() {
        return (count == 81);
    };

    // 生成一个随机数独并返回
    static Sudoku random();

    // 使用当前的数独, 生成一个随机的题目, 可以指定要置空的格子数量
    // 生成失败时, 如数独无解、此数独情况苛刻时, 返回假, 否则返回真
    // 返回真时, 当前数独将变成题目, 返回假时, 此函数不会污染当前数独
    bool makeGame();

    // 从字符串加载数独, 失败返回假
    bool load(const char* str);

    // 获取指定格子的数字
    inline int digit(int index) const {
        return (bits[index] & DIGIT_MASK) >> DIGIT_SHIFT;
    };

    // 为指定格子提交数字, 如果此数字已经提交则直接返回真, 如果此数字不可用则失败并返回假, 否则返回真并标记此数字在同行、列、块的其他格子中不可用
    bool commit(int index, int digit);
    void uncommit(int index);

    //  尝试解答, 发现答案时返回真, 无解或尝试完毕时返回假
    //  首次调用便返回假的是无解数独, 首次真后续假的是尝试结束
    //  得到答案时, 当前数独便是答案
    //  尝试完毕时, 数独会成原始数独, 之后再次调用此函数会重新开始尝试解答
    //  无解时, 当前数独是填充后的数独
    bool crack();
    bool crack_bp();

    // 获取美丽字串(以图形方式展示当前数独的字串), 用来接受字串的缓冲区大小最少要521个char, 可以指定一个格子来突出显示以及突出样式
    // 要突出显示的格子索引默认是-1, 即不启用突出显示
    // 突出样式的索引分别代表“<[(>_*|-=~”, 默认为0, 样式如: <9>
    void toPrettyString(char* result, int gazed_index = -1, int gazed_style = 0);
    std::string toPrettyString(int gazed_index = -1, int gazed_style = 0);

    // 生成当前数独的字串, 包含所有81个数字以及空止符, 没有数字的以0表示
    void toString(char str[82], char zero = '0') const;
    std::string toString(char zero = '0') const;

private:
    // 取消提交
    // 注意: 此函数未经测试, 极大概率出错, 可先暂时通过其他函数的配合完成类似的目的
    void _uncommit(int index);
    // 检查数字对于指定格子是否可用, 可用返回真, 否则假
    inline bool isDigitAvailable(int index, int digit) {
        // return (((bits[index] >> DIGIT_BITC << 1 >> digit) & 1) == 0);
        return isMarked(index, digit) == false;
    };

    // 检查指定格子的指定数字是否被标记为禁用
    inline bool isMarked(int index, int digit) {
        return ((bits[index] & MARKLIST_MASK) >> MARKLIST_SHIFT << 1 >> digit & 1) == 1;
    };

    // 探测所有格子可提交的数字, 并返回数独是否合法
    // 参数用来接受此索引处格子所探测到的可提交数字(须保证能容纳82个char, 若不需要可以提供nullptr, 最后一个元素意味探测到的数量(-1意味数独非法)), 0意味未探测到, -1意味探测到两个不同的可提交数字(非法数独)并返回假
    // 另一个可选参数用来指定, 如果数独合法, 是否提交这些探测到的数字
    // 注意: 此函数始终写入传入的参数数组, 即使数独非法也不会恢复它, 可通过(返回值==假)或(数组[81]==-1)来判断数独非法且传入的数组已被污染
    bool detect(char found[82], bool willCommit = false);

    // 探测可提交的格子, 同时检查数独合法性, 非法时返回假
    // 另外必须提供82char大小的数组, 接受探测到的索引和数字, 最后一个元素[81]是探测到的数量
    // 注意: 当函数返回假时, 数组数据将不可靠
    // 可选指定探测方式: 1是标记列表找唯一可用数字, 效率高; 2是确认同组格子都不可用某数字, 效率一般; 其他任意数字是同时使用两种方式
    // 注意: 指定一种探测方式时, 探测到的将不是所有可提交格子, 只有两种方式都使用时探测到的才是完整的; 在允许提交的情况下, 多次方式1且提交, 最后方式2并提交, 这是性能最高的方式
    bool detectBy(int plan, char found[82]);

    // 不断提交所有探测到的数字, 发现数独非法时直接返回-1, 否则返回提交的数量
    bool fill();

    // 获取与指定格子相冲突的同行、列、块格子, 返回冲突的格子数, 参数输出冲突格子索引的数组
    // 注意: 此函数未经测试, 极大概率出错, 可先暂时通过其他函数的配合完成类似的目的
    int getConflicts(int index, int conflicts[8 + 8 + 4]);

    // 恢复数独到首次加载时的状态(构造或load()后的数独)
    void reload();

private:
    // 清空当前数独的所有数据
    void clear();
    // 输出原始数字字串
    void orgString(char str[82]);
    // 检查同组格子是否已有此数字, 占用时返回真, 否则假
    // 此函数检查同组格子的数字数据, 而不是自身的标记数据, 因此使用在标记无法参考的场合, 如取消提交
    // 注意: 此函数未经测试, 极大概率出错, 可先暂时通过其他函数的配合完成类似的目的
    bool digitOccurredInMate(int index, int digit);
    // 判断指定格子在同组中, 是否是唯一可用指定数字的格子, 是则返回真
    // 当同组的其他格子对于指定的数字都不可用时返回真, 任意一个同组的格子可用此数字时返回假
    // 注意: 此函数未经测试, 极大概率出错, 可先暂时通过其他函数的配合完成类似的目的
    bool allMatesUnavailable(int index, int digit);
    // 检查格子是否有效, 如果所有数字对于这个格子都不可用, 则意为无效并返回真
    inline bool isMarkFull(int index) {
        return ((bits[index] & MARKLIST_MASK) == MARKLIST_MASK);
    };
    // 检查指定格子是否可以提交数字, 可以则返回该数字, 否则返回0
    int isMarkSole(int index);
    // 提供合法的可提交数字的数组, 当元素为1~9时为此索引处的格子提交这个数字, 并标记同组格子此数字不可用, 返回提交的数量
    int commit(char digits[81]);


public:
    void init();
    void dest();
    Sudoku();
    Sudoku(std::string str);
    // 新建一个现有数独对象的副本
    Sudoku(const Sudoku &obj);
    Sudoku(const Sudoku* obj);
    Sudoku &operator=(const Sudoku &src);
    ~Sudoku();


private:
    // 从一个现有数独拷贝完全一样的数据过来
    void copyOf(const Sudoku &src);
    int* bits;                                                                 // 数独数据: 'mmmm'mmmm'mRRR'Rttt'tttt'tttt'oooo'dddd
    int count;                                                                 // 已提交的格子数量
    constexpr static int DIGIT_BITC = 4;                                       // 已提交数字的位宽
    constexpr static int DIGIT_SHIFT = 0;                                      // 已提交数字的偏移
    constexpr static int DIGIT_MASK = 0b0'1111;                                // 已提交数字的掩码
    constexpr static int ORG_BITC = 4;                                         // 原始数独位宽
    constexpr static int ORG_SHIFT = DIGIT_SHIFT + DIGIT_BITC;                 // 原始数独偏移
    constexpr static int ORG_MASK = 0b0'1111 << ORG_SHIFT;                     // 原始数独掩码
    constexpr static int TRYING_BITC = 7 + 4;                                  // 尝试数据, 7位尝试索引+4位尝试数字, [0]除外, 它是正在尝试的个数
    constexpr static int TRYING_SHIFT = ORG_SHIFT + ORG_BITC;                  // 尝试数据偏移
    constexpr static int TRYING_MASK = 0b0'0111'1111'1111 << TRYING_SHIFT;     // 尝试数据掩码
    constexpr static int MARKLIST_BITC = 9;                                    // 标记列表的位宽
    constexpr static int MARKLIST_SHIFT = sizeof(bits[0]) * 8 - MARKLIST_BITC; // 标记列表的偏移
    constexpr static int MARKLIST_MASK = 0b1'1111'1111 << MARKLIST_SHIFT;      // 标记列表的掩码, 每一位分别表示此格子的一个数字占用情况, 0为可用, 1为不可用
    // 设置指定格子的数字数据
    static constexpr int SOLEBIT[]{
        0b0'0000'0000,
        0b0'0000'0001,
        0b0'0000'0010,
        0b0'0000'0100,
        0b0'0000'1000,
        0b0'0001'0000,
        0b0'0010'0000,
        0b0'0100'0000,
        0b0'1000'0000,
        0b1'0000'0000,
    };
    inline void setDigit(int index, int digit) {
        bits[index] &= ~DIGIT_MASK;
        bits[index] |= digit << DIGIT_SHIFT & DIGIT_MASK;
    }
    // 获取指定格子的标记列表
    inline unsigned int marklist(int index) {
        return (bits[index] & MARKLIST_MASK) >> MARKLIST_SHIFT;
    };
    // 设置指定格子的标记列表
    void setMarklist(int index, int ml) {
        bits[index] &= ~MARKLIST_MASK;
        bits[index] |= ml << MARKLIST_SHIFT & MARKLIST_MASK;
    };
    // 标记格子的指定的数字为不可用状态
    inline void mark(int index, int digit) {
        bits[index] |= 0b1 << digit >> 1 << MARKLIST_SHIFT;
    };
    // 标记格子的指定数字为可用状态
    inline void unmark(int index, int digit) {
        bits[index] &= ~(0b1 << digit >> 1 << MARKLIST_SHIFT);
    };
    // 获取指定格子的原始数字
    inline int orgDigit(int index) {
        return (bits[index] & ORG_MASK) >> ORG_SHIFT;
    }
    // 获取指定格子的原始数字
    inline void setOrg(int index, int digit) {
        bits[index] &= ~ORG_MASK;
        bits[index] |= digit << ORG_SHIFT & ORG_MASK;
    }
    // 从trying数组中获取一次尝试, 索引从1开始
    void trying(int index, int &tryIndex, int &tryDigit) {
        int trying = (bits[index] & TRYING_MASK) >> TRYING_SHIFT;
        tryIndex = trying >> 4;
        tryDigit = trying & 0b1111;
    }
    // 获取trying数组中的元素个数
    inline int tryingCount() {
        return (bits[0] & TRYING_MASK) >> TRYING_SHIFT;
    }
    // 在trying数组中指定的位置设置一个尝试, 索引从1开始, 因为[0]是尝试的个数
    inline void setTrying(int index, int tryIndex, int tryDigit) {
        int trying = (tryIndex & 0b111'1111) << 4 | (tryDigit & 0b1111);
        bits[index] &= ~TRYING_MASK;
        bits[index] |= trying << TRYING_SHIFT & TRYING_MASK;
    }
    // 向后添加一个新的尝试
    void pushTrying(int i, int d) {
        int n = tryingCount() + 1;
        setTrying(n, i, d);
        setTryingCount(n);
    }
    // 设置trying的元素个数, 即尝试个数(尝试进度)
    inline void setTryingCount(int cnt) {
        bits[0] &= ~TRYING_MASK;
        bits[0] |= cnt << TRYING_SHIFT & TRYING_MASK;
    }

private:
    // ...计划使用如下联合体重构数独数据, 以便内部使用更清晰明了的接口
    union CellData {
    private:
        int raw;
        struct {
            int ds_1: 1;
            int ds_2: 1;
            int ds_3: 1;
            int ds_4: 1;
            int ds_5: 1;
            int ds_6: 1;
            int ds_7: 1;
            int ds_8: 1;
            int ds_9: 1;
            int reserve: 4;
            int _digit: 4;
            int _origin: 4;
            int tryd: 4;
            int tryi: 7;
        };
    public:
        CellData() {
            raw = 0;
        }
        // 获取指定格子的数字
        inline int digit() {
            return _digit;
        }
        inline void setDigit(int digit) {
            _digit = digit;
        }
        // 获取指定格子的原始数字
        inline int origin() {
            return _origin;
        }
        inline void setOrigin(int digit) {
            _origin = digit;
        }
        // 获取此格子的某数字标记状态
        inline bool marked(int digit) {
            return raw >> digit-- == 0b1;
        }
        // 标记格子的指定的数字为不可用状态
        inline void mark(int digit) {
            raw |= 1 << digit >> 1; // 注意: 此代码受编译器内存布局影响, 如果程序异常, 改用下方通用代码
            {
                // 通用代码
                // switch (digit) {
                // case 1: ds_1 = true; break;
                // case 2: ds_2 = true; break;
                // case 3: ds_3 = true; break;
                // case 4: ds_4 = true; break;
                // case 5: ds_5 = true; break;
                // case 6: ds_6 = true; break;
                // case 7: ds_7 = true; break;
                // case 8: ds_8 = true; break;
                // case 9: ds_9 = true; break;
                // default: return;
                // }
            };
        }
        // 标记格子的指定数字为可用状态
        inline void unmark(int digit) {
            raw = ~(1 << digit >> 1); // 注意: 此代码受编译器内存布局影响, 如果程序异常, 改用下方通用代码
            {
                // 通用代码
                // switch (digit) {
                // case 1: ds_1 = false; break;
                // case 2: ds_2 = false; break;
                // case 3: ds_3 = false; break;
                // case 4: ds_4 = false; break;
                // case 5: ds_5 = false; break;
                // case 6: ds_6 = false; break;
                // case 7: ds_7 = false; break;
                // case 8: ds_8 = false; break;
                // case 9: ds_9 = false; break;
                // default: return;
                // }
            };
        };
        // 获取指定格子的标记列表
        inline unsigned int marklist(int index) {
            int ml = raw & 0b1'1111'1111; // 注意: 此代码受编译器内存布局影响, 如果程序异常, 改用下方通用代码
            {                             // 通用代码
                                          // int ml = 0 | ds_9 << 8 | ds_8 << 7 | ds_7 << 6 | ds_6 << 5 | ds_5 << 4 | ds_4 << 3 | ds_3 << 2 | ds_2 << 1 | ds_1;
            }
            return ml;
        };
        void setMarklist(int ml) {
            raw = raw & ~0b1'1111'1111 | (ml & 0b1'1111'1111); // 注意: 此代码受编译器内存布局影响, 如果程序异常, 改用下方通用代码
            {                                                  // 通用代码
                                                               // this->ds_1 = ml & 1;
                                                               // this->ds_2 = ml >> 1 & 1;
                                                               // this->ds_3 = ml >> 2 & 1;
                                                               // this->ds_4 = ml >> 3 & 1;
                                                               // this->ds_5 = ml >> 4 & 1;
                                                               // this->ds_6 = ml >> 5 & 1;
                                                               // this->ds_7 = ml >> 6 & 1;
                                                               // this->ds_8 = ml >> 7 & 1;
                                                               // this->ds_9 = ml >> 8 & 1;
            }
        };
    };
    CellData* datas;
};

#endif