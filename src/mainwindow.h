#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "box.h"
#include "about.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <iostream>
#include <QString>
#include <QStringList>
#include <QMessageBox>


#define EMPTY "."   //set the char to identify "unknown value" in the file
#define DELIM " "   //set the string used to separate values in the file

#define PRUNE_TOT 1
#define PRUNE_ADV 2
#define PRUNE_MED 3
#define PRUNE_BAS 4


extern "C"
{
    int prune(int **matrix,int num,int tier,int lastX,int lastY);
    int solveRational(int **matrix,int num,int lastX,int lastY);
    int solveBruteForce(int **matrix,int num);
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int n;
    int n2;
    int mask;   //1<<n2+1
    int emptyValue; //(1<<(n2+2))-1
    bool showPossibilities;

    void unfocusBox(int id);
    void setValue(int id, int value);


private:
    Ui::MainWindow *ui;
    Box ***cells;   //matrix of Box*
    int **m;    //sudoku matrix
    int lastX;
    int lastY;

    enum smile
    {
        solved,
        solvable,
        error,
        unsolvable
    };

    void pruner();
    void refresh();
    void printMatrix();
    QString *getLine(QFile *file);
    void resetMatrix();
    void changeSmile(smile s);


private slots:
    void on_actionSalva_triggered();
    void on_radioButton_base_toggled(bool checked);
    void on_checkBox_toggled(bool checked);
    void on_actionAbout_SudokuSolver2_triggered();
    void on_actionEsci_triggered();
    void on_actionApri_triggered();
    void on_actionNuovo_triggered();
    void on_pushButton_clicked();
    void on_actionAbout_Qt_triggered();
};

#endif // MAINWINDOW_H
