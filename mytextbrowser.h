

#ifndef MYTEXTBROWSER_H
#define MYTEXTBROWSER_H

#include <QTextBrowser>
#include <QString>

class MyTextBrowser : public QTextBrowser
{
public:
    MyTextBrowser(QWidget *parent) : QTextBrowser(parent) {
    }

};


#endif   // MYTEXTBROWSER_H

