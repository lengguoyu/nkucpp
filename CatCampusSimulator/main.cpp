#include "game_main_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameMainWindow w;
    w.show();

    return a.exec();
}