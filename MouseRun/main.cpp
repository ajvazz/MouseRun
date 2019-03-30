#include <QApplication>

#include "game.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Controller controller;

    return a.exec();
}
