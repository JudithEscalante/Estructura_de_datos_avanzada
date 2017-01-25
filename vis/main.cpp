#include <pqxx/pqxx>
#include "mainwindow.h"
#include <QApplication>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <highgui.h>
#include <sstream>
#include <fstream>
#include <string>
using namespace cv;
using namespace std;
B comp2(pair<I,I> i,pair<I,I> j) { return (i.second>j.second); }
I main(I argc, C *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    //////////////// PCA ////////////////////
    /*
    Mat m(830,1024,CV_32F);
    float *vec=m.ptr<float>(0);
    //float vec[849920];
    ifstream g("vectors.txt");
    int i=0;
    string line;
    while (std::getline(g,line)){
        stringstream iss;
        iss.str(line);
        S  it;
        //vector<S> t;
        while(getline(iss,it,' ')){
            //vec[i]=atof(it.c_str());
            *(vec+i)=atof(it.c_str());
        }

        i++;
    }


    PCA pca(m,Mat(),0,12);

    ofstream ge("val.txt");
    for(int i=0;i<pca.eigenvectors.rows;++i){

        for (int j=0;j<pca.eigenvectors.cols;++j){
            ge<<pca.eigenvectors.at<F>(i,j)<<" ";
        }
        ge<<endl;
    }

    return 0;
    */

    ////////// Postgres /////////////

    /// 12 - dimensional
    /*
    pqxx::connection c("dbname=alexandra user=alexandra");
    pqxx::work exe(c);

    ifstream l("insert.txt");
    ifstream n("images.txt");
    S cad, name;
    while(std::getline(l,cad) && std::getline(n,name)){

        exe.exec("INSERT INTO imagen2 VALUES("+exe.quote(cad)+", "+exe.quote(name)+")");

    }
    exe.commit();
    */


}
