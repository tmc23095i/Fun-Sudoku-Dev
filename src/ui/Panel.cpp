#include <QMouseEvent>
#include <thread>

#include "Panel.hpp"
#include "Cell.hpp"
#include "Sudoku.hh"

void Panel::setCell(int i, char ch) {
    cells[i].setText(QString(ch));
}
char Panel::getCell(int i, char ch) {
    char d = cells[i].text().toStdString().c_str()[0];
    if (d < '1' || d > '9') {
        d = ch;
    }
    return d;
}

void Panel::setCells(const char* str) {
    if (str == nullptr) {
        return;
    }

    for (int i = 0; i < 81 && *str != 0; i++) {
        setCell(i, *str++);
    }
}

std::string Panel::getCells(char ch) {
    char str[82];
    str[81] = 0;

    for (int i = 0; i < 81; i++) {
        str[i] = getCell(i, ch);
    }
    return std::string(str);
}

void Panel::deinitCells() {
}

void Panel::setParentWindow(QWidget* w) {
    parentWindow = w;
}
bool Panel::adjustParentSize(QWidget* w) {
    if (w == nullptr) {
        w = parentWindow;
    }

    return false;
}

void Panel::resizeEvent(QResizeEvent* event) {
    int min_sz = (config->cell_size_min * 3 + config->block_spacing) * 3;
    min_sz = (min_sz > config->panel_size_min) ? min_sz : config->panel_size_min;
    setMinimumSize(min_sz, min_sz);
    //
    //     QSize old_size = event->oldSize();
    //     QSize new_size = event->size();
    //
    //     if (config->keep_square) {
    //         int sz = (new_size.width() < new_size.height()) ? new_size.width() : new_size.height();
    //         new_size = QSize(sz, sz);
    //
    //         if (new_size != old_size) {
    //             if (config->auto_adjust_parent_size && parentWindow != nullptr) {
    //                 parentWindow->resize(parentWindow->size() + new_size - old_size);
    //             } else {
    //                 resize(new_size);
    //             }
    //         }
    //     }

    QWidget::resizeEvent(event);
}

bool Panel::fixSquare(QSize &adjust, bool shrink11grow) {
    int w = size().width();
    int h = size().height();
    int s = (w < h) ? w : h;
    if (shrink11grow == false) {
        s = (w > h) ? w : h;
    }
    s = (s > config->cell_size_min) ? s : config->cell_size_min;

    QSize sz = QSize(s, s);
    if (sz == size()) {
        adjust = QSize(0, 0);
        return false;
    }

    adjust = sz - size();
    return true;
}
void Panel::leaveEvent(QEvent* event) {
    refresh();
    QWidget::leaveEvent(event);
}

Panel::Panel(const char* origins, QWidget* parent)
    : QWidget(parent) {
    config = new PanelConfig;
    config->origins_str = origins;
    parentWindow = parentWidget();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto makeCells = [this]() {
        // alloc mem for cells and panel pointer
        char* mem = new char[sizeof(NewCell) * 82];
        cells = (NewCell*)mem + 1;
        *((Panel**)cells - 1) = this; // save this pointer to cells[81] for Label to use

        // create cells
        for (int i = 0; i < 81; i++) {
            new (&cells[i]) NewCell(i, nullptr);
            cells[i].setStyleSheet(config->normal_cell_ss);
            cells[i].setChar(' ');
        }

        // create panel layout
        QGridLayout* lout_panel = new QGridLayout(this);
        lout_panel->setObjectName("QGridLayout-grid_81");
        lout_panel->setSpacing(config->block_spacing);
        lout_panel->setContentsMargins(0, 0, 0, 0);

        // create block layouts
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int b = r * 3 + c;
                // create 3x3 layout
                QGridLayout* lout_block = new QGridLayout();
                lout_block->setObjectName("QGridLayout-Block3x3");
                lout_block->setSpacing(0);
                lout_block->setContentsMargins(0, 0, 0, 0);
                // add cells to 3x3 layout
                for (int u = 0; u < 9; u++) {
                    int i = Sudoku::iAtB(b, u);
                    lout_block->addWidget(&cells[i], u / 3, u % 3);
                }

                // create 3x3 frame
                QFrame* frame_block = new QFrame(this);
                frame_block->setObjectName("QFrame-Block3x3Frame");
                frame_block->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                frame_block->setStyleSheet(config->block3x3_ss);
                frame_block->setMouseTracking(true);
                frame_block->setLayout(lout_block);       // set 3x3 layout to 3x3 frame
                lout_panel->addWidget(frame_block, r, c); // add 3x3 frame to panel
            }
        }
        return 0;
    }();
    resetStyle();
}

Panel::~Panel() {
    auto deleteCells = [this]() {
        for (int i = 0; i < 81; i++) {
            cells[i].~NewCell();
        }
        delete[] (char*)(cells - 1);
        return 0;
    }();
    delete config;
}

int Panel::lastGazed() {
    return last_gazed;
}
int Panel::mouseAt() {
    return last_gazed;
}

void Panel::setMatesStyle(int i) {
    int x, y, z;
    Sudoku::i2fi(i, x, y, z);
    for (int u = 0; u < 9; u++) {
        cells[x].setStyleSheet(config->gazing_mate_ss);
        cells[y].setStyleSheet(config->gazing_mate_ss);
        cells[z].setStyleSheet(config->gazing_mate_ss);

        x += 9;
        y += 1;
        z += (u == 2 || u == 5) ? 7 : 1;
    }
    cells[i].setStyleSheet(config->gazing_cell_ss);
}

void Panel::setOriginStyle() {
    const char* str = config->origins_str;
    if (config->origins_str == nullptr) {
        return;
    }

    int cnt = strlen(str);
    for (int i = 0; i < cnt; i++) {
        if ('0' <= str[i] && str[i] <= '9') {
            cells[i].setStyleSheet(config->origin_cell_ss);
        }
    }
}
void Panel::setSucceedStyle() {
    for (int i = 0; i < 81; i++) {
        cells[i].setStyleSheet(config->succeed_cells);
    }
}
void Panel::setFailedStyle() {
    for (int i = 0; i < 81; i++) {
        cells[i].setStyleSheet(config->failed_cells);
    }
}
void Panel::setMinSize(int sz) {
    config->panel_size_min = sz;
}

void Panel::resetStyle() {
    for (int i = 0; i < 81; i++) {
        cells[i].setStyleSheet(config->normal_cell_ss);
    }
}
void Panel::refresh() {
    resetStyle();
    setOriginStyle();
}
void Panel::gazeCell(int i) {
    last_gazed = i;
    refresh();
    setMatesStyle(i);
}
void Panel::whenEnteredCell(int index) {
    gazeCell(index);
}
void Panel::whenLeftCell(int index) {
    last_gazed = -1;
}
void Panel::whenCellKeyPressed(int index) {
}
void Panel::whenCellKeyReleased(int index) {
}

PanelConfig::PanelConfig() {
    origins_str = nullptr;

    normal_cell_ss = "color: #FFFFFF; background-color: #2C2A2F; border: 1px solid #FFFFFF;";
    gazing_cell_ss = "color: #000000; background-color: #A0BF82; border: 1px solid #FFFFFF;";
    gazing_mate_ss = "color: #FFFFFF; background-color: #589CD6; border: 1px solid #FFFFFF;";
    origin_cell_ss = "color: #FFFFFF; background-color: rgb(78, 77, 79); border: 1px solid #FFFFFF;";

    succeed_cells = "color: #FFFFFF; background-color: #B3A3F7; border: 1px solid #FFFFFF;";
    failed_cells = "color: #FFFFFF; background-color: rgb(251, 174, 180); border: 1px solid #FFFFFF;";

    block3x3_ss = "   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; border: 2px solid #FFFFFF;";
    block_spacing = 2;

    cell_size_min = 5;
    panel_size_min = 0;

    keep_square = true;
    auto_adjust_parent_size = true;
}
PanelConfig::~PanelConfig() {
}