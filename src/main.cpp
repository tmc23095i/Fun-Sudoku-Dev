#include <QApplication>
#include "Widget.hpp"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    w.showedThenDo();
    return a.exec();
}
