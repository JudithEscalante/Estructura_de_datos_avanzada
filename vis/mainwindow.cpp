//#include <pqxx/pqxx>
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "CImg.h"
#include <iostream>
#include <string>
#include "colorDesc.h"

//using namespace cimg_library;

#include <QDirIterator>
#include <qdebug.h>
#include <algorithm>
#include <fstream>

#include <struct.h>
#include "functions.h"

#define TAM 812

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ptr_list = QSharedPointer<QStringListModel>(new QStringListModel(this));
    ui->listViewRes->setModel(this->ptr_list.data());
    ui->listViewRes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->file="";
    ui->limite->setText("10");
    ui->limite->setValidator(new QIntValidator(1,10000,this));

    ifstream f_("/home/alexandra/Documentos/caffe/vec_.txt");
    S ccad="";
    while(getline(f_,ccad)){
        vector<D> vec;
        split(ccad,' ',vec);
        this->test.push_back(vec);
    }


}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_pushButton_clicked()
{
    this->ptr_list->removeRows(0, this->ptr_list->rowCount());
    this->images.clear();
    QString fileName = QFileDialog::getOpenFileName(this,"Open Image File",/*QDir::currentPath()*/"/home/alexandra/Documentos/caffe/images");


    if(!fileName.isEmpty())
    {
        QImage image(fileName);

        if(image.isNull())
        {
            QMessageBox::information(this,"Image Viewer","Error Displaying image");
            ui->ButtonBusc->setEnabled(false);
            return;
        }

        this->file=fileName;
        this->results.clear();
        this->images.clear();

        if (!this->ptr_scene and !this->ptr_item){
            this->ptr_scene = QSharedPointer<QGraphicsScene>(new QGraphicsScene());
            ui->graphicsView->setScene(this->ptr_scene.data());
            this->ptr_item = QSharedPointer<QGraphicsPixmapItem>(new QGraphicsPixmapItem());
            this->ptr_scene->addItem(this->ptr_item.data());
        }
        this->ptr_item->setPixmap(QPixmap(fileName));

        ui->ButtonBusc->setEnabled(true);
    }
    else{
        ui->ButtonBusc->setEnabled(false);
    }
}

void MainWindow::on_ButtonBusc_clicked()
{
    this->ptr_list->removeRows(0, this->ptr_list->rowCount());
    
    /////////////////////////// Histograms //////////////////

    /*
    QDirIterator dirIt("/home/alexandra/Documentos/EDA/qt/nuevo_vis/jpg_",QDirIterator::Subdirectories);
    I cont=0;

    Mat img1 = imread(this->file.toStdString().c_str(), 1);
    colorDesc d(8,12,3);
    d.describe(img1);

    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (QFileInfo(dirIt.filePath()).suffix() == "jpg"){


               Mat img2 = imread(dirIt.filePath().toStdString().c_str(), 1);
               colorDesc f(8,12,3);
               f.describe(img2);


               F c=0;
               for (I i=0;i<5;i++){
                   F g=compareHist(d.features[i],f.features[i],0);
                   //cout<<g<<endl;
                   c+=g;
               }
               results.push_back(pair<I,F>(cont,c/5));
               images.push_back(dirIt.filePath());
               cont++;
               qDebug(QString::number(cont).toStdString().c_str());
            }

    }

    sort (results.begin(), results.end(), comp);

    //shoe
    if(ui->limite->text()=="")
        ui->limite->setText("10");

    cont=1;
    QStringList list;
    list = this->ptr_list->stringList();
    while(cont<=ui->limite->text().toInt()){

        list.append(QString::number(cont));

        cont++;
    }
    this->ptr_list->setStringList(list);*/

    /////////////////// Feature vectors - L ///////////////////

    QFileInfo name(this->file);
    I indx=stoi(name.baseName().toStdString());

    ifstream f_("vectors.txt");
    S ccad="";
    I index=-1, n=0;

    while(getline(f_,ccad)){
        vector<D> vec;
        split(ccad,' ',vec);

        D m=euclidean(this->test[indx],vec);

        results.push_back(pair<I,F>(n,m));

        n++;
    }

    sort (results.begin(), results.end(), comp);

    qDebug(to_string(results[0].first).c_str());
    qDebug(to_string(results[1].first).c_str());
    qDebug(to_string(results[2].first).c_str());
    qDebug(to_string(results[3].first).c_str());
    qDebug(to_string(results[4].first).c_str());
    qDebug(to_string(results[5].first).c_str());

    if(ui->limite->text()=="")
        ui->limite->setText("10");

    I cont=1;
    QStringList list;
    list = this->ptr_list->stringList();
    while(cont<=ui->limite->text().toInt()){

        list.append(QString::number(cont));

        cont++;
    }
    this->ptr_list->setStringList(list);



    //////////////// Feature vectors - K ////////////////


    /*
    ifstream f_("vectors.txt");
    S ccad="";
    I index=-1, n=0;
    D min=100000000.0;

    struct kd_node_t *root, *points;
    //vector<kd_node_t> points(TAM);
    points =(struct kd_node_t*) calloc(TAM, sizeof(struct kd_node_t));
    //points = new kd_node_t[TAM];
    I i=0;



    while(getline(f_,ccad)){
        //vector<D> vec;
        split(ccad,' ',points[i].x);
        ++i;
    }

    qDebug((to_string((points+8)->x[45])).c_str());
    root = make_tree(points, TAM-1, 0, DIM);

    ifstream h("images.txt");
    vector<S> im;
    while(!h.eof()){
        char line[100];
        h.getline(line,500);
        im.push_back(S(line));
    }

    QFileInfo name(this->file);
    I indx=stoi(name.baseName().toStdString());


    //Mat vv=imread(im[index].c_str());
    struct kd_node_t tes;
    tes.x=this->test[indx];

    struct kd_node_t *found;
    D best_dist;
    nearest(root, &tes, 0, DIM, &found, &best_dist);*/
}

void MainWindow::on_listViewRes_doubleClicked(const QModelIndex &index)
{
    /*
    pqxx::connection c("dbname=alexandra user=alexandra");
    pqxx::work exe(c);

    pqxx::result result = exe.exec( "SELECT * FROM imagen" );
    pqxx::binarystring blob2(result[0]["data"]);

    I w,h;
    result[0]["width"].to(w);
    result[0]["height"].to(h);
    CImg<F> x((F*)blob2.data(),w,h,1,3);
    x.display();*/

    ifstream h("images.txt");

    while(!h.eof()){
        char line[100];
        h.getline(line,500);
        this->images.push_back(S(line));
    }

    Mat img1=imread(images[this->results[index.row()].first].c_str());
    cv::resize(img1,img1,Size(img1.cols/3,img1.rows/3));
    imshow( "Imagen", img1 );
    waitKey(0);
}

void MainWindow::on_listViewRes_entered(const QModelIndex &index)
{
    on_listViewRes_doubleClicked(index);
}

void MainWindow::on_listViewRes_activated(const QModelIndex &index)
{

}

void MainWindow::on_Load()
{
    QFileInfo name(this->file);
    /*I indx=stoi(name.baseName().toStdString());

    ifstream f_("vectors.txt");
    S ccad="";
    I index=-1, n=0;
    D min=100000000.0;

    struct kd_node_t *root, *points;
    points =(struct kd_node_t*) calloc(TAM, sizeof(struct kd_node_t));
    I i=0;

    while(getline(f_,ccad)){
        //vector<D> vec;
        split(ccad,' ',points[i].x);


    }

    root = make_tree(points, TAM, 0, DIM);*/

}
