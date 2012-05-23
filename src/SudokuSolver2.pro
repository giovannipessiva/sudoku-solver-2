#-------------------------------------------------
#
# Project created by QtCreator 2011-03-18T13:24:47
#
#-------------------------------------------------

QT       += core gui

TARGET = SudokuSolver2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Solver/solver.c \
    Solver/chkSquare.c \
    Solver/chkRow.c \
    Solver/chkFunctions.c \
    Solver/chkColumn.c \
    Solver/solverInterface.c \
    box.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    SS.h \
    Solver/header.h \
    box.h \
    about.h

FORMS    += mainwindow.ui \
    about.ui

OTHER_FILES +=

RESOURCES += \
    media.qrc

RC_FILE = icon.rc
