#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QFileDialog>
#include<QMessageBox>
#include<string.h>
#include"filterwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void recv_filter(QVariant data);
    void on_FilterButton_clicked();
    void on_ResetButton_clicked();
    void on_BackupSourceFolderButton_clicked();
    void on_BackupDestFolderButton_clicked();
    void on_BackupButton_clicked();
    void on_BackupPackCB_clicked();
    void on_BackupCompressCB_clicked();
    void on_BackupEncryptCB_clicked();
    void on_PackSourceFolderButton_clicked();
    void on_PackDestFolderButton_clicked();
    void on_PackButton_clicked();
    void on_UnpackSourceFolderButton_clicked();
    void on_UnpackDestFolderButton_clicked();
    void on_UnpackButton_clicked();
    void on_CompressSourceFolderButton_clicked();
    void on_CompressDestFolderButton_clicked();
    void on_CompressEncryptCB_clicked();
    void on_CompressButton_clicked();
    void on_DecompressSourceFolderButton_clicked();
    void on_DecompressDestFolderButton_clicked();
    void on_DecompressUnpackCB_clicked();
    void on_DecompressButton_clicked();

private:
    Ui::MainWindow *ui;
    Filterwidget fw;
    QString DecompressFileName, CompressFileName, PackFileName, BackupFileName;
    inline int err(int v);
};
#endif // MAINWINDOW_H
