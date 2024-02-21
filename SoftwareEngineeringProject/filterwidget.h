#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include<QWidget>
#include<QVector>
#include<QMessageBox>
#include<QVariant>
#include<QButtonGroup>
#include<QStringList>
#include<QDebug>
#include<QDate>
#include<QDateTime>

namespace Ui {
class Filterwidget;
}

struct filter1
{
    int type;//0 null 1 path, 2 name, 3type, 4time
    bool isblack;//
    QVector<QString>path, name, suff;
    time_t dayl, dayr;
    inline void clear()
    {
        type = 0;
        isblack = 1;
        path.clear();
        name.clear();
        suff.clear();
        dayl = dayr = 0;
    }
    filter1()
    {
        clear();
    }
};

Q_DECLARE_METATYPE(filter1);

class Filterwidget : public QWidget
{
    Q_OBJECT

public:
    explicit Filterwidget(QWidget *parent = nullptr);
    void clear();
    ~Filterwidget();

signals:
    void send_filter(QVariant);

private slots:
    void on_PathRB_clicked();
    void on_NameRB_clicked();
    void on_TypeRB_clicked();
    void on_TimeRB_clicked();
    void on_ConfirmButton_clicked();

private:
    Ui::Filterwidget *ui;
    QButtonGroup *BG1, *BG2;
};

#endif // FILTERWIDGET_H
