#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QSharedPointer>

#include <QDebug>
#include <QListView>
#include <QModelIndex>
#include <QStringListModel>
#include <string>
#include <QString>
#include "types.h"
#include <vector>

using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_listViewRes_activated(const QModelIndex &index);

    void on_ButtonBusc_clicked();

    void on_listViewRes_doubleClicked(const QModelIndex &index);

    void on_listViewRes_entered(const QModelIndex &index);

    void on_Load();

private:
    Ui::MainWindow *ui;
    QSharedPointer<QGraphicsScene> ptr_scene;
    QSharedPointer<QGraphicsPixmapItem> ptr_item;
    QSharedPointer<QStringListModel> ptr_list;
    QString file;
    vector<pair<I,F> > results;
    vector<string> images;
    vector<vector<D> > test;
};

#endif // MAINWINDOW_H
