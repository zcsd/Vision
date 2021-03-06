#include <stdio.h>
#include <time.h>
#include "Vision.h"

#define NUMBER_OF_BLOBS 1 // get 1 biggest blob

#define FRAME_WIDTH 320
#define FRAME_HEIGHT 240

/////////////CUT WINDOW////////////////
#define LEFT_FLOOR_OBS 40  //FIRA OBS FLOOR
#define RIGHT_FLOOR_OBS 280
#define UP_FLOOR_OBS 60
#define DOWN_FLOOR_OBS 240

#define LEFT_FOLDER_OBS 0 //FIRA OBS FOLDER
#define RIGHT_FOLDER_OBS 320
#define UP_FOLDER_OBS 0
#define DOWN_FLODER_OBS 240

#define LEFT_ARROW_MARA 0 //FIRA MARA ARROW
#define RIGHT_ARROW_MARA 320
#define UP_ARROW_MARA 0
#define DOWN_ARROW_MARA 240

#define LEFT_LINE_MARA 0 //FIRA MARA LINE
#define RIGHT_LINE_MARA 320
#define UP_LINE_MARA 0
#define DOWN_LINE_MARA 240

#define LEFT_BALL_BSK 0  //FIRA BSK BALL
#define RIGHT_BALL_BSK 320
#define UP_BALL_BSK 0
#define DOWN_BALL_BSK 240

#define LEFT_BASKET_BSK 0 //FIRA BSK BASKET
#define RIGHT_BASKET_BSK 320
#define UP_BASKET_BSK 0
#define DOWN_BASKET_BSK 240
///////////////////////////////////////

//////////////BLOB SIZE////////////////
#define MIN_LINE_SIZE_MARA 0
#define MAX_LINE_SIZE_MARA 50000
#define MIN_ARROW_SIZE_MARA 100
#define MAX_ARROW_SIZE_MARA 20000

#define MIN_FLOOR_SIZE_OBS 100
#define MAX_FLOOR_SIZE_OBS 70000
#define MIN_REDFOLDER_SIZE_OBS 100
#define MAX_REDFOLDER_SIZE_OBS 50000

#define MIN_BALL_SIZE_BSK 50
#define MAX_BALL_SIZE_BSK 20000
#define MIN_BASKET_SIZE_BSK 50
#define MAX_BASKET_SIZE_BSK 20000
///////////////////////////////////////

////////////FIRA MARA/////////////////
#define ARROW_MIN_X 10 //Process arrow when
#define ARROW_MAX_X 310
#define ARROW_MIN_Y 100 //Important*****100
#define ARROW_MAX_Y 220 //Important****

#define ARROW_LEFT_MIN_RATIO 0.1 //Process arrow ratio
#define ARROW_LEFT_MAX_RATIO 0.3
#define ARROW_RIGHT_MIN_RATIO 0.7
#define ARROW_RIGHT_MAX_RATIO 0.9
#define ARROW_UP_RATIO 0.3
#define ARROW_DOWN_RATIO 0.7

#define DIFFERENCE_LR_MAX 20 //pixel differences between left and right 
#define DIFFERENCE_LR_MIN 10 // check go straight 

#define LINE_MINY_SEND 180 // if Line upper point smaller than 180, send line; greater than 180, send arrow
//////////////////////////////////////

////////////FIRA OBS/////////////////
#define HUE_MIN3 255
#define HUE_MAX3 255
#define SAT_MIN3 255
#define SAT_MAX3 255
#define LUM_MIN3 255
#define LUM_MAX3 255
/////////////////////////////////////

unsigned char min[3]= {0};
unsigned char max[3]= {0};
unsigned char min1[3]= {0};
unsigned char max1[3]= {0};
unsigned char min2[3]= {0};
unsigned char max2[3]= {0};
unsigned int Invert[3]={0};

unsigned int ArX_min=0,ArX_max=0,ArY_min=0,ArY_max=0;

int colorNo,countS,countL,countR,COUNT_RESET,COUNT_LINE=0;
char TuneMode=0;

char str[50]; 
char str1[50]; 
char str2[50]; 
char str3[50]; 
char str4[50];
char str5[50];
char str6[50];
char str7[50];
char str8[50];
char str9[50];
char str10[50];
char str11[50];

CvMemStorage *storage=0;
CvCapture *capture;
CvFont font;
IplImage *frame;

void LoadValueColor(VisionRange range, VisionRange range1, VisionRange range2)
{
	Invert[0] =range.invert;
	min[0]=range.Hmin;
	min[1]=range.Smin;
	min[2]=range.Lmin;
	max[0]=range.Hmax;
	max[1]=range.Smax;
	max[2]=range.Lmax;
	
	Invert[1] =range1.invert;
	min1[0]=range1.Hmin;
	min1[1]=range1.Smin;
	min1[2]=range1.Lmin;
	max1[0]=range1.Hmax;
	max1[1]=range1.Smax;
	max1[2]=range1.Lmax;
	
	Invert[2] =range2.invert;
	min2[0]=range2.Hmin;
	min2[1]=range2.Smin;
	min2[2]=range2.Lmin;
	max2[0]=range2.Hmax;
	max2[1]=range2.Smax;
	max2[2]=range2.Lmax;
}

void LoadColor(Color color)
{
	colorNo=color.set;
}

int VISION_InitCam()
{
	storage = cvCreateMemStorage( 0 );//setup memory buffer, needed by the face detector
	capture = cvCaptureFromCAM( CV_CAP_ANY );//initialize camera

	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	return(0);
}

void VISION_DestoryCam()
{
	cvReleaseCapture( &capture );
	cvReleaseMemStorage( &storage );
}

int VISION_GrabFrame() 
{
	frame = cvQueryFrame(capture);

	if( !frame )
	{
		fprintf( stderr, "Cannot query frame!\n" );
		return(0);
	}
	return(1);
}

int VISION_ShowOriginalFrame()
{
	cvShowImage("Original",frame);
//	cvMoveWindow("Original",830,100);
	return(1);
}

void VISION_EnableTuneMode()
{
	TuneMode = 1;
}

void VISION_DisableTuneMode()
{
	TuneMode = 0;
}

IplImage* RotateImage(IplImage* frame,IplImage* imgRotate,double degree,int rect_center_x,int rect_center_y)  
{   
    CvPoint2D32f center;    
    center.x=rect_center_x;  //img->width/2.0+0.5
    center.y=rect_center_y; // img->height/2.0+0.5
    float m[6];              
    CvMat M = cvMat( 2, 3, CV_32F, m );  
    cv2DRotationMatrix( center, degree,1, &M);  
    cvWarpAffine(frame,imgRotate, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0) ); 
    return imgRotate;   
} 
	
int VISION_Tune_Color1(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("color1");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)
	for(b=0;b<imgHSV->width;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])  )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( (H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
		
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",830,380);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>60000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
	
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}
	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
	}
	
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Tune_Color2(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("color2");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)
	for(b=0;b<imgHSV->width;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[1]==0)
		{
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])  )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( (H <= min1[0] || H >= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			{
				data_ts[a*step_ts+b]=255;	
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
		
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1);
	 
	cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",830,380);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>60000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}

		cnt2++;

		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;
					}
				}
				break;
			}
		}
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 180, 0), 2, 8, 0);
	}
		
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Tune_Color3(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("color3");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	for(a=0;a<imgHSV->height;a++)
	for(b=0;b<imgHSV->width;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[2]==0)
		{
			if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])  )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( (H <= min2[0] || H >= max2[0]) && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
			{
				data_ts[a*step_ts+b]=255;
				
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvShowImage("Threshold",imgThreshed);
//	cvMoveWindow("Threshold",830,380);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>60000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}

		cnt3++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea3[i])
			{
				maxArea3[i] = area1;
				contours3[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea3[m] < maxArea3[m+1])
					{
						tmp_area1 = maxArea3[m+1];
						tmp_cont = contours3[m+1];
						maxArea3[m+1] = maxArea3[m];
						contours3[m+1] = contours3[m];
						maxArea3[m] = tmp_area1;
						contours3[m] = tmp_cont;
					}
				}
				break;
			}
		}
	}
	
	if(cnt3 != 0)
	{
		CvRect rect = ((CvContour*)contours3[0])->rect;
		cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 0), 2, 8, 0);
	}
	
	VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_1Color(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game color1\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	cvZero(imgThreshed);
	
	for(a=0;a<imgHSV->height;a++)
	for(b=0;b<imgHSV->width;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		
		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			|| (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])   )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2])
			||((H >= min1[0] && H <= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			||((H <= min2[0] || H >= max2[0]) && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
			{
				data_ts[a*step_ts+b]=255;	
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	if(TuneMode)
	{
		cvShowImage("Threshold",imgThreshed);
//		cvMoveWindow("Threshold",830,380);
	}
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<100 || area1>50000 )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}
	
	}
	
	unsigned int rect_x, rect_y,rect_max_x,rect_max_y,rect_width;
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		rect_x = rect.x;
		rect_y = rect.y;
		rect_max_x = rect.x + rect.width;
		rect_max_y = rect.y + rect.height;
		rect_width = rect.width;
		printf("%d %d %d %d %d\n",rect_x,rect_y,rect_max_x,rect_max_y,rect_width);
		printf("width is %d\n",rect.width);
		
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);
		
		if(rect.y > 120 && rect.width <60) //Upper X smaller than 10
		{
			COUNT_LINE = COUNT_LINE + 1;
			printf("COUNT is %d\n",COUNT_LINE);
			cvCircle(frame,cvPoint(160,60),7,CV_RGB(0,0,255),3,8,0);
		}
		else
		{
			COUNT_LINE = 0;
		}
		
/*		if(COUNT_LINE > 30 && rect.width <60)
		{
			blob->Xmin = 251;
			blob->Xmax = 251;
			blob->Ymin = 251;
			blob->Ymax = 251;
			cvCircle(frame,cvPoint(160,120),7,CV_RGB(255,0,0),3,8,0);
		}
		else	
		{	
			blob->Xmin = rect.x / 2;
			blob->Xmax = (rect.x + rect.width) / 2;
			blob->Ymin = rect.y;
			blob->Ymax = rect.y + rect.height;
		}*/	
	}
	else
	{
		blob->Xmin = 251;
		blob->Xmax = 251;
		blob->Ymin = 251;
		blob->Ymax = 251;
		cvCircle(frame,cvPoint(160,120),7,CV_RGB(255,0,0),3,8,0);
	}
	
	if(cnt1!=0 && COUNT_LINE<15)
	{
		blob->Xmin = rect_x / 2;
		blob->Xmax = rect_max_x / 2;
		blob->Ymin = rect_y;
		blob->Ymax = rect_max_y;
	}
	
	if(cnt1!=0 && COUNT_LINE>=15)
	{
		blob->Xmin = 251;
		blob->Xmax = 251;
		blob->Ymin = 251;
		blob->Ymax = 251;
		cvCircle(frame,cvPoint(160,120),7,CV_RGB(255,0,0),3,8,0);
	}
			
	if(TuneMode)	
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);

	return(1);
}

int VISION_Game_OBS(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game OBS\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);  
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m,k,tmp,tmp1; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_FLOOR_OBS;a<DOWN_FLOOR_OBS;a++)
	for(b=LEFT_FLOOR_OBS;b<RIGHT_FLOOR_OBS;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];
		
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
			  ||(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
			  ||(H >= HUE_MIN3 && H <= HUE_MAX3 && S >= SAT_MIN3 && S<=SAT_MAX3 && V>=LUM_MIN3 && V<=LUM_MAX3))
			{
				data_ts[a*step_ts+b]=0;//Floor,black
			}
			else
			{
				data_ts[a*step_ts+b]=255;//others
			}		
	}
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
//	cvDilate(imgThreshed,imgThreshed,NULL,1);
//	cvErode(imgThreshed,imgThreshed,NULL,1); 

	if(TuneMode)
		cvShowImage("Floor",imgThreshed1);
//	cvMoveWindow("Floor",850,350);
	int max_y;
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_FLOOR_SIZE_OBS || area1>MAX_FLOOR_SIZE_OBS )
		{
			cvSeqRemove(contour,0);
			continue;
		}

			cnt1++;
			
			for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
			{
				if(area1 > maxArea1[i])
				{
					maxArea1[i] = area1;
					contours1[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea1[m] < maxArea1[m+1])
						{
							tmp_area1 = maxArea1[m+1];
							tmp_cont = contours1[m+1];
							maxArea1[m+1] = maxArea1[m];
							contours1[m+1] = contours1[m];
							maxArea1[m] = tmp_area1;
							contours1[m] = tmp_cont;
						}
					}
					break;
				}
			}
			
		max_y=0;
		
		for(k=0;k<contours1[0]->total;++k)
		{
			pt=(CvPoint *)cvGetSeqElem(contours1[0],k);
			tmp=pt->y;
			if(tmp>max_y)
			{
				max_y=tmp;
			}
		}
		
		tmp1=max_y;		
			
	}	
	
	cvZero(imgThreshed);
	//For second color,red color folder
	for(a=UP_FOLDER_OBS;a<DOWN_FLODER_OBS;a++)
	for(b=LEFT_FOLDER_OBS;b<RIGHT_FOLDER_OBS;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;

			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
				
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Red_Folder",imgThreshed);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_REDFOLDER_SIZE_OBS || area1>MAX_REDFOLDER_SIZE_OBS )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt2++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;
					}
				}
				break;
			}
		}
	}
			
	if(cnt1 != 0)
	{
		if(TuneMode)
			cvRectangle(frame, cvPoint(0, tmp1), cvPoint(320, 240),CV_RGB(0, 0, 255), 2, 8, 0);

		blob->Xmin = LEFT_FLOOR_OBS / 2;
		blob->Xmax = RIGHT_FLOOR_OBS / 2; //320/2
		blob->Ymin = tmp1;
		blob->Ymax = DOWN_FLOOR_OBS;
	}
	else
	{
		blob->Xmin = LEFT_FLOOR_OBS / 2;
		blob->Xmax = RIGHT_FLOOR_OBS / 2;//320/2
		blob->Ymin = UP_FLOOR_OBS;
		blob->Ymax = DOWN_FLOOR_OBS;
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 255, 0), 2, 8, 0);

		blob1->Xmin = rect.x / 2;
		blob1->Xmax = (rect.x + rect.width) / 2;
		blob1->Ymin = rect.y;
		blob1->Ymax = rect.y + rect.height;
	}
	else
	{
		blob1->Xmin = 255;
		blob1->Xmax = 255;
		blob1->Ymin = 255;
		blob1->Ymax = 255;
	}	
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);
	
	return(1);
}

int VISION_Game_BSK(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,BlobCoord *blob1,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.5f, 0.5f,0,2,8);
//	printf("Game Basketball\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,l=0,i,m; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_BALL_BSK;a<DOWN_BALL_BSK;a++)
	for(b=LEFT_BALL_BSK;b<RIGHT_BALL_BSK;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;

			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;	
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
//	cvDilate(imgThreshed,imgThreshed,NULL,1);
//	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
	if(TuneMode)
		cvShowImage("Ball",imgThreshed1);
//	cvMoveWindow("Threshold",850,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_BALL_SIZE_BSK || area1>MAX_BALL_SIZE_BSK )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}
			
	
	}
	
	cvZero(imgThreshed);
	//For second color
	for(a=UP_BASKET_BSK;a<DOWN_BASKET_BSK;a++)
	for(b=LEFT_BASKET_BSK;b<RIGHT_BASKET_BSK;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[1]==0)
		{
			if( (H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]))
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min1[0] || H >= max1[0]) && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2]) )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Basket",imgThreshed);
		
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
		
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_BASKET_SIZE_BSK || area1>MAX_BASKET_SIZE_BSK )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt2++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;
					}
				}
				break;
			}
		}
	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);

		blob->Xmin = rect.x / 2;
		blob->Xmax = (rect.x + rect.width) / 2;
		blob->Ymin = rect.y;
		blob->Ymax = rect.y + rect.height;
		
	}
	else
	{
		blob->Xmin = 255;
		blob->Xmax = 255;
		blob->Ymin = 255;
		blob->Ymax = 255;
	}
	
	if(cnt2 != 0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 255, 0), 2, 8, 0);

		blob1->Xmin = rect.x / 2;
		blob1->Xmax = (rect.x + rect.width) / 2;
		blob1->Ymin = rect.y;
		blob1->Ymax = rect.y + rect.height;
	}
	else
	{
		blob1->Xmin = 255;
		blob1->Xmax = 255;
		blob1->Ymin = 255;
		blob1->Ymax = 255;
	}
		
	if(TuneMode)
	{
		sprintf(str,"%d",blob->Xmin);
		sprintf(str1,"%d",blob->Xmax);
		sprintf(str2,"%d",blob->Ymin);
		sprintf(str3,"%d",blob->Ymax);
		sprintf(str4,"%d",(blob->Xmax+blob->Xmin)/2);
		sprintf(str5,"%d",(blob->Ymax+blob->Ymin)/2);
		sprintf(str6,"%d",blob1->Xmin);
		sprintf(str7,"%d",blob1->Xmax);
		sprintf(str8,"%d",blob1->Ymin);
		sprintf(str9,"%d",blob1->Ymax);
		sprintf(str10,"%d",(blob1->Xmax+blob1->Xmin)/2);
		sprintf(str11,"%d",(blob1->Ymax+blob1->Ymin)/2);
		
		
		cvPutText(frame, "X1", cvPoint(5,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "X2", cvPoint(45,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y1", cvPoint(85,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y2", cvPoint(125,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "CX", cvPoint(165,20), &font, CV_RGB(255,0,0));
		cvPutText(frame, "CY", cvPoint(205,20), &font, CV_RGB(255,0,0));
		
		cvPutText(frame, "X1", cvPoint(5,220), &font, CV_RGB(255,0,0));
		cvPutText(frame, "X2", cvPoint(45,220), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y1", cvPoint(85,220), &font, CV_RGB(255,0,0));
		cvPutText(frame, "Y2", cvPoint(125,220), &font, CV_RGB(255,0,0));
		cvPutText(frame, "CX", cvPoint(165,220), &font, CV_RGB(255,0,0));
		cvPutText(frame, "CY", cvPoint(205,220), &font, CV_RGB(255,0,0));
		
		cvPutText(frame, str, cvPoint(5,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str1, cvPoint(45,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str2, cvPoint(85,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str3, cvPoint(125,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str4, cvPoint(165,40), &font, CV_RGB(255,0,0));
		cvPutText(frame, str5, cvPoint(205,40), &font, CV_RGB(255,0,0));
		
		cvPutText(frame, str6, cvPoint(5,240), &font, CV_RGB(255,0,0));
		cvPutText(frame, str7, cvPoint(45,240), &font, CV_RGB(255,0,0));
		cvPutText(frame, str8, cvPoint(85,240), &font, CV_RGB(255,0,0));
		cvPutText(frame, str9, cvPoint(125,240), &font, CV_RGB(255,0,0));
		cvPutText(frame, str10, cvPoint(165,240), &font, CV_RGB(255,0,0));
		cvPutText(frame, str11, cvPoint(205,240), &font, CV_RGB(255,0,0));
		
		VISION_ShowOriginalFrame();
	}
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);

	return(1);	
}

//Find minareaRect, Rotate and compare left right
int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game Arrow\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgRotate=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgRotate_HSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,i,m,l=0; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	int corner_X[4],corner_Y[4],cntcorner1=0,cntcorner2=0,cntcorner3=0,cntcorner4=0,xxxx=0;
	
	int LnX_min=0,LnX_max=0,LnY_min=0,LnY_max=0;
	int flag=0,flag1;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	unsigned char *data_hsv1 = (unsigned char *)imgRotate_HSV->imageData;
	int step_hsv1 = imgRotate_HSV->widthStep/sizeof(unsigned char);
	int chanels_hsv1 = imgRotate_HSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_ARROW_MARA;a<DOWN_ARROW_MARA;a++)
	for(b=LEFT_ARROW_MARA;b<RIGHT_ARROW_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if((H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
		|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]) )
		{
			data_ts[a*step_ts+b]=255;
		}
		else
		{
			data_ts[a*step_ts+b]=0;
		}
	}
	
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
	if(TuneMode)
		cvShowImage("Arrow",imgThreshed1);
//	cvMoveWindow("Threshold",500,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_ARROW_SIZE_MARA || area1>MAX_ARROW_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
	
			cnt2++;
			
			for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
			{
				if(area1 > maxArea2[i])
				{
					maxArea2[i] = area1;
					contours2[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea2[m] < maxArea2[m+1])
						{
							tmp_area1 = maxArea2[m+1];
							tmp_cont = contours2[m+1];
							maxArea2[m+1] = maxArea2[m];
							contours2[m+1] = contours2[m];
							maxArea2[m] = tmp_area1;
							contours2[m] = tmp_cont;
						}
					}
					break;
				}
			}
	}
	
	cvZero(imgThreshed);
	
	for(a=UP_LINE_MARA;a<DOWN_LINE_MARA;a++)
	for(b=LEFT_LINE_MARA;b<RIGHT_LINE_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Line",imgThreshed);
		
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_LINE_SIZE_MARA || area1>MAX_LINE_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);

		LnX_min = rect.x / 2;
		LnX_max = (rect.x + rect.width) / 2;
		LnY_min = rect.y;
		LnY_max = rect.y + rect.height;
	}

	int COUNT_LEFT=0,COUNT_RIGHT=0,Difference_LR;
	CvSeq* result;
	int f,rect_center_x,rect_center_y;
	double tangent,degree;
	int rect_width,rect_height,rect_temp1,rect_temp2,rect_rotate_x,rect_rotate_y;
	float WD2HG;
	int rect_max_x,rect_max_y;
	
	if(cnt2==0 && cnt1!=0)//if only see line,reset to 0
	{	
		countL=0;
		countR=0;
		countS=0;
	}
	
/*	if(COUNT_RESET==0 && cnt1!=0)//if only see line,reset to 0
	{
		countL=0;
		countR=0;
		countS=0;
	}
*/	
//	COUNT_RESET=0;	
	if(cnt2 != 0 && cnt1 !=0)//  see line and arrow at the same time
	{
		CvBox2D rect=cvMinAreaRect2(contours2[0],storage);
		CvPoint2D32f rect_pts0[4];
		cvBoxPoints(rect,rect_pts0);
		int npts=4;
		CvPoint rect_pts[4],*pt1=rect_pts;
		
		for(a=0;a<4;a++)
		{
			rect_pts[a]=cvPointFrom32f(rect_pts0[a]);
		}	
		
		cvPolyLine(frame,&pt1,&npts,1,1,CV_RGB(255,255,255),2,8,0);

		CvPoint pt2[2];		

		rect_center_x = (pt1[0].x + pt1[2].x)/2;
		rect_center_y = (pt1[0].y + pt1[2].y)/2;
		
		rect_temp1=sqrt((pt1[0].y-pt1[1].y)*(pt1[0].y-pt1[1].y)+(pt1[0].x-pt1[1].x)*(pt1[0].x-pt1[1].x));
		rect_temp2=sqrt((pt1[1].y-pt1[2].y)*(pt1[1].y-pt1[2].y)+(pt1[1].x-pt1[2].x)*(pt1[1].x-pt1[2].x));
		
		if(rect_temp1>=rect_temp2)
		{
			rect_width = rect_temp1;
			rect_height = rect_temp2;
		}
		else
		{
			rect_width = rect_temp2;
			rect_height = rect_temp1;	
		}
		
//		printf("Rect width = %d, Rect height = %d\n",rect_width,rect_height);
		
		rect_rotate_x = rect_center_x - rect_width/2;
		rect_rotate_y = rect_center_y - rect_height/2;
		WD2HG = rect_width / rect_height;
		rect_max_x = rect_rotate_x + rect_width;
		rect_max_y = rect_rotate_y + rect_height;
		
//		printf("new x,y is (%d, %d)\n",rect_rotate_x,rect_rotate_y);
		if(WD2HG>0.4 && WD2HG<1.8 && rect_rotate_x>ARROW_MIN_X && rect_rotate_y>ARROW_MIN_Y && rect_max_x<ARROW_MAX_X && rect_max_y<ARROW_MAX_Y)
		{
			if(pt1[0].y >= pt1[1].y && pt1[0].y >= pt1[2].y && pt1[0].y >= pt1[3].y)
			{
				pt2[0].x=pt1[0].x;
				pt2[0].y=pt1[0].y;
				if(pt1[1].y >= pt1[3].y)
				{
					pt2[1].x=pt1[1].x;
					pt2[1].y=pt1[1].y;
				}
				else
				{
					pt2[1].x=pt1[3].x;
					pt2[1].y=pt1[3].y;
				}		
			}
			
			if(pt1[1].y >= pt1[0].y && pt1[1].y >= pt1[2].y && pt1[1].y >= pt1[3].y)
			{
				pt2[0].x=pt1[1].x;
				pt2[0].y=pt1[1].y;
				if(pt1[0].y >= pt1[2].y)
				{
					pt2[1].x=pt1[0].x;
					pt2[1].y=pt1[0].y;
				}
				else
				{
					pt2[1].x=pt1[2].x;
					pt2[1].y=pt1[2].y;
				}
				
			}
			
			if(pt1[2].y >= pt1[0].y && pt1[2].y >= pt1[1].y && pt1[2].y >= pt1[3].y)
			{
				pt2[0].x=pt1[2].x;
				pt2[0].y=pt1[2].y;
				if(pt1[1].y >= pt1[3].y)
				{
					pt2[1].x=pt1[1].x;
					pt2[1].y=pt1[1].y;
				}
				else
				{
					pt2[1].x=pt1[3].x;
					pt2[1].y=pt1[3].y;
				}
			}
			
			if(pt1[3].y >= pt1[0].y && pt1[3].y >= pt1[1].y && pt1[3].y >= pt1[2].y)
			{
				pt2[0].x=pt1[3].x;
				pt2[0].y=pt1[3].y;
				if(pt1[0].y >= pt1[2].y)
				{
					pt2[1].x=pt1[0].x;
					pt2[1].y=pt1[0].y;
				}
				else
				{
					pt2[1].x=pt1[2].x;
					pt2[1].y=pt1[2].y;
				}
			}
			
			tangent= ((pt2[0].y-pt2[1].y)*0.01)/((pt2[0].x-pt2[1].x)*0.01);
//			printf("Tangent is %f\n", tangent);	
//			cvCircle(frame,pt2[0],7,CV_RGB(0,0,255),2,8,0);
//			cvCircle(frame,pt2[1],7,CV_RGB(0,255,0),2,8,0);
			degree=tangent*180/3.1415926;
//			printf("degree is %0.2f\n",degree);
			
			imgRotate = RotateImage(frame,imgRotate, degree,rect_center_x,rect_center_y);		
			
			cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			
			cvCvtColor(imgRotate, imgRotate_HSV, CV_BGR2HSV_FULL);
			
			for(a=rect_rotate_y+rect_height*ARROW_UP_RATIO;a<rect_rotate_y+rect_height*ARROW_DOWN_RATIO;a++)
			for(b=rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO;b<rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO;b++)
			{
				H = data_hsv1[a*step_hsv1+b*chanels_hsv1+0];
				S = data_hsv1[a*step_hsv1+b*chanels_hsv1+1];
				V = data_hsv1[a*step_hsv1+b*chanels_hsv1+2];

				if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
				{
					COUNT_LEFT = COUNT_LEFT + 1;
				}
			}
			
			for(a=rect_rotate_y+rect_height*ARROW_UP_RATIO;a<rect_rotate_y+rect_height*ARROW_DOWN_RATIO;a++)
			for(b=rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO;b<rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO;b++)
			{
				H = data_hsv1[a*step_hsv1+b*chanels_hsv1+0];
				S = data_hsv1[a*step_hsv1+b*chanels_hsv1+1];
				V = data_hsv1[a*step_hsv1+b*chanels_hsv1+2];

				if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
				{
					COUNT_RIGHT = COUNT_RIGHT + 1;
				}
			}
			
			Difference_LR= COUNT_LEFT - COUNT_RIGHT;
			if(Difference_LR < 0)
				Difference_LR = (-1)*Difference_LR;
				
			if(COUNT_LEFT > COUNT_RIGHT && Difference_LR > DIFFERENCE_LR_MAX)
			{
//				printf("Turn Left\n");
				countL=countL+1;
			}
			else
			if(COUNT_RIGHT > COUNT_LEFT && Difference_LR > DIFFERENCE_LR_MAX)
			{
//				printf("Turn Right\n");
				countR=countR+1;
			}
			else
			if(Difference_LR < DIFFERENCE_LR_MIN)
			{
//				printf("Go Straight\n");
				countS=countS+1;
			}
//			COUNT_RESET=1;
		}	
		
		if(countL>countR && countL>countS)
		{
			cvCircle(frame,cvPoint(40,120),7,CV_RGB(255,0,0),3,8,0);
//				printf("Turn Left\n");
			flag = 1;
			flag1 = 1;
			ArX_min = 40;
			ArX_max = 40;
			ArY_min = 110;
			ArY_max = 130;
		}
		
		if(countR>countL && countR>countS)
		{   
			cvCircle(frame,cvPoint(280,120),7,CV_RGB(255,0,0),3,8,0);
//				printf("Turn Right\n");
			flag = 1;
			flag1 = 1;
			ArX_min = 120;
			ArX_max = 120;
			ArY_min = 110;
			ArY_max = 130;		
		}
		
		if(countS>countL && countS>countR)
		{	
			cvCircle(frame,cvPoint(160,20),7,CV_RGB(255,0,0),3,8,0);
//				printf("Go Straight\n");
			flag = 1;
			flag1 = 1;
			ArX_min = 80;
			ArX_max = 80;
			ArY_min = 110;
			ArY_max = 130;		
		}	
	}
	
	if(TuneMode)
		cvShowImage("Rotate",imgRotate);	
	
	if(cnt1 != 0)//have Line
	{
		if(flag==0)// no arrow
		{
			blob->Xmin = LnX_min;
			blob->Xmax = LnX_max;
			blob->Ymin = LnY_min;
			blob->Ymax = LnY_max;
			ArX_min=255;
			ArX_max=255;
			ArY_min=255;
			ArY_max=255;	
		}
		else  //have arrow
		{
			if(LnY_min < 180) //upper point smaller than 200, send Line
			{
				blob->Xmin = LnX_min;
				blob->Xmax = LnX_max;
				blob->Ymin = LnY_min;
				blob->Ymax = LnY_max;	
			}
			else// upper point greater than 200, send Arrow
			{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
			}
		}
	}
	else  //no line
	{
		if(flag == 0)//No arrow
		{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;				
		}
		else  //have arrow
		{
			blob->Xmin = ArX_min;
			blob->Xmax = ArX_max;
			blob->Ymin = ArY_min;
			blob->Ymax = ArY_max;	
		}		
	}
	
	printf("Left:%d, Right:%d,Straight:%d\n",countL,countR,countS);
	flag = 0;
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgRotate);
	cvReleaseImage(&imgRotate_HSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);

	return(1);
}
/*
//Find poly, Rotate and compare left right
int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game Arrow\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgRotate=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgRotate_HSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,i,m,l=0; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	int corner_X[4],corner_Y[4],cntcorner1=0,cntcorner2=0,cntcorner3=0,cntcorner4=0,xxxx=0;
	
	int LnX_min=0,LnX_max=0,LnY_min=0,LnY_max=0;
	int flag=0,flag1;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	unsigned char *data_hsv1 = (unsigned char *)imgRotate_HSV->imageData;
	int step_hsv1 = imgRotate_HSV->widthStep/sizeof(unsigned char);
	int chanels_hsv1 = imgRotate_HSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_ARROW_MARA;a<DOWN_ARROW_MARA;a++)
	for(b=LEFT_ARROW_MARA;b<RIGHT_ARROW_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if((H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
		|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]) )
		{
			data_ts[a*step_ts+b]=255;
		}
		else
		{
			data_ts[a*step_ts+b]=0;
		}
	}
	
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
	if(TuneMode)
		cvShowImage("Arrow",imgThreshed1);
//	cvMoveWindow("Threshold",500,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_ARROW_SIZE_MARA || area1>MAX_ARROW_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
	
			cnt2++;
			
			for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
			{
				if(area1 > maxArea2[i])
				{
					maxArea2[i] = area1;
					contours2[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea2[m] < maxArea2[m+1])
						{
							tmp_area1 = maxArea2[m+1];
							tmp_cont = contours2[m+1];
							maxArea2[m+1] = maxArea2[m];
							contours2[m+1] = contours2[m];
							maxArea2[m] = tmp_area1;
							contours2[m] = tmp_cont;
						}
					}
					break;
				}
			}
	}
	
	cvZero(imgThreshed);
	
	for(a=UP_LINE_MARA;a<DOWN_LINE_MARA;a++)
	for(b=LEFT_LINE_MARA;b<RIGHT_LINE_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Line",imgThreshed);
		
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_LINE_SIZE_MARA || area1>MAX_LINE_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);

		LnX_min = rect.x / 2;
		LnX_max = (rect.x + rect.width) / 2;
		LnY_min = rect.y;
		LnY_max = rect.y + rect.height;
	}

	int COUNT_LEFT=0,COUNT_RIGHT=0,Difference_LR;
	CvSeq* result;
	int f,rect_center_x,rect_center_y;
	double tangent,degree;
	int rect_width,rect_height,rect_temp1,rect_temp2,rect_rotate_x,rect_rotate_y;
	float WD2HG;
	int rect_max_x,rect_max_y;
	
//	if(cnt2==0 && cnt1!=0)//if only see line,reset to 0
//	{	
//		countL=0;
//		countR=0;
//		countS=0;
//	}
	
	if(COUNT_RESET==0 && cnt1!=0)//if only see line,reset to 0
	{
		countL=0;
		countR=0;
		countS=0;
	}
	
	COUNT_RESET=0;	
	if(cnt2 != 0)// && cnt1 !=0 see line and arrow at the same time
	{
		result = cvApproxPoly(contours2[0],sizeof(CvContour),storage,CV_POLY_APPROX_DP,cvContourPerimeter(contours2[0])*0.085,0);
//		printf("%d\n",result->total);//cvContourPerimeter(contours2[0])*0.02
		if(result->total == 4)
		{
			CvPoint *pt1[4],pt2[2];
			for(f=0;f<4;f++)
			{
				pt1[f]=(CvPoint*)cvGetSeqElem(result,f);
			}
			
			if(TuneMode)
			{
				cvLine(frame,*pt1[0],*pt1[1],CV_RGB(0,255,0),2,8,0);
				cvLine(frame,*pt1[1],*pt1[2],CV_RGB(0,255,0),2,8,0);
				cvLine(frame,*pt1[2],*pt1[3],CV_RGB(0,255,0),2,8,0);
				cvLine(frame,*pt1[3],*pt1[0],CV_RGB(0,255,0),2,8,0);
			}
			

			rect_center_x = (pt1[0]->x + pt1[2]->x)/2;
			rect_center_y = (pt1[0]->y + pt1[2]->y)/2;
			
			rect_temp1=sqrt((pt1[0]->y-pt1[1]->y)*(pt1[0]->y-pt1[1]->y)+(pt1[0]->x-pt1[1]->x)*(pt1[0]->x-pt1[1]->x));
			rect_temp2=sqrt((pt1[1]->y-pt1[2]->y)*(pt1[1]->y-pt1[2]->y)+(pt1[1]->x-pt1[2]->x)*(pt1[1]->x-pt1[2]->x));
			
			if(rect_temp1>=rect_temp2)
			{
				rect_width = rect_temp1;
				rect_height = rect_temp2;
			}
			else
			{
				rect_width = rect_temp2;
				rect_height = rect_temp1;	
			}
			
//			printf("Rect width = %d, Rect height = %d\n",rect_width,rect_height);
			
			rect_rotate_x = rect_center_x - rect_width/2;
			rect_rotate_y = rect_center_y - rect_height/2;
			WD2HG = rect_width / rect_height;
			rect_max_x = rect_rotate_x + rect_width;
			rect_max_y = rect_rotate_y + rect_height;
			
//			printf("new x,y is (%d, %d)\n",rect_rotate_x,rect_rotate_y);
				
			if(WD2HG>0.4 && WD2HG<1.8 && rect_rotate_x>ARROW_MIN_X && rect_rotate_y>ARROW_MIN_Y && rect_max_x<ARROW_MAX_X && rect_max_y<ARROW_MAX_Y)
			{
				if(pt1[0]->y >= pt1[1]->y && pt1[0]->y >= pt1[2]->y && pt1[0]->y >= pt1[3]->y)
				{
					pt2[0].x=pt1[0]->x;
					pt2[0].y=pt1[0]->y;
					if(pt1[1]->y >= pt1[3]->y)
					{
						pt2[1].x=pt1[1]->x;
						pt2[1].y=pt1[1]->y;
					}
					else
					{
						pt2[1].x=pt1[3]->x;
						pt2[1].y=pt1[3]->y;
					}		
				}
				
				if(pt1[1]->y >= pt1[0]->y && pt1[1]->y >= pt1[2]->y && pt1[1]->y >= pt1[3]->y)
				{
					pt2[0].x=pt1[1]->x;
					pt2[0].y=pt1[1]->y;
					if(pt1[0]->y >= pt1[2]->y)
					{
						pt2[1].x=pt1[0]->x;
						pt2[1].y=pt1[0]->y;
					}
					else
					{
						pt2[1].x=pt1[2]->x;
						pt2[1].y=pt1[2]->y;
					}
					
				}
				
				if(pt1[2]->y >= pt1[0]->y && pt1[2]->y >= pt1[1]->y && pt1[2]->y >= pt1[3]->y)
				{
					pt2[0].x=pt1[2]->x;
					pt2[0].y=pt1[2]->y;
					if(pt1[1]->y >= pt1[3]->y)
					{
						pt2[1].x=pt1[1]->x;
						pt2[1].y=pt1[1]->y;
					}
					else
					{
						pt2[1].x=pt1[3]->x;
						pt2[1].y=pt1[3]->y;
					}
				}
				
				if(pt1[3]->y >= pt1[0]->y && pt1[3]->y >= pt1[1]->y && pt1[3]->y >= pt1[2]->y)
				{
					pt2[0].x=pt1[3]->x;
					pt2[0].y=pt1[3]->y;
					if(pt1[0]->y >= pt1[2]->y)
					{
						pt2[1].x=pt1[0]->x;
						pt2[1].y=pt1[0]->y;
					}
					else
					{
						pt2[1].x=pt1[2]->x;
						pt2[1].y=pt1[2]->y;
					}
				}
				
				tangent= ((pt2[0].y-pt2[1].y)*0.01)/((pt2[0].x-pt2[1].x)*0.01);
//				printf("Tangent is %f\n", tangent);	
//				cvCircle(frame,pt2[0],7,CV_RGB(0,0,255),2,8,0);
//				cvCircle(frame,pt2[1],7,CV_RGB(0,255,0),2,8,0);
				degree=tangent*180/3.1415926;
//				printf("degree is %0.2f\n",degree);
				
				imgRotate = RotateImage(frame,imgRotate, degree,rect_center_x,rect_center_y);		
				
				cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
				cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
				cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
				cvLine(imgRotate,cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_UP_RATIO),cvPoint(rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO, rect_rotate_y+rect_height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
				
				cvCvtColor(imgRotate, imgRotate_HSV, CV_BGR2HSV_FULL);
				
				for(a=rect_rotate_y+rect_height*ARROW_UP_RATIO;a<rect_rotate_y+rect_height*ARROW_DOWN_RATIO;a++)
				for(b=rect_rotate_x+rect_width*ARROW_LEFT_MIN_RATIO;b<rect_rotate_x+rect_width*ARROW_LEFT_MAX_RATIO;b++)
				{
					H = data_hsv1[a*step_hsv1+b*chanels_hsv1+0];
					S = data_hsv1[a*step_hsv1+b*chanels_hsv1+1];
					V = data_hsv1[a*step_hsv1+b*chanels_hsv1+2];

					if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
					{
						COUNT_LEFT = COUNT_LEFT + 1;
					}
				}
				
				for(a=rect_rotate_y+rect_height*ARROW_UP_RATIO;a<rect_rotate_y+rect_height*ARROW_DOWN_RATIO;a++)
				for(b=rect_rotate_x+rect_width*ARROW_RIGHT_MIN_RATIO;b<rect_rotate_x+rect_width*ARROW_RIGHT_MAX_RATIO;b++)
				{
					H = data_hsv1[a*step_hsv1+b*chanels_hsv1+0];
					S = data_hsv1[a*step_hsv1+b*chanels_hsv1+1];
					V = data_hsv1[a*step_hsv1+b*chanels_hsv1+2];

					if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
					{
						COUNT_RIGHT = COUNT_RIGHT + 1;
					}
				}
				
				Difference_LR= COUNT_LEFT - COUNT_RIGHT;
				if(Difference_LR < 0)
					Difference_LR = (-1)*Difference_LR;
					
				if(COUNT_LEFT > COUNT_RIGHT && Difference_LR > DIFFERENCE_LR_MAX)
				{
	//				printf("Turn Left\n");
					countL=countL+1;
				}
				else
				if(COUNT_RIGHT > COUNT_LEFT && Difference_LR > DIFFERENCE_LR_MAX)
				{
	//				printf("Turn Right\n");
					countR=countR+1;
				}
				else
				if(Difference_LR < DIFFERENCE_LR_MIN)
				{
	//				printf("Go Straight\n");
					countS=countS+1;
				}
				
				COUNT_RESET=1;
			}
			
			if(countL>countR && countL>countS)
			{
				cvCircle(frame,cvPoint(40,120),7,CV_RGB(255,0,0),3,8,0);
//				printf("Turn Left\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 40;
				ArX_max = 40;
				ArY_min = 110;
				ArY_max = 130;
			}
			
			if(countR>countL && countR>countS)
			{   
				cvCircle(frame,cvPoint(280,120),7,CV_RGB(255,0,0),3,8,0);
//				printf("Turn Right\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 120;
				ArX_max = 120;
				ArY_min = 110;
				ArY_max = 130;		
			}
			
			if(countS>countL && countS>countR)
			{	
				cvCircle(frame,cvPoint(160,20),7,CV_RGB(255,0,0),3,8,0);
//				printf("Go Straight\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 80;
				ArX_max = 80;
				ArY_min = 110;
				ArY_max = 130;		
			}
		}	
	}
	
	if(TuneMode)
		cvShowImage("Rotate",imgRotate);	
	
	if(cnt1 != 0)//have Line
	{
		if(flag==0)// no arrow
		{
			blob->Xmin = LnX_min;
			blob->Xmax = LnX_max;
			blob->Ymin = LnY_min;
			blob->Ymax = LnY_max;
			ArX_min=255;
			ArX_max=255;
			ArY_min=255;
			ArY_max=255;	
		}
		else  //have arrow
		{
			if(LnY_min < 180) //upper point smaller than 200, send Line
			{
				blob->Xmin = LnX_min;
				blob->Xmax = LnX_max;
				blob->Ymin = LnY_min;
				blob->Ymax = LnY_max;	
			}
			else// upper point greater than 200, send Arrow
			{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
			}
		}
	}
	else  //no line
	{
		if(flag == 0)//No arrow
		{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;				
		}
		else  //have arrow
		{
			blob->Xmin = ArX_min;
			blob->Xmax = ArX_max;
			blob->Ymin = ArY_min;
			blob->Ymax = ArY_max;	
		}		
	}
	
	printf("Left:%d, Right:%d,Straight:%d\n",countL,countR,countS);
	flag = 0;
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgRotate);
	cvReleaseImage(&imgRotate_HSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);

	return(1);
}
*/
//Draw Rectangular and compare left right, no rotation
/*
int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game Arrow\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,i,m,l=0; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	int corner_X[4],corner_Y[4],cntcorner1=0,cntcorner2=0,cntcorner3=0,cntcorner4=0,xxxx=0;
	
	int LnX_min=0,LnX_max=0,LnY_min=0,LnY_max=0;
	int flag=0,flag1;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_ARROW_MARA;a<DOWN_ARROW_MARA;a++)
	for(b=LEFT_ARROW_MARA;b<RIGHT_ARROW_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if((H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
		|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]) )
		{
			data_ts[a*step_ts+b]=255;
		}
		else
		{
			data_ts[a*step_ts+b]=0;
		}
	}
//	cvErode(imgThreshed,imgThreshed,NULL,1);	
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
	if(TuneMode)
		cvShowImage("Arrow",imgThreshed1);
//	cvMoveWindow("Threshold",500,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_ARROW_SIZE_MARA || area1>MAX_ARROW_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
	
		cnt2++;
			
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea2[i])
			{
				maxArea2[i] = area1;
				contours2[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea2[m] < maxArea2[m+1])
					{
						tmp_area1 = maxArea2[m+1];
						tmp_cont = contours2[m+1];
						maxArea2[m+1] = maxArea2[m];
						contours2[m+1] = contours2[m];
						maxArea2[m] = tmp_area1;
						contours2[m] = tmp_cont;
					}
				}
				break;
			}
		}
	}
	
	cvZero(imgThreshed);
	
	for(a=UP_LINE_MARA;a<DOWN_LINE_MARA;a++)
	for(b=LEFT_LINE_MARA;b<RIGHT_LINE_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Line",imgThreshed);
		
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_LINE_SIZE_MARA || area1>MAX_LINE_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 255, 20), 2, 8, 0);

		LnX_min = rect.x / 2;
		LnX_max = (rect.x + rect.width) / 2;
		LnY_min = rect.y;
		LnY_max = rect.y + rect.height;
	}

	int COUNT_LEFT=0,COUNT_RIGHT=0,Difference_LR;
	float WD2HG,area_ratio;
	int rect_max_x,rect_max_y,rect_area;
	
	if(cnt1!=0 && cnt2==0)//if only have line,reset to 0
	{	
		countL=0;
		countR=0;
		countS=0;
	}
	
	if(cnt2 != 0 && cnt1 !=0)
	{
		CvRect rect = ((CvContour*)contours2[0])->rect;
		
		WD2HG = rect.width / rect.height;
		rect_max_x = rect.x + rect.width;
		rect_max_y = rect.y + rect.height;
		rect_area = rect.width*rect.height;
		area_ratio = rect_area / maxArea2[0];
		
		if(TuneMode && WD2HG>0.7 && WD2HG<1.3 && rect.x>ARROW_MIN_X && rect.y>ARROW_MIN_Y && rect_max_x<ARROW_MAX_X && rect_max_y<ARROW_MAX_Y && area_ratio<1.4 && area_ratio>0.8)
		{
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);	
			cvLine(frame,cvPoint(rect.x+rect.width*ARROW_LEFT_MIN_RATIO, rect.y+rect.height*ARROW_UP_RATIO),cvPoint(rect.x+rect.width*ARROW_LEFT_MIN_RATIO, rect.y+rect.height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(frame,cvPoint(rect.x+rect.width*ARROW_LEFT_MAX_RATIO, rect.y+rect.height*ARROW_UP_RATIO),cvPoint(rect.x+rect.width*ARROW_LEFT_MAX_RATIO, rect.y+rect.height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(frame,cvPoint(rect.x+rect.width*ARROW_RIGHT_MIN_RATIO, rect.y+rect.height*ARROW_UP_RATIO),cvPoint(rect.x+rect.width*ARROW_RIGHT_MIN_RATIO, rect.y+rect.height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
			cvLine(frame,cvPoint(rect.x+rect.width*ARROW_RIGHT_MAX_RATIO, rect.y+rect.height*ARROW_UP_RATIO),cvPoint(rect.x+rect.width*ARROW_RIGHT_MAX_RATIO, rect.y+rect.height*ARROW_DOWN_RATIO),CV_RGB(0,255,0),2,8,0);
		}
		
		if(WD2HG>0.7 && WD2HG<1.3 && rect.x>ARROW_MIN_X && rect.y>ARROW_MIN_Y && rect_max_x<ARROW_MAX_X && rect_max_y<ARROW_MAX_Y && area_ratio<1.4 && area_ratio>0.8)
		{
			for(a=rect.y+rect.height*ARROW_UP_RATIO;a<rect.y+rect.height*ARROW_DOWN_RATIO;a++)
			for(b=rect.x+rect.width*ARROW_LEFT_MIN_RATIO;b<rect.x+rect.width*ARROW_LEFT_MAX_RATIO;b++)
			{
				H = data_hsv[a*step_hsv+b*chanels_hsv+0];
				S = data_hsv[a*step_hsv+b*chanels_hsv+1];
				V = data_hsv[a*step_hsv+b*chanels_hsv+2];

				if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
				{
					COUNT_LEFT = COUNT_LEFT + 1;
				}
			}
			
			for(a=rect.y+rect.height*ARROW_UP_RATIO;a<rect.y+rect.height*ARROW_DOWN_RATIO;a++)
			for(b=rect.x+rect.width*ARROW_RIGHT_MIN_RATIO;b<rect.x+rect.width*ARROW_RIGHT_MAX_RATIO;b++)
			{
				H = data_hsv[a*step_hsv+b*chanels_hsv+0];
				S = data_hsv[a*step_hsv+b*chanels_hsv+1];
				V = data_hsv[a*step_hsv+b*chanels_hsv+2];

				if( (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]))
				{
					COUNT_RIGHT = COUNT_RIGHT + 1;
				}
			}
			
			Difference_LR= COUNT_LEFT - COUNT_RIGHT;
			if(Difference_LR < 0)
				Difference_LR = (-1)*Difference_LR;
				
			if(COUNT_LEFT > COUNT_RIGHT && Difference_LR > DIFFERENCE_LR_MAX)
			{
//				printf("Turn Left\n");
				countL=countL+1;
			}
			else
			if(COUNT_RIGHT > COUNT_LEFT && Difference_LR > DIFFERENCE_LR_MAX)
			{
//				printf("Turn Right\n");
				countR=countR+1;
			}
			else
			if(Difference_LR < DIFFERENCE_LR_MIN)
			{
//				printf("Go Straight\n");
				countS=countS+1;
			}
		}
		
		if(countL>countR && countL>countS)
		{
			printf("Turn Left\n");
			cvCircle(frame,cvPoint(80,120),7,CV_RGB(0,255,10),2,8,0);
			flag = 1;
			flag1 = 1;
			ArX_min = 40;
			ArX_max = 40;
			ArY_min = 120;
			ArY_max = 120;
		}
		
		if(countR>countL && countR>countS)
		{
			printf("Turn Right\n");
			cvCircle(frame,cvPoint(240,120),7,CV_RGB(0,255,20),2,8,0);
			flag = 1;
			flag1 = 1;
			ArX_min = 120;
			ArX_max = 120;
			ArY_min = 120;
			ArY_max = 120;		
		}
		
		if(countS>countL && countS>countR)
		{
			printf("Go Straight\n");
			cvCircle(frame,cvPoint(160,60),7,CV_RGB(0,255,30),2,8,0);
			flag = 1;
			flag1 = 1;
			ArX_min = 80;
			ArX_max = 80;
			ArY_min = 120;
			ArY_max = 120;		
		}
	}	

	if(cnt1 != 0)//have Line
	{
		if(flag==0)// no arrow
		{
			blob->Xmin = LnX_min;
			blob->Xmax = LnX_max;
			blob->Ymin = LnY_min;
			blob->Ymax = LnY_max;
			ArX_min=255;
			ArX_max=255;
			ArY_min=255;
			ArY_max=255;	
		}
		else  //have arrow
		{
			if(LnY_min < LINE_MINY_SEND) //upper point smaller than 200, send Line
			{
				blob->Xmin = LnX_min;
				blob->Xmax = LnX_max;
				blob->Ymin = LnY_min;
				blob->Ymax = LnY_max;	
			}
			else// upper point greater than 200, send Arrow
			{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
			}
		}
	}
	else  //no line
	{
		if(flag == 0)//No arrow
		{

				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
				
		}
		else  //have arrow
		{
			blob->Xmin = ArX_min;
			blob->Xmax = ArX_max;
			blob->Ymin = ArY_min;
			blob->Ymax = ArY_max;	
		}		
	}
	
	printf("Left:%d, Right:%d,Straight:%d\n",countL,countR,countS);
	flag = 0;
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);

	return(1);
}
*/
//Original method,find poly and scan all center
/*
int VISION_Game_ArrowDetect(VisionRange range, VisionRange range1, VisionRange range2, BlobCoord *blob,char *gameplay, Color color)
{
	LoadValueColor(range,range1,range2);
//	printf("Game Arrow\n");
	IplImage *imgHSV=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
	cvCvtColor(frame, imgHSV, CV_BGR2HSV_FULL);
	IplImage *imgThreshed=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	IplImage *imgThreshed1=cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	
	CvSeq *tmp_cont, *contour;
	CvSeq *contours1[NUMBER_OF_BLOBS]={0},*contours2[NUMBER_OF_BLOBS]={0},*contours3[NUMBER_OF_BLOBS]={0},*contours4[NUMBER_OF_BLOBS]={0};
	CvPoint *pt;
	CvScalar data;
	
	unsigned char H,S,V;
	int a=0,b=0,i,m,l=0; 
	int total=0, avg,total_S=0, avg_S=0,total_V=0, avg_V=0;
	int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
	double maxArea1[NUMBER_OF_BLOBS]={0},maxArea2[NUMBER_OF_BLOBS]={0},maxArea3[NUMBER_OF_BLOBS]={0},maxArea4[NUMBER_OF_BLOBS]={0};
	double area1=0,tmp_area1=0;
	int corner_X[4],corner_Y[4],cntcorner1=0,cntcorner2=0,cntcorner3=0,cntcorner4=0,xxxx=0;
	
	int LnX_min=0,LnX_max=0,LnY_min=0,LnY_max=0;
	int flag=0,flag1;
	
	unsigned char *data_ts = (unsigned char *)imgThreshed->imageData, *data_hsv = (unsigned char *)imgHSV->imageData;
	int step_ts = imgThreshed->widthStep/sizeof(unsigned char), step_hsv = imgHSV->widthStep/sizeof(unsigned char);
	int chanels_hsv = imgHSV->nChannels;
	
	cvZero(imgThreshed);
	cvZero(imgThreshed1);
	
	for(a=UP_ARROW_MARA;a<DOWN_ARROW_MARA;a++)
	for(b=LEFT_ARROW_MARA;b<RIGHT_ARROW_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if((H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
		|| (H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2]) )
		{
			data_ts[a*step_ts+b]=255;
		}
		else
		{
			data_ts[a*step_ts+b]=0;
		}
	}
//	cvErode(imgThreshed,imgThreshed,NULL,1);	
	cvDilate(imgThreshed,imgThreshed,NULL,1);
	cvErode(imgThreshed,imgThreshed,NULL,1); 
	
	cvAdd(imgThreshed1,imgThreshed,imgThreshed1,0);
	
	if(TuneMode)
		cvShowImage("Arrow",imgThreshed1);
//	cvMoveWindow("Threshold",500,350);
	
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    
	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_ARROW_SIZE_MARA || area1>MAX_ARROW_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
	
			cnt2++;
			
			for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
			{
				if(area1 > maxArea2[i])
				{
					maxArea2[i] = area1;
					contours2[i] = contour;
					
					for(m = (i-1); m >= 0; --m)
					{
						if(maxArea2[m] < maxArea2[m+1])
						{
							tmp_area1 = maxArea2[m+1];
							tmp_cont = contours2[m+1];
							maxArea2[m+1] = maxArea2[m];
							contours2[m+1] = contours2[m];
							maxArea2[m] = tmp_area1;
							contours2[m] = tmp_cont;
						}
					}
					break;
				}
			}
	}
	
	cvZero(imgThreshed);
	
	for(a=UP_LINE_MARA;a<DOWN_LINE_MARA;a++)
	for(b=LEFT_LINE_MARA;b<RIGHT_LINE_MARA;b++)
	{
		H = data_hsv[a*step_hsv+b*chanels_hsv+0];
		S = data_hsv[a*step_hsv+b*chanels_hsv+1];
		V = data_hsv[a*step_hsv+b*chanels_hsv+2];

		if(Invert[0]==0)
		{
			if( (H >= min[0] && H <= max[0] && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]))
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
		else
		{
			if( ((H <= min[0] || H >= max[0]) && S >= min[1] && S<=max[1] && V>=min[2] && V<=max[2]) )
			{
				data_ts[a*step_ts+b]=255;
			}
			else
			{
				data_ts[a*step_ts+b]=0;
			}
		}
	}
	
	if(TuneMode)
		cvShowImage("Line",imgThreshed);
		
	cvFindContours(imgThreshed, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	for(;contour;contour = contour->h_next)
	{
		area1=fabs(cvContourArea(contour,CV_WHOLE_SEQ,1 ));
		
		if(area1<MIN_LINE_SIZE_MARA || area1>MAX_LINE_SIZE_MARA )
		{
			cvSeqRemove(contour,0);
			continue;
		}
		
		cnt1++;
		
		for(i = NUMBER_OF_BLOBS-1; i >= 0; --i)
		{
			if(area1 > maxArea1[i])
			{
				maxArea1[i] = area1;
				contours1[i] = contour;
				
				for(m = (i-1); m >= 0; --m)
				{
					if(maxArea1[m] < maxArea1[m+1])
					{
						tmp_area1 = maxArea1[m+1];
						tmp_cont = contours1[m+1];
						maxArea1[m+1] = maxArea1[m];
						contours1[m+1] = contours1[m];
						maxArea1[m] = tmp_area1;
						contours1[m] = tmp_cont;
					}
				}
				break;
			}
		}	
	}
	
	if(cnt1 != 0)
	{
		CvRect rect = ((CvContour*)contours1[0])->rect;
		if(TuneMode)
			cvRectangle(frame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(0, 0, 255), 2, 8, 0);

		LnX_min = rect.x / 2;
		LnX_max = (rect.x + rect.width) / 2;
		LnY_min = rect.y;
		LnY_max = rect.y + rect.height;
	}

	int COUNT_LEFT=0,COUNT_RIGHT=0,Difference_LR;
	CvSeq* result;
	int f;
	
	if(cnt1!=0 && cnt2==0)//if only have line,reset to 0
	{	
		countL=0;
		countR=0;
		countS=0;
	}
		
	if(cnt2 != 0 && cnt1 !=0)
	{
		result = cvApproxPoly(contours2[0],sizeof(CvContour),storage,CV_POLY_APPROX_DP,cvContourPerimeter(contours2[0])*0.085,0);
//		printf("%d\n",result->total);//cvContourPerimeter(contours2[0])*0.02
		if(result->total == 4)
		{
			CvPoint *pt1[4],pt2[4];
			for(f=0;f<4;f++)
			{
				pt1[f]=(CvPoint*)cvGetSeqElem(result,f);
			}
			
			cvLine(frame,*pt1[0],*pt1[1],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[1],*pt1[2],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[2],*pt1[3],CV_RGB(0,255,0),2,8,0);
			cvLine(frame,*pt1[3],*pt1[0],CV_RGB(0,255,0),2,8,0);
			
			int midX[4]={0},midY[4]={0},cent=0;
			
			pt2[0].x = midX[0]=(pt1[0]->x + pt1[1]->x)/2;
			pt2[0].y = midY[0]=(pt1[0]->y + pt1[1]->y)/2;
				
			pt2[1].x = midX[1]=(pt1[1]->x + pt1[2]->x)/2;
			pt2[1].y = midY[1]=(pt1[1]->y + pt1[2]->y)/2;
			
			pt2[2].x = midX[2]=(pt1[2]->x + pt1[3]->x)/2;
			pt2[2].y = midY[2]=(pt1[2]->y + pt1[3]->y)/2;
			
			pt2[3].x = midX[3]=(pt1[3]->x + pt1[0]->x)/2;
			pt2[3].y = midY[3]=(pt1[3]->y + pt1[0]->y)/2;
			
			cent = 	(pt1[0]->x + pt1[2]->x)/2;

			int a,b,cnt[4]={0},c=0,tmp1_c=0,tmp2_c=0,cnt_tmp=0,cntwhite=0,tmp3_c,counter[4]={0},d=0,count=0;  
			unsigned char H,S,V;
			unsigned char temp[6];
			
			for(d=0;d<4;d++)
			{
				if(pt1[d]->y +6 <241){
				for(a=(pt1[d]->y)-5;a<(pt1[d]->y)+6;a++)
				for(b=(pt1[d]->x)-5;b<(pt1[d]->x)+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					
					if(H >= min1[0] && H <= max1[0] && S >= min1[1] && S<=max1[1] && V>=min1[2] && V<=max1[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3]==20)
							{
//								printf("Black BG %d\n",counter[3]);
								temp[0] = min2[0];
								temp[1] = max2[0];
								temp[2] = min2[1];
								temp[3] = max2[1];
								temp[4] = min2[2];
								temp[5] = max2[2];								
								
							}							
					}
					else
					if(H >= min2[0] && H <= max2[0] && S >= min2[1] && S<=max2[1] && V>=min2[2] && V<=max2[2])
					{
							counter[d]++;
//							printf("%d counter = %d \n",d,counter[d]);
							if(counter[d] == 20)
							{
								count++;
							}
//							printf("count = %d\n",count);
							if(d==3 && count==4 && counter[3] == 20)
							{
//								printf("White BG %d\n",counter[3]);
								temp[0] = min1[0];
								temp[1] = max1[0];
								temp[2] = min1[1];
								temp[3] = max1[1];
								temp[4] = min1[2];
								temp[5] = max1[2];								
							}							
					}			
				}}
			}
			
			for(c=0;c<4;c++)
			{
				if(midY[c]+6 < 241){
				for(a=midY[c]-5;a<midY[c]+6;a++)
				for(b=midX[c]-5;b<midX[c]+6;b++)
				{
					H = data_hsv[a*step_hsv+b*chanels_hsv+0];
					S = data_hsv[a*step_hsv+b*chanels_hsv+1];
					V = data_hsv[a*step_hsv+b*chanels_hsv+2];
					
					if(H >= temp[0] && H <= temp[1] && S >= temp[2] && S<= temp[3] && V>=temp[4] && V<=temp[5])
					{
						cnt[c]++;
//						printf("cnt=%d\n",cnt[c]);								
					}			
				}
				}   
				if(cnt[c]>5)
				{	
					cntwhite++;						
//					printf("cntwhite = %d\n",cntwhite);
					if(cntwhite == 1) tmp1_c = c;
					if(cntwhite == 2) tmp2_c = c;
					
//					printf("tmp1_c = %d \n",tmp1_c);
//					printf("tmp2_c = %d \n",tmp2_c);
//					printf("c = %d \n",c);	
					if(cntwhite ==2)
					{
//						printf("pt2[tmp1_c].y = %d, pt2[tmp2_c].y = %d \n",pt2[tmp1_c].y,pt2[tmp2_c].y);
						tmp3_c=tmp1_c -tmp2_c;
						
						//if(tmp3_c==4) tmp3_c=0;
						//if(tmp3_c==5) tmp3_c=1;
						//printf("tmp3_c=%d\n",tmp3_c);
						
						if(pt2[tmp1_c].y > pt2[tmp2_c].y)
						{
							cvCircle(frame,pt2[tmp2_c],7,CV_RGB(0,100,60*c),2,8,0);
							//printf("pt2[tmp2_c].x = %d ,pt2[tmp3_c].x= %d\n", pt2[tmp2_c].x,pt2[tmp3_c].x);
							//if(pt2[tmp2_c].x<pt2[tmp3_c].x)							
							if(tmp3_c == 2 || tmp3_c == -2)
							{
//							printf("Go straight\n");
							countS++;
							}
							else if((tmp3_c == 1 || tmp3_c == 3 || tmp3_c == -1 || tmp3_c == -3)&& pt2[tmp2_c].x > cent)
							{
//							printf("turn right\n");
							countR++;
							}
							
						}
						if(pt2[tmp2_c].y > pt2[tmp1_c].y)
						{
							cvCircle(frame,pt2[tmp1_c],7,CV_RGB(0,100,60*c),2,8,0); 
							if(tmp3_c == 2 || tmp3_c == -2)
							{
//							printf("Go straight\n");
							countS++;
							}
							else if(pt2[tmp1_c].x < cent)
							{
//							printf("turn left\n");
							countL++;
							}
						}
						if(pt2[tmp2_c].y == pt2[tmp1_c].y) 
						printf("Same position\n");
						cntwhite=0;
					}   
				}
			}	
			
			if(countL>countR && countL>countS)
			{
				printf("Turn Left\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 40;
				ArX_max = 40;
				ArY_min = 110;
				ArY_max = 130;
			}
			
			if(countR>countL && countR>countS)
			{
				printf("Turn Right\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 120;
				ArX_max = 120;
				ArY_min = 110;
				ArY_max = 130;		
			}
			
			if(countS>countL && countS>countR)
			{
				printf("Go Straight\n");
				flag = 1;
				flag1 = 1;
				ArX_min = 80;
				ArX_max = 80;
				ArY_min = 110;
				ArY_max = 130;		
			}
		}	
	}	
	
	if(cnt1 != 0)//have Line
	{
		if(flag==0)// no arrow
		{
			blob->Xmin = LnX_min;
			blob->Xmax = LnX_max;
			blob->Ymin = LnY_min;
			blob->Ymax = LnY_max;
			ArX_min=255;
			ArX_max=255;
			ArY_min=255;
			ArY_max=255;	
		}
		else  //have arrow
		{
			if(LnY_min < 180) //upper point smaller than 200, send Line
			{
				blob->Xmin = LnX_min;
				blob->Xmax = LnX_max;
				blob->Ymin = LnY_min;
				blob->Ymax = LnY_max;	
			}
			else// upper point greater than 200, send Arrow
			{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;	
			}
		}
	}
	else  //no line
	{
		if(flag == 0)//No arrow
		{
				blob->Xmin = ArX_min;
				blob->Xmax = ArX_max;
				blob->Ymin = ArY_min;
				blob->Ymax = ArY_max;				
		}
		else  //have arrow
		{
			blob->Xmin = ArX_min;
			blob->Xmax = ArX_max;
			blob->Ymin = ArY_min;
			blob->Ymax = ArY_max;	
		}		
	}
	
	printf("Left:%d, Right:%d,Straight:%d\n",countL,countR,countS);
	flag = 0;
	
	if(TuneMode)
		VISION_ShowOriginalFrame();
	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgThreshed1);

	return(1);
}
*/
