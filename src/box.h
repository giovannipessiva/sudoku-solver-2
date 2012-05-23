#ifndef BOX_H
#define BOX_H

#include <QMainWindow>
#include <QTextEdit>
#include <QDebug>
#include <QFont>

class Box : public QTextEdit
{
    Q_OBJECT

public:
    Box(int m_size, int m_id, int value, QMainWindow *m_window, QWidget *parent = 0);
    void unfocus(int value);
    void refresh(int value);

private:
    int m_size;
    int m_id;
    QMainWindow *m_window;
    static QFont m_littleFont;
    static QFont m_bigFont;


    static int s_newSelected;
    static int s_oldSelected;


private slots:
    void focus();
    void change();
};

#endif // BOX_H
