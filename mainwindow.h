#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QVector>
#include <QDir>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QGridLayout>
#include <QListView>
#include <QImageWriter>
#include <QTableWidget>
#include <QFileDialog>
#include <QHeaderView>

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
    void on_pushButton_clicked();
    void dialogClose();
public slots:
    void on_listView_doubleClicked(const QModelIndex &index);
    void twInfoSelected(int logicalIndex);
private:
    Ui::MainWindow *ui;
    bool line_changed=false;
    QFileSystemModel *model;
    QGridLayout *backgr;
    QListView *listView;
    QTableWidget *twInfo=nullptr;
    int *sortcolumns = new int[5];
};
#endif // MAINWINDOW_H
