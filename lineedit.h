#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QMenu>
#include <QContextMenuEvent>
#include <QValidator>
#include "mainwindow.h"
#include <vector>

class LineEdit : public QLineEdit
{
public:
   LineEdit(QWidget *parent) : QLineEdit(parent)
   {
        this->setFocusPolicy(Qt::ClickFocus);
   }


};


#endif   // LINEEDIT_H
