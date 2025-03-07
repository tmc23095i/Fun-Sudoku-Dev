#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QWidget>
#include <QLayout>

class Panel;
class Sudoku;
class Widget : public QWidget {
    Q_OBJECT
    inline bool ifFixedCell(int i);

    void findOutMinSize();

    QSize lastPanelSize;
    char* sdk_string;
    Sudoku* sdk_origin;
    Sudoku* sdk_playing;
    Sudoku* sdk_answers;
    void freeSDKs();
    void makeEmptySDKs();
    bool makeRandomSDK();
    void refreshPanel();
    QSize sizeToScreenRatio(float r);

    Panel* panel;

    QGridLayout* lout_main;
    QVBoxLayout* lout_menu;
    void makeMenu();
    clock_t lastWheel;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
public:
    void playingCommit(int i, int d);
    void showedThenDo();
    Widget(QWidget* parent = nullptr);
    ~Widget();
};
#endif // WIDGET_HPP
