#include "box.h"
#include "mainwindow.h"

int Box::s_newSelected=-1;
int Box::s_oldSelected=-1;
QFont Box::m_littleFont=QFont("Courier", 8);
QFont Box::m_bigFont=QFont("Courier", 32);

Box::Box(int size, int id, int value, QMainWindow *w, QWidget *parent) :
        QTextEdit(parent),
        m_size(size),
        m_id(id),
        m_window(w)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(50,50);

    QObject::connect(this, SIGNAL(  selectionChanged()),
                     this, SLOT(    focus()));

    QObject::connect(this, SIGNAL(  textChanged()),
                     this, SLOT(    change()));

    setPlainText("-");  //cant be initialized to empty value or space (no "", no " ")
    unfocus(value);

    /*
    switch(size)
    {
    case 2:
        insertPlainText("1 2\n3 4");
        setFixedSize(36,36);
        break;

    case 3:
        insertPlainText("1 2 3\n4 5 6\n7 8 9");
        setFixedSize(50,50);
        break;

    case 4:
        insertPlainText(" 1  2  3  4\n 5  6  7  8\n 9 10 11 12\n13 14 15 16");
        setFixedSize(90,90);
        break;

    case 5:
        insertPlainText(" 1  2  3  4  5\n 6  7  8  9 10\n11 12 13 14 15\n16 17 18 19 20\n21 22 23 24 25");
        setFixedSize(110,110);
        break;

    default:
        qDebug() <<"dimensione del Box non valida: " <<size;
        exit(1);
    }
    */

}

void Box::refresh(int value)
{
    //restore old value
    if(value & ((MainWindow *) m_window)->mask)
    {
        if(((MainWindow *) m_window)->showPossibilities)
        {
            setCurrentFont(m_littleFont);

            QString s;
            value & 1<<1? s+="1" : s+=" ";
            value & 1<<2? s+=" 2" : s+="  ";
            value & 1<<3? s+=" 3\n" : s+="  \n";
            value & 1<<4? s+="4" : s+=" ";
            value & 1<<5? s+=" 5" : s+="  ";
            value & 1<<6? s+=" 6\n" : s+="  \n";
            value & 1<<7? s+="7" : s+=" ";
            value & 1<<8? s+=" 8" : s+="  ";
            value & 1<<9? s+=" 9" : s+="  ";
            setPlainText(s);
            setReadOnly(true);
        }else{
            setPlainText("");
            setReadOnly(false);
        }

    }else{
        setCurrentFont(m_bigFont);
        setPlainText(QString::number(value).toAscii());
        setReadOnly(false);
    }
}



void Box::unfocus(int value)
{
    int newValue=toPlainText().toInt();

    if(newValue>=1 &&
       newValue<=((MainWindow *) m_window)->n2)
    {
        //show new value
        ((MainWindow *) m_window)->setValue(m_id, newValue);
        //setCurrentFont(big);
        //setPlainText(QString::number(newValue).toAscii());


    }else{
        /*
        setCurrentFont(little);
        setPlainText("1 2 3\n4 5 6\n7 8 9");
        */

        if(toPlainText().trimmed()=="")
        {
            //reset value
            ((MainWindow *) m_window)->setValue(m_id, -1);
        }else{

            //restore old value
            if(value & ((MainWindow *) m_window)->mask)
            {
                if(((MainWindow *) m_window)->showPossibilities)
                {
                    setCurrentFont(m_littleFont);

                    QString s;
                    value & 1<<1? s+="1" : s+=" ";
                    value & 1<<2? s+=" 2" : s+="  ";
                    value & 1<<3? s+=" 3\n" : s+="  \n";
                    value & 1<<4? s+="4" : s+=" ";
                    value & 1<<5? s+=" 5" : s+="  ";
                    value & 1<<6? s+=" 6\n" : s+="  \n";
                    value & 1<<7? s+="7" : s+=" ";
                    value & 1<<8? s+=" 8" : s+="  ";
                    value & 1<<9? s+=" 9" : s+="  ";
                    setPlainText(s);
                    setReadOnly(true);
                }

            }else{
                setCurrentFont(m_bigFont);
                setPlainText(QString::number(value).toAscii());
                setReadOnly(false);
            }
        }
    }
}

void Box::focus()
{
    s_oldSelected=s_newSelected;
    s_newSelected=m_id;

    if(s_oldSelected!=s_newSelected)
    {
        if(s_oldSelected!=-1)
            ((MainWindow *) m_window)->unfocusBox(s_oldSelected);    //cambia la vecchia cell

        if(currentFont()!=m_bigFont)
        {
            setCurrentFont(m_bigFont);
            setPlainText("");
        }

        setReadOnly(false);
    }
}

void Box::change()
{
    if(toPlainText().length()==2)
        setPlainText(toPlainText().at(0));
}
