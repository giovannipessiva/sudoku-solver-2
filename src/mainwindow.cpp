#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    showPossibilities(true),
    ui(new Ui::MainWindow),
    lastX(-1), lastY(-1)
{
    ui->setupUi(this);

    n=3;
    n2=n*n;
    mask=1<<(n2+1);
    emptyValue=(1<<(n2+2))-1;

    cells=new Box**[n2];
    for(int i=0; i<n2; i++)
        cells[i]=new Box*[n2];

    m=new int*[n2];
    for(int i=0; i<n2; i++)
        m[i]=new int[n2];

    for(int i=0; i<n2; i++)
        for(int j=0; j<n2; j++)
        {
            m[i][j]=emptyValue;
            cells[i][j]=new Box(n, i*n2+j, emptyValue, this);
            ui->gridLayout->addWidget(cells[i][j], i, j, 0);
        }
}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i=0; i<n2; i++)
    {
        for(int j=0; j<n2; j++)
            delete cells[i][j];
        delete[] cells[i];
    }
    delete[] cells;

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::unfocusBox(int id)
{
    lastX=id/n2;
    lastY=id%n2;

    cells[lastX][lastY]->unfocus(m[lastX][lastY]);
}

void MainWindow::setValue(int id, int value)
{
    int i=id/n2;
    int j=id%n2;

    if(value<0)
        value=emptyValue;

    m[i][j]=value;

    pruner();
}

void MainWindow::refresh()
{
    for(int i=0; i<n2; i++)
        for(int j=0; j<n2; j++)
            cells[i][j]->refresh(m[i][j]);
}

void MainWindow::printMatrix()
{
    int i,j;

    for(i=0; i<n2; i++)
    {
        QString s;
        for(j=0; j<n2; j++)
        {
            if(m[i][j]>mask)
                s+=QString(". ");
            else
                s+=QString::number(m[i][j])+" ";
        }
        qDebug() <<s;
    }
    qDebug() <<"";
}

void MainWindow::on_pushButton_clicked()
{
    int result=solveRational(m,n,lastX,lastY);

    switch(result)
    {
    case -1:    changeSmile(MainWindow::error);         break;
    case 0:     changeSmile(MainWindow::unsolvable);    break;
    default:    changeSmile(MainWindow::solved);        break;
    }

    if(!result)
    {
        QMessageBox::critical(  this, QObject::tr("Sudoku non risolvibile"),
                                QObject::tr("Questo sudoku non è risolvibile usando metodi razionali;\n"
                                "potrebbe essere incompleto o contenere errori."),
                                QMessageBox::Close,
                                0);
    }
    refresh();
}

void MainWindow::on_actionNuovo_triggered()
{
    resetMatrix();
}

void MainWindow::resetMatrix()
{
    lastX=-1;
    lastY=-1;

    for(int i=0; i<n2; i++)
        for(int j=0; j<n2; j++)
        {
            m[i][j]=emptyValue;
            cells[i][j]->refresh(emptyValue);
        }
    changeSmile(MainWindow::unsolvable);
}

void MainWindow::on_actionApri_triggered()
{
    QFileDialog fileSelector;
    fileSelector.setWindowTitle("Apri un  sudoku");
    fileSelector.setFileMode(QFileDialog::ExistingFile);
    fileSelector.setViewMode(QFileDialog::Detail);
    fileSelector.setWindowIcon(QIcon(":/icon.png"));

    fileSelector.setLabelText(QFileDialog::LookIn,"Cerca in:");
    fileSelector.setLabelText(QFileDialog::FileName,"Nome:");
    fileSelector.setLabelText(QFileDialog::FileType,"");
    fileSelector.setLabelText(QFileDialog::Accept,"Apri");
    fileSelector.setLabelText(QFileDialog::Reject,"Annulla");

    fileSelector.exec();

    QStringList fileName = fileSelector.selectedFiles();
    QFile *file=new QFile(fileName.join(""));


    if (!file->open(QFile::ReadOnly | QFile::Text))
    {
        std::cerr << "Error: cannot read file" << std::endl;
        std::cerr << qPrintable(file->errorString()) << std::endl;
        return;

    }else{
        //read first line (matrix base, from 1 to 5)
        QString *tmp=getLine(file);
        tmp->replace(" ","");
        int num=tmp->toInt();
        if(num!=3)
        {
                qDebug() <<"Error detected in the file: can't manage a matrix with base: " <<num;
                file->close();
                return;
        }

        n=num;
        n2=n*n;
        mask=1<<(n2+1);   //position of the "flags's flag" (to indicate it's a int of flag, not a number)


        if(m)
        {
            for(int i=0; i<n2; i++)
                delete[] m[i];
            delete m;
        }


        m=new int*[n2];
        for(int i=0; i<n2; i++)
            m[i]=new int[n2];


        for(int i=0; i<n2; i++)
        {
            tmp=getLine(file);

            QStringList list=tmp->split(DELIM);

            if(list.size()<n2)
            {
                qDebug() <<"error while reading file: invalid line " <<i;
                file->close();
                resetMatrix();
                return;
            }

            for(int j=0; j<n2; j++)
            {
                if(list[j]==EMPTY)
                    m[i][j]=emptyValue;
                else{
                    if(list[j].toInt()!=0)
                        m[i][j]=list[j].toInt();
                    else{
                        list[j]=((QString) list[j]).replace("\n","");

                        //riprova; questa volta se non funge esci
                        if(list[j]==EMPTY)
                            m[i][j]=emptyValue;
                        else{
                            if(list[j].toInt()!=0)
                                m[i][j]=list[j].toInt();
                            else{
                                qDebug() <<"error while reading file: invalid value " <<list[j];
                                file->close();
                                resetMatrix();
                                return;
                            }
                        }
                    }
                }

                cells[i][j]->refresh(m[i][j]);
            }
        }
        file->close();

        lastX=-1;
        lastY=-1;

        pruner();
    }
}

QString *MainWindow::getLine(QFile *file)
{
    char buf[128];

    qint64 lineLength = file->readLine(buf, sizeof(buf));
    if (lineLength != -1)
        return new QString(buf);
    else
        return NULL;
}

void MainWindow::on_actionEsci_triggered()
{
    if(m)
    {
        for(int i=0; i<n2; i++)
            delete[] m[i];
        delete m;
    }
    this->close();
}

void MainWindow::on_actionAbout_SudokuSolver2_triggered()
{
    About a;
    a.exec();
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        showPossibilities=true;
        ui->radioButton_base->setEnabled(true);
        ui->radioButton_avanzato->setEnabled(true);

    }else{
        showPossibilities=false;
        ui->radioButton_base->setEnabled(false);
        ui->radioButton_avanzato->setEnabled(false);

    }

    refresh();
}

void MainWindow::on_radioButton_base_toggled(bool checked)
{
    Q_UNUSED(checked)
    pruner();
}

void MainWindow::on_actionSalva_triggered()
{

    QFileDialog fileSelector;
    fileSelector.setFileMode(QFileDialog::AnyFile);
    fileSelector.setViewMode(QFileDialog::Detail);
    fileSelector.setWindowIcon(QIcon(":/icon.png"));


    fileSelector.setWindowTitle("Salva il sudoku");
    fileSelector.setLabelText(QFileDialog::LookIn,"Salva in:");
    fileSelector.setLabelText(QFileDialog::FileName,"Nome:");
    fileSelector.setLabelText(QFileDialog::FileType,"");
    fileSelector.setLabelText(QFileDialog::Accept,"Salva");
    fileSelector.setLabelText(QFileDialog::Reject,"Annulla");

    fileSelector.exec();

    QStringList fileName = fileSelector.selectedFiles();
    QFile *file=new QFile(fileName.join(""));


    if (!file->open(QFile::WriteOnly | QFile::Text))
    {
        std::cerr << "Error: cannot write file" << std::endl;
        std::cerr << qPrintable(file->errorString()) << std::endl;
        return;

    }else{
        QTextStream out(file);

        out <<n <<endl;

        for(int i=0; i<n2; i++)
        {
            for(int j=0; j<n2; j++)
            {
                if(m[i][j]>mask)
                    out <<EMPTY;
                else
                    out <<m[i][j];

                out <<DELIM;
            }
            out <<endl;
        }

        file->close();

    }
}

void MainWindow::changeSmile(smile s)
{
    switch(s)
    {
    case MainWindow::solved:
        ui->face->setText("<img src=':/state1.png'' />");
        break;
    case MainWindow::solvable:
        ui->face->setText("<img src=':/state2.png'' />");
        break;
    case MainWindow::error:
        ui->face->setText("<img src=':/state3.png'' />");
        break;
    case MainWindow::unsolvable:
        ui->face->setText("<img src=':/state4.png' />");
        break;
    }
}

void MainWindow::pruner()
{
    int tier=PRUNE_BAS;
    if(ui->radioButton_avanzato->isChecked())
        tier=PRUNE_ADV;

    switch(prune(m,n,tier,lastX,lastY))
    {
    case -1:  changeSmile(MainWindow::error);         break;
    case 0:   changeSmile(MainWindow::unsolvable);    break;
    case 1:   changeSmile(MainWindow::solvable);      break;
    case 2:   changeSmile(MainWindow::solved);        break;
    default:    qDebug() <<"prune returned an invalid value" <<endl; break;
    }

    refresh();
}
