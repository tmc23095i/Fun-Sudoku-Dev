#include <QtGui/qevent.h>
#include <QPushButton>
#include <QSpacerItem>
#include <iostream>
#include "Widget.hpp"
#include "Cell.hpp"
#include "Panel.hpp"
#include "Sudoku.hh"
#include "Monotime.hh"

using std::cout;
using std::endl;

void Widget::freeSDKs() {
    if (sdk_origin != nullptr) {
        delete sdk_origin;
    }
    if (sdk_playing != nullptr) {
        delete sdk_playing;
    }
    if (sdk_answers != nullptr) {
        delete sdk_answers;
    }
}

void Widget::refreshPanel() {
    if (panel == nullptr) {
        return;
    }
    panel->setCells(sdk_string);
    panel->refresh();
}

void Widget::makeEmptySDKs() {
    freeSDKs();
    sdk_origin = new Sudoku();
    sdk_playing = new Sudoku();
    sdk_answers = new Sudoku();
    sdk_origin->toString(sdk_string, ' ');

    refreshPanel();
}

bool Widget::makeRandomSDK() {
    Sudoku* making = new Sudoku(Sudoku::random());
    int times_limit = 10;
    while (true) {
        if (times_limit-- == 0) {
            delete making;
            return false;
        }
        if (making->makeGame() == true) {
            break;
        }
    }

    freeSDKs();
    making->toString(sdk_string, ' ');
    sdk_origin = new Sudoku(making->toString());
    delete making;
    sdk_answers = new Sudoku(sdk_origin);
    sdk_answers->crack();
    sdk_playing = new Sudoku(sdk_origin);

    refreshPanel();
    return true;
}

QSize Widget::sizeToScreenRatio(float ratio) {
    if (ratio <= 0.0 || ratio > 1.0) {
        return QSize(10, 10);
    }
    QScreen* screen = QGuiApplication::primaryScreen();
    if (! screen) {
        return QSize(10, 10);
    }

    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    int height = screenGeometry.height();

    return QSize(width * ratio, height * ratio);
}

void Widget::makeMenu() {
    QFrame* menu = new QFrame(this);
    lout_main->addWidget(menu, 0, 1);
    menu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lout_main->setColumnStretch(1, 2);

    lout_menu = new QVBoxLayout(menu);

    QPushButton* btn_random = new QPushButton("Generate", this);
    btn_random->setDefault(false);
    btn_random->setFocusPolicy(Qt::NoFocus);
    QObject::connect(btn_random, &QPushButton::clicked, [this]() {
        makeRandomSDK();
    });
    lout_menu->addWidget(btn_random);

    QPushButton* btn_solve = new QPushButton("Solve", this);
    btn_solve->setDefault(false);
    btn_solve->setFocusPolicy(Qt::NoFocus);
    QObject::connect(btn_solve, &QPushButton::clicked, [this]() {
        if (sdk_answers->crack() == true) {
            panel->setCells(sdk_answers->toString(' ').c_str());
        } else {
            panel->setCells(sdk_string);
        }
    });
    lout_menu->addWidget(btn_solve);

    QPushButton* btn_check = new QPushButton("Check", this);
    btn_check->setDefault(false);
    btn_check->setFocusPolicy(Qt::NoFocus);
    QObject::connect(btn_check, &QPushButton::clicked, [this]() {
        Sudoku ans(panel->getCells('0'));
        if (sdk_answers->checkAnswer(&ans) == true) {
            panel->setSucceedStyle();
        } else {
            panel->setFailedStyle();
        }
    });
    lout_menu->addWidget(btn_check);

    QPushButton* btn_clear = new QPushButton("Clear", this);
    btn_clear->setDefault(false);
    btn_clear->setFocusPolicy(Qt::NoFocus);
    QObject::connect(btn_clear, &QPushButton::clicked, [this]() {
        panel->setCells(sdk_string);
        panel->refresh();
    });
    lout_menu->addWidget(btn_clear);

    QPushButton* btn_resize = new QPushButton("Check", this);
    btn_resize->setDefault(false);
    btn_resize->setFocusPolicy(Qt::NoFocus);
    QObject::connect(btn_resize, &QPushButton::clicked, [this]() {
    });
    lout_menu->addWidget(btn_resize);

    // QPushButton* btn = new QPushButton("Check", this);
    // btn->setDefault(false);
    // btn->setFocusPolicy(Qt::NoFocus);
    // QObject::connect(btn, &QPushButton::clicked, [this]() {
    // });
    // lout_menu->addWidget(btn);
}

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , sdk_origin(nullptr)
    , sdk_playing(nullptr)
    , sdk_answers(nullptr)
    , panel(nullptr)
    , lastWheel(0) {
    sdk_string = new char[82];
    makeEmptySDKs();

    setWindowTitle("Fun Sudoku");

    lout_main = new QGridLayout(this);

    panel = new Panel(sdk_string, this);
    lout_main->addWidget(panel, 0, 0);
    lout_main->setColumnStretch(0, 9);

    makeMenu();

    setFocusPolicy(Qt::StrongFocus);
    // setMouseTracking(true);
}

void Widget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (panel == nullptr) {
        return;
    }
    int index = panel->mouseAt();
    if (index == -1) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        panel->setCell(index, ' ');
    }
}

void Widget::wheelEvent(QWheelEvent* event) { //==== 根据输入频率改变数字而非时间间隔
    int CD = 90;
    if (panel == nullptr) {
        return;
    }
    int index = panel->mouseAt();
    if (index == -1) {
        return;
    }

    clock_t t = mstime();
    cout << t << endl;
    if ((t - lastWheel) < CD) {
        return;
    }
    lastWheel = t;

    int D = event->angleDelta().y();
    cout << D << endl;
    if (D > 0) { // ↑ 小数字
        int d = panel->getCell(index, '0') - '0' - 1;
        if (d < 1) {
            d = 1;
        }
        d += '0';
        panel->setCell(index, d);
    }
    if (D < 0) { // ↓ 大数字
        int d = panel->getCell(index, '0') - '0' + 1;
        if (d > 9) {
            d = 9;
        }
        d += '0';
        panel->setCell(index, d);
    }
}

Widget::~Widget() {
    freeSDKs();
    delete sdk_string;
}

void Widget::findOutMinSize() {
    QSize last_sz = size();
    QSize D(1, 1);
    while (true) {
        QSize now_sz = size() - D;
        resize(now_sz);
        if (now_sz == last_sz) {
            break;
        }
        last_sz = now_sz;
        continue;
    }

    panel->fixSquare(D, false);
    D += panel->size();
    panel->setMinSize(D.width());
}

void Widget::showedThenDo() {
    findOutMinSize();
    resize(sizeToScreenRatio(0.5));
}

void Widget::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
}

void Widget::keyPressEvent(QKeyEvent* event) {
    int cell = panel->lastGazed();

    std::cout << event->key() << " @ " << cell << std::endl;
    { // keyboard: numb0 ~ numb9 to set digit of the gazing cell
        if (cell != -1) {
            auto numb = [&event]() {
                int d = -1;
                int key = event->key();
                if (key == Qt::Key_0) {
                    d = 0;
                }
                if (key == Qt::Key_1) {
                    d = 1;
                }
                if (key == Qt::Key_2) {
                    d = 2;
                }
                if (key == Qt::Key_3) {
                    d = 3;
                }
                if (key == Qt::Key_4) {
                    d = 4;
                }
                if (key == Qt::Key_5) {
                    d = 5;
                }
                if (key == Qt::Key_6) {
                    d = 6;
                }
                if (key == Qt::Key_7) {
                    d = 7;
                }
                if (key == Qt::Key_8) {
                    d = 8;
                }
                if (key == Qt::Key_9) {
                    d = 9;
                }
                return d;
            };

            playingCommit(cell, numb());
        }
    }
    {
        int newi = cell;
        if (newi == -1) {
            newi = 0;
        } else {
            int key = event->key();
            if (key == Qt::Key_W || key == Qt::Key_Up) {
                if (cell < 9) {
                    newi += 81;
                }
                newi -= 9;
            }
            if (key == Qt::Key_S || key == Qt::Key_Down) {
                if (cell > 72) {
                    newi -= 81;
                }
                newi += 9;
            }

            if (key == Qt::Key_A || key == Qt::Key_Left) {
                if (cell % 9 == 0) {
                    newi += 9;
                }
                newi -= 1;
            }
            if (key == Qt::Key_D || key == Qt::Key_Right) {
                if (cell % 9 == 8) {
                    newi -= 9;
                }
                newi += 1;
            }
        }
        panel->gazeCell(newi);
    }
}

void Widget::playingCommit(int i, int d) {
    if (ifFixedCell(i)) {
        return;
    }
    if (d < 0 || 9 < d) {
        return;
    }

    if (d == 0) {
        sdk_playing->uncommit(i);
        panel->setCell(i, ' ');
    } else {
        sdk_playing->commit(i, d);
        panel->setCell(i, d + '0');
    }
}

void Widget::resizeEvent(QResizeEvent* event) {
    static bool SKIP = false;

    int dw = event->size().width() - event->oldSize().width();
    int dh = event->size().height() - event->oldSize().height();
    double direction = atan2(-dh, dw) * 180.0 / M_PI + 180;
    if (SKIP) {
        cout << time(nullptr) << ": " << "[" << dw << "," << dh << "]" << " " << direction << "°  |  " << event->oldSize().width() << " , " << event->oldSize().height() << "  |  " << event->size().width() << " , " << event->size().height();
        cout << " ==SKIP== \n\n\n";
        SKIP = false;

        QWidget::resizeEvent(event);
        return;
    }
    cout << time(nullptr) << ": " << "[" << dw << "," << dh << "]" << " " << direction << "°  |  " << event->oldSize().width() << " , " << event->oldSize().height() << "  |  " << event->size().width() << " , " << event->size().height();

    QSize adjust;
    if (270 <= direction && direction <= 360) { // 缩小
        panel->fixSquare(adjust, true);
    } else if (90 <= direction && direction <= 180) { // 放大
        panel->fixSquare(adjust, false);
    } else {
        cout << "\n\n\n";
        SKIP = true;
        resize(event->oldSize());
        return;
    }
    cout << " (" << adjust.width() << " , " << adjust.height() << ") \n\n\n";
    SKIP = true;
    resize(size() + adjust);
}

bool Widget::ifFixedCell(int i) {
    return (1 <= sdk_string[i] && sdk_string[i] <= 9);
}