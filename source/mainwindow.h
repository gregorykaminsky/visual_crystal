#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "mytextbrowser.h"
#include "header.h"
#include <QGLWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QLineEdit>
#include <vector>
#include <iterator>
#include <list>



class QAction;
class QActionGroup;
class QLabel;
class QMenu;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void append(int the_string);
    void printText(std::string the_string);
    void printFileToBrowser(std::string file_name);
    void resizeEvent(QResizeEvent* event);
    void manageSizeOfWidgets();
    QGLWidget* getMyGLWidget();
    QTextBrowser* getTextBrowser();
    QLineEdit *getLineEdit();
    QTextEdit *getTextEdit();
    void Initialize_Cell();
    void closeEvent (QCloseEvent *event);
    void Initialize_command_list();
    std::vector<string> command_list;
protected:
   // void changeEvent(QEvent *e);
    bool eventFilter(QObject* obj, QEvent *event);

private slots:
    void on_lineEdit_returnPressed();
    bool focusNextPrevChild(bool next);
   //int searchVector(vector<string> &input, string word);

private:
    Ui::MainWindow *ui;
    std::list<string>::iterator iter;
    std::list<string> command_history;
};


void line_keypress(MainWindow *w, string put);


#endif   // MAINWINDOW_H
