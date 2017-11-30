#include "cv.hpp"
#include "stdio.h"
#include "math.h"
struct INFO
{
  float i,j;
};
int main()
{
  
  Mat img1=imread("Roads.JPG");
  Mat img(img1.rows,img1.cols,CV_8UC1);
  cvtColor(img1,img,CV_BGR2GRAY);
  GaussianBlur(img, img, Size(7,7), 0, 0);
  Canny(img,img, 100,150, 3);
  imshow("sgh",img);
  INFO info[img.rows*img.cols];
  int n=0;
  for(int i=0;i<img.rows;i++)
    for(int j=0;j<img.cols;j++)
      if(img.at<uchar>(i,j)==255)
	{
	  info[n].i=(float)i*1.0;
	  info[n].j=(float)j*1.0;
	  n++;
	}
  int P=1000;
  int **vote=(int**)calloc(P,sizeof(int*));
  for(int i=0;i<1000;i++)
    vote[i]=(int*)calloc(361,sizeof(int));
  for(int i=0;i<n;i++)
    {
      float fi=0.0;
      for(int a=0;a<361;a++)
	{
	  float p=info[i].j*cos(fi*3.14/180.0)+info[i].i*sin(fi*3.14/180.0);
	  if(p>=0.0)
	    vote[(int)p][(int)fi]++;
	  fi=fi+1.0;
	}
    }
  int max[10];
  for(int i=0;i<10;i++)
    max[i]=0;
  int p_[10],fi_[10];
  for(int p=0;p<1000;p++)
    for(int fi=0;fi<361;fi++)
      for(int a=0;a<10;a++)
	  if(max[a]<vote[p][fi])
	    {
	      max[a]=vote[p][fi];
	      p_[a]=p;
	      fi_[a]=fi;
	      break;
	    }
  //Mat graph(1000,361,CV_8UC1,Scalar(0));
  //printf("**%d**",max);
  // for(int p=0;p<1000;p++)
  //   for(int fi=0;fi<361;fi++)
  //     if(vote[p][fi]>0)
  // 	{
  // 	  graph.at<uchar>(p+100,fi)=(int)((255.00*(float)vote[p][fi])/((float)max));
  // 	}
  // imshow("gas",graph);
  
  Mat line(img.rows,img.cols,CV_8UC1,Scalar(0));
  for(int a=0;a<10;a++)
    {
      for(int i=0;i<img.cols;i++)
	{
	  int y=(int)((float)p_[a]-(float)i*cos((float)fi_[a]*3.14/180.0))/sin((float)fi_[a]*3.14/180.0);
	  //for(int a=0;a<3;a++)
	  line.at<uchar>(y,i)=255;
	}
    }
  imshow("aer",line);
  waitKey(0);
  
  return 0;
}
