#ifndef COLORDESC_H
#define COLORDESC_H
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <qdebug.h>
using namespace cv;
using namespace std;

class colorDesc{
public:
    vector<I> bins;
    vector<MatND > features;

    colorDesc(I bins1, I bins2, I bins3){
        bins.push_back(bins1);
        bins.push_back(bins2);
        bins.push_back(bins3);
    }
    ~colorDesc(){
        //for (I i=0;i<features.size();++i)
            //delete features[i];
    }
    void describe(/*Mat& image,*/ Mat &image2){

        cvtColor(image2, image2, COLOR_BGR2HSV);
        tcont h=image2.rows;
        tcont w=image2.cols;
        tcont cx=w*0.5, cy=h*0.5;

        //Dividing
        tcont segments[4][4]={{0,cx,0,cy},{cx,w,0,cy},{cx,w,cy,h},{0,cx,cy,h}};
        //Mask
        I axesx=w*0.75/2, axesy=h*0.75/2;
        Size size;
        size.height=axesx;
        size.width=axesy;
        Mat ellipmask=Mat(h,w,CV_8UC1,cvScalar(0.));
        Point center;
        center.x=cx; center.y=cy;
        ellipse(ellipmask,center,size,0,0,360,255,-1);

        for (tcont i=0;i<4;++i){
            Mat cornermask=Mat(h,w,CV_8UC1,cvScalar(0.));
            Point rectini, rectfin;
            rectini.x=segments[i][0];
            rectini.y=segments[i][2];
            rectfin.x=segments[i][1];
            rectfin.y=segments[i][3];

            rectangle(cornermask,rectini,rectfin,255,-1);
            subtract(cornermask,ellipmask,cornermask);

            //histogram
            MatND hist;


            histogram(image2,cornermask,hist);

            features.push_back(hist);
        }


        MatND hist;
        histogram(image2,ellipmask,hist);
        features.push_back(hist);
    }

    void histogram (Mat& image, Mat& mask, MatND& hist){

        //MatND hist;
        I channels[]={0,1,2};
        I histsize[]={bins[0],bins[1],bins[2]};
        F ranges1[] = { 0, 180 };
        F ranges2[] = { 0, 256 };
        F ranges3[] = { 0, 256 };
        const F *ranges[] = {ranges1,ranges2,ranges3};
        if(!mask.data){// || mask.type()==CV_8UC1){
            qDebug("no data");
            return;
        }

        calcHist(&image,1,channels,mask,hist,3,histsize,ranges,true,false);

        normalize(hist,hist);
    }

};


#endif // COLORDESC_H
