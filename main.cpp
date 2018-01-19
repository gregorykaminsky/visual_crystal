#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "mytextbrowser.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   //MainWindow *w =  MainWindowSingleton::get_main_window();
   //w->show();
    MainWindow *w = new MainWindow();
    w->show();


   //MainWindowSingleton::delete_main_window();
    return a.exec();
}

