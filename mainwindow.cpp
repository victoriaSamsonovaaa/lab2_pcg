#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::QDir::AllEntries);
    model->setRootPath("");
    ui->listView->setModel(model);
    ui->lineEdit->setText(" ");
    for(int i = 0; i < 5; i++)
    {
        sortcolumns[i] = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QFileDialog *files = new QFileDialog;
    files->setWindowTitle("Open files");
    QList<QUrl> list = files->getOpenFileUrls();
    QDialog *table = new QDialog();
    table->setWindowTitle("Table");
    table->setWindowIcon(QPixmap(":/img/folder.png"));
    QGridLayout *tableLayout = new QGridLayout(table);
    table->setMinimumSize(700,700);
    twInfo = new QTableWidget(table);
    tableLayout->addWidget(twInfo);
    twInfo->setColumnCount(5);
    twInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    twInfo->setRowCount(list.size());
    twInfo->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    twInfo->setHorizontalHeaderItem(1, new QTableWidgetItem("Size"));
    twInfo->setHorizontalHeaderItem(2, new QTableWidgetItem("Resolution"));
    twInfo->setHorizontalHeaderItem(3, new QTableWidgetItem("Color depth"));
    twInfo->setHorizontalHeaderItem(4, new QTableWidgetItem("Contraction"));
    twInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for(int i = 0; i < list.size(); i++)
    {
        QFile temp(list[i].toLocalFile());
        QString resolution = "";
        if (temp.fileName().lastIndexOf('.') == -1)
        {
            twInfo->setRowCount(twInfo->rowCount()-1);
            continue;
        }
        for(int i = temp.fileName().lastIndexOf('.'); i < temp.fileName().size(); i++)
        {
            resolution.append(temp.fileName()[i]);
        }
        if (resolution != ".jpg" && resolution != ".gif" && resolution != ".tif" && resolution != ".bmp" &&
            resolution != ".png" && resolution != ".pcx" && resolution != ".BMP" && resolution != ".PNG" &&
            resolution != ".JPG" && resolution != ".jpeg")

        {
            twInfo->setRowCount(twInfo->rowCount()-1);
            continue;
        }

        QString fileName = "";
        for(int i = temp.fileName().lastIndexOf('/') + 1; i < temp.fileName().lastIndexOf('.'); i++)
        {
            fileName.append(temp.fileName()[i]);
        }
        twInfo->setItem(i,0,new QTableWidgetItem(fileName));
        QImage im(list[i].toLocalFile());
        QImageWriter a(list[i].toLocalFile());
        twInfo->setItem(i, 2, new QTableWidgetItem(QString::number(im.physicalDpiX())));
        twInfo->setItem(i, 1, new QTableWidgetItem(QString::number(im.size().width())+"x"+QString::number(im.size().height())));
        twInfo->setItem(i, 3, new QTableWidgetItem(QString::number(im.bitPlaneCount())));
        twInfo->setItem(i, 4, new QTableWidgetItem(QString::number(a.compression())));
    }
    if (twInfo->rowCount() == 0)
    {
        return;
    }
    table->show();
    table->setStyleSheet("background-color:#40432F;"
                         "color: #F7E8C6;"
                         " border-collapse: collapse;"
                         "border: 1px solid grey;"
                                         );

    connect(table, SIGNAL(rejected()), this, SLOT(dialogClose()));
    connect(twInfo->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(twInfoSelected(int)));
    this->hide();
}


void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QListView* listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);
    ui->lineEdit->setText(model->filePath(index));
    if(fileInfo.fileName() == "..")
    {
        QDir dir = fileInfo.dir();
        dir.cdUp();
        model->index(dir.absolutePath());
        listView->setRootIndex(model->index(dir.absolutePath()));
    }
    else if (fileInfo.fileName() == ".")
    {
        listView->setRootIndex(model->index(""));
    }

    else if(fileInfo.isDir())
    {
        listView->setRootIndex(index);
    }
    else if(!fileInfo.isDir())
    {
        QDir dir = fileInfo.dir();
        QString fileExt = model->fileName(index);
        QImageWriter a(model->filePath(index));
        QString resolution = "";
        QImage img (model->filePath(index));
        for(int i = fileExt.lastIndexOf('.'); i < fileExt.size(); i++)
        {
            resolution.append(fileExt[i]);
        }
        if (resolution == ".JPG" || resolution == ".jpg" || resolution == ".gif" || resolution == ".tif" || resolution == ".bmp" ||
            resolution == ".PNG"||resolution == ".png" || resolution == ".pcx" || resolution == ".BMP" || resolution == ".jpeg")
        {
            ui->filename->setText(model->fileName(index));
            ui->size->setText(QString::number(img.size().width() )+ "x" + QString::number(img.size().height() ));
            ui->contraction->setText(QString::number(a.compression()));
            ui->depth->setText(QString::number(img.bitPlaneCount()));
            ui->resolution->setText(QString::number(img.physicalDpiX()));
        }
    }
}

void MainWindow::dialogClose()
{
    this->show();
}

bool column1(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text() > item2->text();
}


bool column2(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    QStringList string1 = item1->text().split('x');
    QStringList string2 = item2->text().split('x');
    if (string1[0].toInt() > string2[0].toInt())
    {
        return true;
    }
    return false;
}

bool column3(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text() > item2 -> text();
}

bool column4(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text().toInt() > item2->text().toInt();
}

bool column5(QTableWidgetItem* item1, QTableWidgetItem* item2)
{
    return item1->text().toInt() > item2->text().toInt();
}

std::vector<std::function<bool(QTableWidgetItem*, QTableWidgetItem*)>> functions = {column1, column2, column3, column4, column5};

std::vector<int> sortcolumns(5, 0);

void MainWindow::twInfoSelected(int columnIndex)
{
    if (sortcolumns[columnIndex] == 0)
    {
        for(int i = 0; i < twInfo->rowCount(); i++)
        {
            for(int j = 0; j < twInfo->rowCount() - 1; j++)
            {
                if (functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                {
                    for(int k = 0; k < twInfo->columnCount(); k++)
                    {
                        QString temp1 = twInfo->item(j, k)->text();
                        QString temp2 = twInfo->item(j + 1, k) -> text();
                        twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                        twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                    }
                }
            }
        }

        for(int k = 0; k < 5; k++)
        {
            sortcolumns[k] = 0;
        }
        sortcolumns[columnIndex] = 1;
    }
    else
    {
        if (sortcolumns[columnIndex] == 1)
        {
            for(int i = 0; i < twInfo->rowCount(); i++)
            {
                for(int j = 0; j < twInfo->rowCount() - 1; j++)
                {
                    if (!functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                    {
                        for(int k = 0; k < twInfo->columnCount(); k++)
                        {
                            QString temp1 = twInfo->item(j, k)->text();
                            QString temp2 = twInfo->item(j + 1, k) -> text();
                            twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                            twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                        }
                    }
                }
            }
            for(int k = 0; k < 5; k++)
            {
                sortcolumns[k] = 0;
            }
            sortcolumns[columnIndex] = 2;
        }
        else
        {
            for(int i = 0; i < twInfo->rowCount(); i++)
            {
                for(int j = 0; j < twInfo->rowCount() - 1; j++)
                {
                    if (functions[columnIndex](twInfo->item(j, columnIndex), twInfo->item(j + 1, columnIndex)))
                    {
                        for(int k = 0; k < twInfo->columnCount(); k++)
                        {
                            QString temp1 = twInfo->item(j, k)->text();
                            QString temp2 = twInfo->item(j + 1, k) -> text();
                            twInfo->setItem(j+1, k, new QTableWidgetItem(temp1));
                            twInfo->setItem(j, k, new QTableWidgetItem(temp2));
                        }
                    }
                }
            }
            for(int k = 0; k < 5; k++)
            {
                sortcolumns[k] = 0;
            }
            sortcolumns[columnIndex] = 1;
        }
    }
}



