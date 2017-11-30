#include "cv.hpp"
#include "stdio.h"
#include "math.h"
struct INFO
{
  int i,j;//i=distance j=angle row and fi
  int dir;
};
struct horizon
{
	int vanish_x,vanish_y;
   	int left_x,left_y,right_x,right_y;			
};
int hough(Mat img,int section[],int term,horizon hor[]);
int isvalid(int p,int fi,int x,int y);
int main()
{
  
  Mat img1=imread("Roads.JPG");
  Mat img(img1.rows,img1.cols,CV_8UC1);
  cvtColor(img1,img,CV_BGR2GRAY);
  GaussianBlur(img, img, Size(7,7), 0, 0);
  Canny(img,img, 100,150, 3);
  int n=5;
  int *section=(int*)malloc(n*sizeof(int));
  for(int i=0;i<n-1;i++)
    {
      section[i]=img.rows/3+i*img.rows/8;
    }
  section[n-1]=img.rows;
  horizon hor[n];
  for(int i=0;i<n-1;i++)
    {
       Mat img_section(img.rows,img.cols,CV_8UC1,Scalar(0));
       Mat img_sec(img.rows,img.cols,CV_8UC1,Scalar(0));
      for(int y=section[i];y<section[i+1];y++)
	{
	  for(int x=0;x<img.cols;x++)
	    img_section.at<uchar>(y,x)= img.at<uchar>(y,x);	   
	}
      hough(img_section,section,i,hor);
    }
  return 0;
}
int hough(Mat img,int section[],int term,horizon hor[])
{ 
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
  int q=10;
  int max[10];
  for(int i=0;i<q;i++)
    max[i]=0;
  int fi_[10],p_[10];
  INFO inf[10];
  for(int a=0;a<q;a++)
    {
      for(int p=0;p<1000;p++)
  	for(int fi=0;fi<361;fi++)
  	  if(max[a]<vote[p][fi])
  	    {
  	      max[a]=vote[p][fi];
  	      p_[a]=p;
  	      fi_[a]=fi;
	      inf[a].i=p;
	      inf[a].j=fi;
  	    }
      for(int x=-10;x<10;x++)
  	{
  	  for(int y=-10;y<10;y++)
  	    {
  	      if( (isvalid(p_[a],fi_[a],x,y))==1)
  		vote[p_[a]+x][fi_[a]+y]=1;
  	    }
  	}
      
    }
  Mat line1(img.rows,img.cols,CV_8UC1,Scalar(0));
  // Mat line2(img.rows,img.cols,CV_8UC1,Scalar(0));
  for(int a=0;a<q;a++)
    {
      for(int i=0;i<img.cols;i++)
  	{
  	  if(p_[a]>=0||p_[a]<1000)
  	    {
	      int y=(int)((float)inf[a].i-(float)i*cos((float)inf[a].j*3.14/180.0))/sin((float)inf[a].j*3.14/180.0);
  	      //int y1=(int)((float)p_[a]-(float)i*cos((float)fi_[a]*3.14/180.0))/sin((float)fi_[a]*3.14/180.0);
  	      if(y>-1&&y<img.rows)
		{
		  line1.at<uchar>(y,i)=255;
		  //line2.at<uchar>(y1,i)=255;
		}
  	    }
  	}
    }
  imshow("ags",img);
  imshow("aer",line1);
  // imshow("auger",line2);
  for(int i=0;i<q;i++)
    {

      if((inf[i].j>=10&&inf[i].j<=80))
		inf[i].dir=0;
      if((inf[i].j>100&&inf[i].j<=170))
		inf[i].dir=1;
      if((inf[i].j>190&&inf[i].j<=260))
		inf[i].dir=2;
      if((inf[i].j>280&&inf[i].j<=350))
		inf[i].dir=3;
    }
  INFO nod[2];
  int small=361,large=0;
  for(int i=0;i<q;i++)
    {
      if(inf[i].dir==0)
	{
	  if(small>inf[i].j)
	    {
	      small=inf[i].j;
	      nod[1].j=inf[i].j;
	      nod[1].i=inf[i].i;
	    }
	}
      if(inf[i].dir==2)
	{
	  if(small>(inf[i].j-180))
	    {
	      small=(inf[i].j-180);
	      nod[1].j=inf[i].j;
	      nod[1].i=inf[i].i;
	    }
	}
    }
  for(int i=0;i<q;i++)
    {
      if(inf[i].dir==1)
	{
	  if(large<inf[i].j)
	    {
	      large=inf[i].j;
	      nod[0].j=inf[i].j;
	      nod[0].i=inf[i].i;
	    }
	}
      if(inf[i].dir==3)
	{
	  if(large<(inf[i].j-180))
	    {
	      large=(inf[i].j-180);
	      nod[0].j=inf[i].j;
	      nod[0].i=inf[i].i;
	    }
	}		
    }		
  int l=0;	
  Mat line(img.rows,img.cols,CV_8UC1,Scalar(0));
  for(int a=0;a<2;a++)
  {
      for(int i=0;i<img.cols;i++)
	    {
  	        int y=(int)((float)nod[a].i-(float)i*cos((float)nod[a].j*3.14/180.0))/sin((float)nod[a].j*3.14/180.0);
		if(y>-1&&y<img.rows)
		  line.at<uchar>(y,i)=255;
  	    }
  }
hor[term].vanish_x=((float)nod[0].i*sin((float)nod[1].j*3.14/180.0)-(float)nod[1].i*sin((float)nod[0].j*3.14/180.0))/sin((float)(nod[1].j-nod[0].j));
hor[term].vanish_y=((float)nod[0].i*cos((float)nod[1].j*3.14/180.0)-(float)nod[1].i*cos((float)nod[0].j*3.14/180.0))/sin((float)(nod[0].j-nod[1].j));

//Mat img_hor(line.rows,line.cols,CV_8UC1,Scalar(0));
//img_hor.at<uchar>(hor[term].vanish_y,hor[term].vanish_x)=255;
// printf("%d",term+1);
// printf("p=%d\tfi=%d\n",nod[0].i,nod[0].j);
// printf("p=%d\tfi=%d\n",nod[1].i,nod[1].j);
// //horizon(sec1,sec2);
// Mat img_point(img.rows,img.cols,CV_8UC1,Scalar(0));
// img_point.at<uchar>(98,219)=255;
// img_point.at<uchar>(98-1,219)=255;
// img_point.at<uchar>(98+1,219)=255;
// img_point.at<uchar>(98,219-1)=255;
// img_point.at<uchar>(98,219+1)=255;
// imshow("asdvsasd",img_point);
  //  imshow("ags",img);
//imshow("adds",img_hor);
  imshow("aedfsr",line);
  waitKey(0);
  //  printf("%d\t%d\n",nod[0].i,nod[0].j);  
  return 0;
}
int isvalid(int p,int fi,int x,int y)
{
  if(p+x<0||fi+y<0||p+x>999||fi+y>360)
    return 0;
  else
    return 1;
}
//4th  is 1st quadrant and 1st is 4th quadrent
 int horizon(INFO sec1[],INFO sec2[])
 {
 	




 }
