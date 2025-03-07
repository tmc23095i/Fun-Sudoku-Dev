

#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QColor>

class NewCell;

class PanelConfig {
public:
    const char* origins_str; // 给定一个char数组, 数字字符1~9则表示此格子是初始即固定的, 无法被改变
    QString origin_cell_ss;  // 初始固定格子的样式
    QString normal_cell_ss;  // 格子的通常样式
    QString gazing_cell_ss;  // 鼠标位于此格子时, 应用的样式
    QString gazing_mate_ss;  // 鼠标位于此格子时, 朋友索格应用的样式
    QString succeed_cells;   // 调用setSucceedStyle()时, 所有格子应用此样式
    QString failed_cells;    // 调用setFailedStyle()时, 所有格子应用此样式
    QString block3x3_ss;     // 3x3子宫格的样式, 主要用于设置其边框宽度
    int block_spacing;       // 3x3子宫格之间的空隙大小
    int cell_size_min;       // 每个格子的最小尺寸
    int panel_size_min;
    bool keep_square;             // 是否保持格子是正方形
    bool auto_adjust_parent_size; // 当矫正格子尺寸为方形时, 是否也矫正父窗口的尺寸

    PanelConfig();
    ~PanelConfig();
};

class Panel : public QWidget {
    Q_OBJECT
    friend NewCell;
public:
    explicit Panel(const char* origins, QWidget* parent = nullptr);
    ~Panel();
    void refresh();

    void setSucceedStyle();
    void setFailedStyle();

    // 返回当前鼠标所在格子的索引, 不在时返回-1
    // 非遍历算法, 而是直接将当前鼠标与最后一次所在格子比较, 所以性能不错
    int mouseAt();
    void setCell(int i, char ch);
    char getCell(int i, char ch = ' ');
    void setCells(const char* str);
    std::string getCells(char ch = ' ');

    void setMinSize(int sz);
    bool fixSquare(QSize &adjust, bool shrink11grow = true);
    void setParentWindow(QWidget* w);
    bool adjustParentSize(QWidget* w = nullptr);
    PanelConfig* config;

    void gazeCell(int i);
    int lastGazed();
private:
    QWidget* parentWindow;
    void resizeEvent(QResizeEvent* event) override;
    void leaveEvent(QEvent* event) override;
    virtual void whenEnteredCell(int index);
    virtual void whenLeftCell(int index);
    virtual void whenCellKeyPressed(int index);  //==...
    virtual void whenCellKeyReleased(int index); //==...

    void initCells();
    void deinitCells();
    NewCell* cells;
    int last_gazed;

    void initStyle();
    void deinitStyle();
    void setMatesStyle(int i);
    void setOriginStyle();
    void resetStyle();
    void setStyle(int i);

    // int cell_border;
    // int block_border;
};

#endif // SUDOKUUI_H