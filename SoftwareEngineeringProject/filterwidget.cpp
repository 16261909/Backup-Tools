#include"filterwidget.h"
#include"ui_filterwidget.h"


Filterwidget::Filterwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Filterwidget)
{
    ui->setupUi(this);
    BG1 = new QButtonGroup();
    BG1->addButton(ui->PathRB);
    BG1->addButton(ui->NameRB);
    BG1->addButton(ui->TypeRB);
    BG1->addButton(ui->TimeRB);
    BG1->setId(ui->PathRB, 0);
    BG1->setId(ui->NameRB, 1);
    BG1->setId(ui->TypeRB, 2);
    BG1->setId(ui->TimeRB, 3);
    BG2 = new QButtonGroup();
    BG2->addButton(ui->BlackRB);
    BG2->addButton(ui->WhiteRB);
    BG2->setId(ui->BlackRB, 0);
    BG2->setId(ui->WhiteRB, 1);
    clear();
}

void Filterwidget::clear()
{
    ui->ToTime->setDateTime(QDateTime::currentDateTime());
    ui->FromTime->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->TextEdit->hide();
    ui->TipsLabel->hide();
    ui->FromTime->hide();
    ui->FromLabel->hide();
    ui->ToTime->hide();
    ui->Tolabel->hide();
    BG1->setExclusive(0);
    BG2->setExclusive(0);
    ui->PathRB->setChecked(0);
    ui->NameRB->setChecked(0);
    ui->TypeRB->setChecked(0);
    ui->TimeRB->setChecked(0);
    ui->BlackRB->setChecked(0);
    ui->WhiteRB->setChecked(0);
    BG1->setExclusive(1);
    BG2->setExclusive(1);
}

Filterwidget::~Filterwidget()
{
    delete ui;
}

void Filterwidget::on_PathRB_clicked()
{
    ui->TextEdit->show();
    ui->TipsLabel->show();
    ui->FromTime->hide();
    ui->FromLabel->hide();
    ui->ToTime->hide();
    ui->Tolabel->hide();
}

void Filterwidget::on_NameRB_clicked()
{
    ui->TextEdit->show();
    ui->TipsLabel->show();
    ui->FromTime->hide();
    ui->FromLabel->hide();
    ui->ToTime->hide();
    ui->Tolabel->hide();
}

void Filterwidget::on_TypeRB_clicked()
{
    ui->TextEdit->show();
    ui->TipsLabel->show();
    ui->FromTime->hide();
    ui->FromLabel->hide();
    ui->ToTime->hide();
    ui->Tolabel->hide();
}

void Filterwidget::on_TimeRB_clicked()
{
    ui->TextEdit->hide();
    ui->TipsLabel->hide();
    ui->FromTime->show();
    ui->FromLabel->show();
    ui->ToTime->show();
    ui->Tolabel->show();
}

void Filterwidget::on_ConfirmButton_clicked()
{
    struct filter1 FT1;
    if(ui->WhiteRB->isChecked())
    {
        FT1.isblack = 0;
    }
    else if(ui->BlackRB->isChecked())
    {
        FT1.isblack = 1;
    }
    else
    {
        FT1.clear();
        QMessageBox::critical(this, "Error", "Please select blacklist or whitelist.");
        return;
    }
    if(ui->PathRB->isChecked())
    {
        QStringList list = ui->TextEdit->toPlainText().split("\n");
        for(auto u: list)FT1.path.push_back(u);
        FT1.type = 1;
    }
    else if(ui->NameRB->isChecked())
    {
        QStringList list = ui->TextEdit->toPlainText().split("\n");
        for(auto u: list)FT1.name.push_back(u);
        FT1.type = 2;
    }
    else if(ui->TypeRB->isChecked())
    {
        QStringList list = ui->TextEdit->toPlainText().split("\n");
        for(auto u: list)FT1.suff.push_back(u);
        FT1.type = 3;
    }
    else if(ui->TimeRB->isChecked())
    {
        FT1.dayl = ui->FromTime->dateTime().toTime_t();
        FT1.dayr = ui->ToTime->dateTime().toTime_t();
        FT1.type = 4;
        if(FT1.dayl > FT1.dayr)
        {
            FT1.clear();
            QMessageBox::critical(this, "Error", "The start time should be less than or equal to the end time.");
            return;
        }
    }
    else
    {
        FT1.clear();
        QMessageBox::critical(this, "Error", "Please select a filter type.");
        return;
    }
    this->hide();
    QVariant var;
    var.setValue(FT1);
    emit send_filter(var);
}
