#-------------------------------------------------
#
# Project created by QtCreator 2016-03-26T23:59:59
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mirabo17
TEMPLATE = app

LIBS += -lGLU
#-lglut
#-lGLEW
#-lGL
#-lGLU

SOURCES += main.cpp\
        mainwindow.cpp \
        stray_math_func.cpp \
        reader_molecule.cpp \
    mouse.cpp \
    opengl_keypress.cpp \
    line_keypress.cpp \
    read_cif_files.cpp \
    AtomColorRadius.cpp \
    read_xyz_files.cpp \
    projectfile.cpp 


HEADERS  += mainwindow.h \
    myglwidget.h \
    mytextbrowser.h \
    lineedit.h \
    header.h \
    Unit_Cell.h \
    Atom.h \
    Vec3d.h \
    Geometry.h \
    Manager.h \
    Plane.h \
    Stack.h \
    Singleton.h \
    myarray.h \
    projectfile.h 

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3



FORMS    += mainwindow.ui
