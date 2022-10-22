/* Tarnished Pixels
 * CS3505 Spring 2022
 * Assignment 7: Sprite Editor Implementation
*/

#include "mainwindow.h"
#include "clickablelabel.h"
#include <QApplication>
#include "model.h"
#include "previewwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model m;
    PreviewWindow pw(m);
    MainWindow w(m, pw);

    w.show();
    return a.exec();
}
