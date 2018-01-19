#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPoint>
#include <QColor>
#include <QPalette>
#include <QGLWidget>
#include <QTextBlock>
#include <string>
#include <stdio.h>
#include <QScrollBar>
#include <iostream>
#include <stdlib.h>
#include <QColorDialog>
#include <QCloseEvent>

#include <QFile>
#include <QTextStream>
#include <QString>

#include <sys/types.h>
#include <sys/stat.h>



   //saves the file in a folder called 'Saved_files'
   //the second entry of string array input must contain the name of the file to be saved.
bool saveAll(MainWindow *window, vector<Unit_Cell> &cell, vector<string> input);

   //opens a file with name stored in input[1], whatever was opened previously is erased
void openFile(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              string input);

/**
 * @brief tabAction when the user types in an incomplete
 *                  command and presses 'tab' in the LineEdit widget,
 *                  the program starts shifting through all the possible commands
 *                  that have the same beginning that the user indicated.
 * @param window the MainWindow pointer
 * @param tab_array the array where all the possible command strings are stored
 */
void tabAction(MainWindow *window, MyArray& tab_array);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    iter = command_history.begin();

    ui->setupUi(this);   //original
    ui->lineEdit->installEventFilter(this);

    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    manageSizeOfWidgets();

    ui->TextBrowser->ensureCursorVisible();



    ui->TextBrowser->setStyleSheet("background-color:lightblue;");
    ui->lineEdit->setStyleSheet("background-color:lightblue;");
    ui->TextEdit->setStyleSheet("background-color:lightyellow;");


    ui->TextBrowser->append("Please enter the name of the file:" );
    ui->openGLWidget->window = this;
    Initialize_Cell();
    Initialize_command_list();

}

/**
 * @brief MainWindow::focusNextPrevChild this method makes sure the tab key in LineEdit
 *                                       method works. It  disables
 *                                       tab key attempts to switch active
 *                                       widgets in MainWindow.
 * @param next
 * @return
 */
bool MainWindow::focusNextPrevChild(bool next)
{
    next = false;
    return next;
}


inline bool exists_test3 (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

void MainWindow::Initialize_Cell()
{
    Singleton& the_s = Singleton::getInstance();

    the_s.manage = Manager();
   //cell.add_atom("H", 0, 0, 0);    
   // cell.setRadii(0);

   //molecule_reader_for_xyz(cell, "LCO.xyz");

   //makeBonds(cell, "Co", "O", 2.0);
   //cell.setGeometry("Co", 5.40529, 5.40529, 12.94404,  90.0, 90.0, 120.0);
   //opens a file with name stored in input[1], whatever was opened previously is erased

    string input = "Saved_files/mirabo_autosave.txt";
    if(exists_test3(input) == false)
    {
        openFile(this, the_s.cell, the_s.manage, "LCO.cif ");
    }
    else
    {
        openFile(this, the_s.cell, the_s.manage, input);
    }
   //string input = "Saved_files/LCO.xyz";
   //string input = "Saved_files/CaTiO3.cif";

/*
        read_CIF_FILE(cell, "LCO.cif");
       makeBondsOutside(cell, "Co", "O", 2.0);

       the_s.cell.push_back(cell);
       the_s.manage.putAtom(0);
       the_s.manage.putAtom(1);
       the_s.manage.putAtom(2);
       createPlane(the_s.cell, the_s.manage);
*/
}


MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief MainWindow::printText prints the input string into the TextBrowser widget.
 * @param the_string input string
 */
void MainWindow::printText(std::string the_string)
{
    ui->TextBrowser->append(the_string.c_str());
}



/**
 * @brief MainWindow::printFileToBrowser prints an entire text file to a TextBrowser widget
 * @param file_name the name of the input file.
 */
void MainWindow::printFileToBrowser(string file_name)
{
    QFile file(file_name.c_str());
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    this->getTextBrowser()->append(content);
    return;
}




void MainWindow::append(int input)
{
    char buffer[10];
    snprintf(buffer, 10, "%d", input);
    ui->TextBrowser->append(buffer);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   manageSizeOfWidgets();
}

void MainWindow::manageSizeOfWidgets()
{
     this->layout()->setContentsMargins(0, 0, 0, 0);
    this->statusBar()->hide();
   //ui->mainToolBar->hide();
    QSize size = this->size();
    QSize linesize = ui->lineEdit->size();
    int spacing = 2;

    double hspace = 9;
    ui->openGLWidget->move(spacing, hspace);
    ui->openGLWidget->resize(size.width()*(2.0/3.0) - spacing*3, size.height() - hspace*2
                             - linesize.height());

    ui->TextBrowser->move(size.width()*(2.0/3.0) + spacing, hspace);
    ui->TextBrowser->resize(size.width()*(1.0/3.0) - spacing*2, size.height()/2.0 - hspace/2.0);
    ui->lineEdit->move(spacing,
                       size.height() - hspace*2 - linesize.height());
    ui->lineEdit->resize(size.width()*(2.0/3.0) - spacing*3, ui->lineEdit->size().height());

    ui->TextEdit->move(size.width()*2.0/3.0 + spacing, size.height()*3.0/4.0 - hspace*2);
    ui->TextEdit->resize(size.width()*(1.0/3.0) - spacing*2, size.height()*1.0/4.0 - hspace);

}

/**
 * @brief MainWindow::on_lineEdit_returnPressed controls what happens when the user
 *                                              presses enter in the lineEdit widget.
 */
void MainWindow::on_lineEdit_returnPressed()
{
    QString keep = ui->lineEdit->text();
    ui->TextBrowser->append(" ");
    ui->TextBrowser->append(keep);
    ui->lineEdit->clear();

    line_keypress(this, keep.toStdString());
    ui->TextBrowser->ensureCursorVisible();
}

void MainWindow::Initialize_command_list()
{
    ifstream read2("man_commands.txt");
    string line;
    vector<string> token;
    while(getline(read2, line))
    {
        if(line.find("line_command") == string::npos)
            continue;

        token = tokenizer(line, "= \t");
        string temp_hold = "";
        if(token.size() > 1)
        {
            for(unsigned int i = 1; i < token.size() - 1; i++)
                temp_hold = temp_hold + token[i] + " ";
            temp_hold = temp_hold + token[token.size() - 1];
            command_list.push_back(temp_hold);
        }
    }
    read2.close();

}

/**
 * @brief tabAction when the user types in an incomplete
 *                  command and presses 'tab' in the LineEdit widget,
 *                  the program starts shifting through all the possible commands
 *                  that have the same beginning that the user indicated.
 * @param window the MainWindow pointer
 * @param tab_array the array where all the possible command strings are stored
 */
void tabAction(MainWindow *window, MyArray& tab_array)
{
    QLineEdit *lineEdit = window->getLineEdit();

    if(tab_array.getAccess() == true && tab_array.size() > 0)
    {
        tab_array.increment();
        string initial = tab_array.getInitial();
        lineEdit->clear();
        lineEdit->insert(tab_array.get().c_str());
        lineEdit->setCursorPosition(initial.size());
        lineEdit->setSelection(initial.size(), tab_array.get().size());
    }
    else if(tab_array.getAccess() == false)
    {
        string input = lineEdit->text().toStdString();
        MyArray array = MyArray();
        array.put(input);


        for(unsigned int i = 0; i < window->command_list.size(); i++)
        {
            if(window->command_list[i].find(input) == 0)
                array.push_back(window->command_list[i]);
        }

        tab_array = array;
        if(tab_array.size() > 0)
        {
            lineEdit->clear();
            lineEdit->insert(tab_array.get().c_str());
            lineEdit->setCursorPosition(input.size());
            lineEdit->setSelection(input.size(), tab_array.get().size());
        }
        tab_array.access(true);
    }
}

/**
 * @brief arrowUpDownAction allows to cycle back through old commands.
 * @param lineEdit reference to the lineEdit widget
 * @param keyEvent
 * @param arrow_array the array where all the old commands entered into lineEdit are stored.
 */
void arrowUpDownAction(QLineEdit *lineEdit, QKeyEvent *keyEvent, MyArray &arrow_array)
{
    if(arrow_array.size() < 1)
        return;

    else if(keyEvent->key() == Qt::Key_Up)
    {
         lineEdit->clear();
         lineEdit->insert(arrow_array.get().c_str());
         arrow_array.decrement();
    }

    else if(keyEvent->key() == Qt::Key_Down)
    {
         lineEdit->clear();
         arrow_array.increment();
         lineEdit->insert(arrow_array.get().c_str());
    }
}

/**
 * @brief MainWindow::eventFilter controls what happens with the 'tab'
 *                                and 'up' and 'down' arrows commands int he LineEdit widget
 * @param obj
 * @param event
 * @return
 */
bool MainWindow::eventFilter(QObject* obj, QEvent *event)
{
    if (obj != ui->lineEdit || event->type() != QEvent::KeyPress)
        return false;

    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    Singleton& the_s = Singleton::getInstance();


    if(keyEvent->key() == Qt::Key_Tab)
        tabAction(this, the_s.tab_array);
    else
        the_s.tab_array.access(false);


    if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        arrowUpDownAction(ui->lineEdit, keyEvent, the_s.lineEdit_input);


    return false;
}

/**
 * @brief MainWindow::closeEvent in the event the user presses red 'x', the
 *                               image and information is saved in 'mirabo_autosave.txt' file
 * @param event
 */
void MainWindow::closeEvent (QCloseEvent *event)
{
    vector<string> input;
    input.push_back("save");
    input.push_back("Saved_files/mirabo_autosave.txt");
    Singleton& the_s = Singleton::getInstance();
    saveAll(this, the_s.cell, input);
}

/**
 * @brief MainWindow::getMyGLWidget
 * @return reference to MyGLWidget
 */
QGLWidget* MainWindow::getMyGLWidget()
{
    return ui->openGLWidget;
}

/**
 * @brief MainWindow::getTextBrowser
 * @return reference to TextBrowser widget
 */
QTextBrowser* MainWindow::getTextBrowser()
{
    return ui->TextBrowser;
}


/**
 * @brief MainWindow::getTextEdit
 * @return reference to TextEdit
 */
QTextEdit* MainWindow::getTextEdit()
{
    return ui->TextEdit;
}


/**
 * @brief MainWindow::getLineEdit
 * @return reference to LineEdit
 */
QLineEdit* MainWindow::getLineEdit()
{
    return ui->lineEdit;
}
