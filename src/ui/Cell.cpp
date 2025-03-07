
#include "Cell.hpp"
#include "Panel.hpp"

void NewCell::resizeEvent(QResizeEvent* event) {
    int sz = size().height();
    sz *= 0.75;
    QFont fn = font();
    fn.setPixelSize(sz);
    setFont(fn);
}
void NewCell::enterEvent(QEnterEvent* event) {
    ::Panel* panel = this->panel();
    panel->whenEnteredCell(_id);

    QLabel::enterEvent(event);
}
void NewCell::leaveEvent(QEvent* event) {
    ::Panel* panel = this->panel();
    panel->whenLeftCell(_id);

    QLabel::leaveEvent(event);
}

::Panel* NewCell::panel() {
    return *((::Panel**)(this - _id) - 1);
}

NewCell::NewCell(int id, QWidget* parent) {
    _id = id;
    setParent(parent);
    setObjectName("Label-CellWithPanelAndID");
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setAlignment(Qt::AlignCenter);
    setText(std::to_string(id).c_str());

    // setMouseTracking(true);
}
void NewCell::setChar(char c) {
    char s[2];
    s[0] = c;
    s[1] = 0;
    setText(s);
}

char NewCell::getChar() {
    return text().toStdString().c_str()[0];
}

int NewCell::id() {
    return _id;
}
void NewCell::setID(int id) {
    _id = id;
};

bool NewCell::check(QPointF gloPos) {
    return check(gloPos.toPoint());
}
bool NewCell::check(QPoint gloPos) {
    QPoint lt = mapToGlobal(QPoint(0, 0));
    QRect bounds(lt, size());
    return bounds.contains(gloPos, true);
}