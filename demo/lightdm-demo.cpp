//
// Created by dingjing on 2022/3/3.
//

#include <QApplication>
#include "lightdm-mine.h"

using namespace graceful;

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    LightDMMine dm;

    return QApplication::exec();
}