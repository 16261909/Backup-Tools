#include"mainwindow.h"
#include"ui_mainwindow.h"
#include"kernel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&fw, SIGNAL(send_filter(QVariant)), this, SLOT(recv_filter(QVariant)));
    ui->CompressPasswordLine->hide();
    ui->label_2->hide();
    ui->BackupCompressCB->hide();
    ui->BackupEncryptCB->hide();
    ui->label_11->hide();
    ui->label->hide();
    ui->BackupPasswordLine->hide();
    ui->FilterLabel->hide();
    ui->DecompressClearCB->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

inline int MainWindow::err(int v = 0)
{
    switch(v)
    {
        case 1:
            QMessageBox::critical(this, "Error", "Doesn't correspond to Ustar Format.");
            break;
        case 2:
            QMessageBox::critical(this, "Error", "Checksum is wrong or data was modified.");
            break;
        case 3:
            QMessageBox::critical(this, "Error", "Destination directionary already exists.");
            break;
        case 4:
            QMessageBox::critical(this, "Error", "Password is wrong.");
            break;
        case 5:
            QMessageBox::critical(this, "Error", "Cannot open files.");
            break;
        case 6:
            QMessageBox::critical(this, "Error", "Cannot create a new folder.");
            break;
        case 7:
            QMessageBox::critical(this, "Error", "Unable to get root permission.");
            break;
    }
    return v;
}

void MainWindow::recv_filter(QVariant data)
{
    FT.clear();
    FT.dayl = data.value<filter1>().dayl;
    FT.dayr = data.value<filter1>().dayr;
    FT.type = data.value<filter1>().type;
    FT.path = data.value<filter1>().path;
    FT.name = data.value<filter1>().name;
    FT.suff = data.value<filter1>().suff;
    FT.isblack = data.value<filter1>().isblack;
    if(FT.type == 0)
    {
        ui->FilterLabel->hide();
    }
    else if(FT.type == 1)
    {
        ui->FilterLabel->show();
        ui->FilterLabel->setText("Path");
    }
    else if(FT.type == 2)
    {
        ui->FilterLabel->show();
        ui->FilterLabel->setText("Name");
    }
    else if(FT.type == 3)
    {
        ui->FilterLabel->show();
        ui->FilterLabel->setText("type");
    }
    else if(FT.type == 4)
    {
        ui->FilterLabel->show();
        ui->FilterLabel->setText("Time");
    }
}

void MainWindow::on_FilterButton_clicked()
{
    fw.show();
}

void MainWindow::on_ResetButton_clicked()
{
    FT.clear();
    ui->FilterLabel->hide();
    fw.clear();
}

void MainWindow::on_BackupSourceFolderButton_clicked()
{
    QString from = QFileDialog::getExistingDirectory(this, "Choose source directory", "/home");
    ui->BackupSourceLine->setText(from);
    if(ui->BackupPackCB->checkState() == Qt::Checked)
    {
        if(ui->BackupCompressCB->checkState() != Qt::Checked)//.tar
        {
            from += ".tar";
        }
        else//.huf
        {
            from += ".tar.huf";
        }
    }
    ui->BackupDestLine->setText(from);
    QFileInfo file(from);
    BackupFileName = file.fileName();
}

void MainWindow::on_BackupDestFolderButton_clicked()
{
    if(ui->BackupPackCB->checkState() != Qt::Checked)//folder
    {
        ui->BackupDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home"));
    }
    else//.tar .huf
    {
        ui->BackupDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home") + "/" + BackupFileName);
    }
}
void MainWindow::on_BackupPackCB_clicked()
{
    if(ui->BackupPackCB->checkState() == Qt::Checked)
    {
        ui->BackupClearCB->hide();
        ui->BackupCompressCB->show();
        if(ui->BackupCompressCB->checkState() == Qt::Checked)
        {
            ui->BackupEncryptCB->show();
            if(ui->BackupEncryptCB->checkState() == Qt::Checked)
            {
                ui->label_11->show();
                ui->label->show();
                ui->BackupPasswordLine->show();
            }
            else
            {
                ui->label_11->hide();
                ui->label->hide();
                ui->BackupPasswordLine->hide();
            }
        }
    }
    else
    {
        ui->BackupClearCB->show();
        ui->BackupCompressCB->hide();
        ui->BackupEncryptCB->hide();
        ui->label_11->hide();
        ui->label->hide();
        ui->BackupPasswordLine->hide();
    }
}
void MainWindow::on_BackupCompressCB_clicked()
{
    if(ui->BackupCompressCB->checkState() == Qt::Checked)
    {
        ui->BackupEncryptCB->show();
        if(ui->BackupEncryptCB->checkState() == Qt::Checked)
        {
            ui->label_11->show();
            ui->label->show();
            ui->BackupPasswordLine->show();
        }
        else
        {
            ui->label_11->hide();
            ui->label->hide();
            ui->BackupPasswordLine->hide();
        }
    }
    else
    {
        ui->BackupEncryptCB->hide();
        ui->label_11->hide();
        ui->label->hide();
        ui->BackupPasswordLine->hide();
    }
}
void MainWindow::on_BackupEncryptCB_clicked()
{
    if(ui->BackupEncryptCB->checkState() == Qt::Checked)
    {
        ui->label_11->show();
        ui->label->show();
        ui->BackupPasswordLine->show();
    }
    else
    {
        ui->label_11->hide();
        ui->label->hide();
        ui->BackupPasswordLine->hide();
    }
}
void MainWindow::on_BackupButton_clicked()
{
    if(ui->BackupSourceLine->text().isEmpty() || ui->BackupDestLine->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Folder path cannot be empty.");
        return;
    }
    QByteArray tmp;
    char from[100] = {0}, to[100] = {0}, key[100] = {0};
    tmp.append(ui->BackupSourceLine->text());
    strcpy(from, tmp);
    tmp.clear();
    tmp.append(ui->BackupDestLine->text());
    strcpy(to, tmp);
    if(ui->BackupPackCB->checkState() != Qt::Checked)
    {
        strcat(to, ".tar");
        err(pack(from, to));
        strcpy(from, to);
        to[strlen(to) - 4] = 0;
        err(unpack(from, to, ui->BackupClearCB->checkState() == Qt::Checked));
        if(ui->BackupKeepCB->checkState() != Qt::Checked)
        {
            remove(from);
        }
    }
    else if(ui->BackupCompressCB->checkState() != Qt::Checked)
    {
        err(pack(from, to));
    }
    else if(ui->BackupEncryptCB->checkState() != Qt::Checked)
    {
        to[strlen(to) - 4] = 0;
        err(pack(from, to));
        strcpy(from, to);
        strcat(to, ".huf");
        hufzip(from, to, key, ui->BackupKeepCB->checkState() == Qt::Checked);
        printf("%s\n", from);
        if(ui->BackupKeepCB->checkState() != Qt::Checked)
        {
            remove(from);
            from[strlen(from) - 8] = 0;
            remove(from);
        }
    }
    else
    {
        if(ui->BackupEncryptCB->checkState() == Qt::Checked && ui->BackupPasswordLine->text().length() > 32)
        {
            QMessageBox::critical(this, "error", "The length of password is longer than 32.");
            return;
        }
        if(ui->BackupEncryptCB->checkState() == Qt::Checked && ui->BackupPasswordLine->text().length() == 0)
        {
            QMessageBox::critical(this, "error", "Password cannot be empty.");
            return;
        }
        tmp.clear();
        tmp.append(ui->BackupPasswordLine->text());
        strcpy(key, tmp);
        to[strlen(to) - 4] = 0;
        err(pack(from, to));
        strcpy(from, to);
        strcat(to, ".huf");
        err(hufzip(from, to, key, ui->BackupKeepCB->checkState() == Qt::Checked));
        if(ui->BackupKeepCB->checkState() != Qt::Checked)
        {
            remove(from);
        }
    }
}

void MainWindow::on_PackSourceFolderButton_clicked()
{
    QString from = QFileDialog::getExistingDirectory(this, "Choose source directory", "/home");
    ui->PackSourceLine->setText(from);
    from += ".tar";
    ui->PackDestLine->setText(from);
    QFileInfo file(from);
    PackFileName = file.fileName();
}

void MainWindow::on_PackDestFolderButton_clicked()
{
    ui->PackDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home") + "/" + PackFileName);
}

void MainWindow::on_PackButton_clicked()
{
    if(ui->PackSourceLine->text().isEmpty() || ui->PackDestLine->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Folder path cannot be empty.");
        return;
    }
    QByteArray tmp;
    char from[100] = {0}, to[100] = {0};
    tmp.append(ui->PackSourceLine->text());
    strcpy(from, tmp);
    tmp.clear();
    tmp.append(ui->PackDestLine->text());
    strcpy(to, tmp);
    err(pack(from, to));
}

void MainWindow::on_UnpackSourceFolderButton_clicked()
{
    QString from = QFileDialog::getOpenFileName(this, "Choose source file", "/home", "*.tar");
    ui->UnpackSourceLine->setText(from);
    from.chop(4);
    ui->UnpackDestLine->setText(from);
    QFileInfo file(from);
    DecompressFileName = file.fileName();
}

void MainWindow::on_UnpackDestFolderButton_clicked()
{
    ui->UnpackDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home"));
}

void MainWindow::on_UnpackButton_clicked()
{
    if(ui->UnpackSourceLine->text().isEmpty() || ui->UnpackDestLine->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Folder path cannot be empty.");
        return;
    }
    QByteArray tmp;
    char from[100] = {0}, to[100] = {0};
    tmp.append(ui->UnpackSourceLine->text());
    strcpy(from, tmp);
    tmp.clear();
    tmp.append(ui->UnpackDestLine->text());
    strcpy(to, tmp);
    if(err(unpack(from, to, ui->UnpackClearCB->checkState() == Qt::Checked)))return;
}

void MainWindow::on_CompressSourceFolderButton_clicked()
{
    QString from = QFileDialog::getOpenFileName(this, "Choose source file", "/home", "*.tar");
    ui->CompressSourceLine->setText(from);
    from += ".huf";
    ui->CompressDestLine->setText(from);
    QFileInfo file(from);
    CompressFileName = file.fileName();
}

void MainWindow::on_CompressDestFolderButton_clicked()
{
    ui->CompressDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home") + "/" + CompressFileName);
}

void MainWindow::on_CompressEncryptCB_clicked()
{
    if(ui->CompressEncryptCB->checkState() != Qt::Checked)
    {
        ui->CompressPasswordLine->hide();
        ui->label_2->hide();
    }
    else
    {
        ui->CompressPasswordLine->show();
        ui->label_2->show();
    }
}

void MainWindow::on_CompressButton_clicked()
{
    if(ui->CompressSourceLine->text().isEmpty() || ui->CompressDestLine->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Folder path cannot be empty.");
        return;
    }
    if(ui->CompressEncryptCB->checkState() == Qt::Checked && ui->CompressPasswordLine->text().length() > 32)
    {
        QMessageBox::critical(this, "error", "The length of password is longer than 32.");
        return;
    }
    if(ui->CompressEncryptCB->checkState() == Qt::Checked && ui->CompressPasswordLine->text().length() == 0)
    {
        QMessageBox::critical(this, "error", "Password cannot be empty.");
        return;
    }
    QByteArray tmp;
    char from[200] = {0}, to[200] = {0}, key[100] = {0};
    tmp.append(ui->CompressSourceLine->text());
    strcpy(from, tmp);
    tmp.clear();
    tmp.append(ui->CompressDestLine->text());
    strcpy(to, tmp);
    if(ui->CompressEncryptCB->checkState() == Qt::Checked)
    {
        tmp.clear();
        tmp.append(ui->CompressPasswordLine->text());
        strcpy(key, tmp);
    }
    err(hufzip(from, to, key, ui->CompressKeepCB->checkState() == Qt::Checked));
}
void MainWindow::on_DecompressSourceFolderButton_clicked()
{
    QString from = QFileDialog::getOpenFileName(this, "Choose source file", "/home", "*.huf");
    ui->DecompressSourceLine->setText(from);
    if(ui->DecompressUnpackCB->checkState() == Qt::Checked)
    {
        from.chop(8);
    }
    else
    {
        from.chop(4);
    }
    ui->DecompressDestLine->setText(from);
    QFileInfo file(from);
    DecompressFileName = file.fileName();
}

void MainWindow::on_DecompressDestFolderButton_clicked()
{
    if(ui->DecompressUnpackCB->checkState() == Qt::Checked)
    {
        ui->DecompressDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home"));
    }
    else
    {
        ui->DecompressDestLine->setText(QFileDialog::getExistingDirectory(this, "Choose destination directory", "/home") + "/" + DecompressFileName);
    }
}

void MainWindow::on_DecompressUnpackCB_clicked()
{
    if(ui->DecompressUnpackCB->checkState() == Qt::Checked)
    {
        ui->DecompressClearCB->show();
    }
    else
    {
        ui->DecompressClearCB->hide();
    }
}

void MainWindow::on_DecompressButton_clicked()
{
    if(ui->DecompressSourceLine->text().isEmpty() || ui->DecompressDestLine->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Folder path cannot be empty.");
        return;
    }
    QByteArray tmp;
    char from[100] = {0}, to[100] = {0}, key[40] = {0};
    tmp.append(ui->DecompressSourceLine->text());
    strcpy(from, tmp);
    tmp.clear();
    tmp.append(ui->DecompressDestLine->text());
    strcpy(to, tmp);
    tmp.clear();
    tmp.append(ui->DecompressPasswordLine->text());
    strcpy(key, tmp);
    strcat(to, ".tar");
    if(err(hufunzip(from, to, key, ui->DecompressKeepCB->checkState() == Qt::Checked)))return;
    else
    {
        if(ui->DecompressUnpackCB->checkState() == Qt::Checked)
        {
            strcpy(from, to);
            to[strlen(to) - 4] = 0;
            if(err(unpack(from, to, ui->DecompressClearCB->checkState() == Qt::Checked)))return;
            if(ui->DecompressKeepCB->checkState() != Qt::Checked)
            {
                remove(from);
            }
        }
    }
}
