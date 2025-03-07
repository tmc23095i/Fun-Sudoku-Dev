#ifndef __CELL_HPP
#define __CELL_HPP

#include <QWidget>
#include <QLabel>

class Panel;
/*
class MyLabel : public QLabel {
    Q_OBJECT
public:
    char _id;
    void mouseMoveEvent(QMouseEvent* ev) override;
};

union Cell {
private:
    MyLabel label;
    Panel* _panel;
public:
    Cell();
    ~Cell();
    QLabel &qlabel();
    int id();
    void setID(int id);
    Panel* panel();
    void setPanel(Panel* p);
}; */

class NewCell : public QLabel {
    Q_OBJECT
private:
    char _id;
public:
    void resizeEvent(QResizeEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    ::Panel* panel();
    NewCell(int id = -1, QWidget* parent = nullptr);
    int id();
    void setID(int id);
    void setChar(char c);
    char getChar();
    // 给定一个全局坐标, 判断是否坐落在当前格子内
    bool check(QPointF gloPos);
    bool check(QPoint gloPos);
};

#endif