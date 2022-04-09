#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <graphics.h>
#include <mmsystem.h>   //多媒体
#include <Windows.h>
#pragma  comment(lib,"winmm.lib")

using namespace std;
using namespace cv;

#define MAX_fault  500
#define SETNUMBER  120     //图像对比参数

HWND  hwnd;   //定义一个窗口句柄

//图像直接卷积实现Sobel
void sobelEdge(const Mat image, Mat &result, uchar threshold)
{
	CV_Assert(image.channels() == 1);
	//初始化水平核因子
	Mat sobelx = (Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	//初始化垂直核因子
	Mat sobely = (Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	//初始化45度核因子
	Mat sobel45 = (Mat_<float>(3, 3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
	//初始化135度核因子
	Mat sobel135 = (Mat_<float>(3, 3) << 0, -1, -2, 1, 0, -1, 2, 1, 0);
	result = Mat::zeros(image.rows - 2, image.cols - 2, image.type());
	double graMag = 0;
	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{//遍历所有的像素
			float edgex = 0, edgey = 0, edge45 = 0, edge135 = 0;
			//遍历计算水平与垂直梯度
			for (int k = -1; k < 2; k++)
			{
				for (int p = -1; p < 2; p++)
				{

					edgex += (float)image.at<uchar>(k + i, p + j) * sobelx.at<float>(1 + k, 1 + p);
					edgey += (float)image.at<uchar>(k + i, p + j) * sobely.at<float>(1 + k, 1 + p);
					edge45 += (float)image.at<uchar>(k + i, p + j) * sobel45.at<float>(1 + k, 1 + p);
					edge135 += (float)image.at<uchar>(k + i, p + j) * sobel135.at<float>(1 + k, 1 + p);

				}
			}
			////计算梯度模长
			graMag = sqrt(pow(edgex, 2) + pow(edgey, 2) + pow(edge45, 2) + pow(edge135, 2));

			//优化：获取梯度图像
			/*
			   实现步骤：
			   1、先判断该点在那个方向是梯度最大的
			   2、再判断该点在该方向是否是极小值
			*/

			//在x方向上最大

			if (edgex > edgey&&edgex > edge45&&edgex > edge135)
			{
				float tmep0 = (float)image.at<uchar>(i, j);
				float tmep_1 = (float)image.at<uchar>(i, j - 1);
				float temp1 = (float)image.at<uchar>(i, j + 1);

				if (tmep0 < tmep_1 && tmep0 < temp1)
				{
					result.at<uchar>(i - 1, j - 1) = 0;
				}

			}

			//在y方向上最大
			if (edgey > edgex&&edgey > edge45 && edgey > edge135)
			{
				float tmep0 = (float)image.at<uchar>(i, j);
				float tmep_1 = (float)image.at<uchar>(i - 1, j);
				float temp1 = (float)image.at<uchar>(i + 1, j);
				if (tmep0 < tmep_1 && tmep0 < temp1)
				{
					result.at<uchar>(i - 1, j - 1) = 0;
				}

			}

			//在45方向上最大
			if (edge45 > edgex&&edge45 > edgey&&edge45 > edge135)
			{
				float tmep0 = (float)image.at<uchar>(i, j);
				float tmep_1 = (float)image.at<uchar>(i - 1, j + 1);
				float temp1 = (float)image.at<uchar>(i + 1, j - 1);
				if (tmep0 < tmep_1 && tmep0 < temp1)
				{
					result.at<uchar>(i - 1, j - 1) = 0;
				}

			}

			//在135方向上最大
			if (edge135 > edgex&&edge135 > edge45&&edge135 > edgey)
			{
				float tmep0 = (float)image.at<uchar>(i, j);
				float tmep_1 = (float)image.at<uchar>(i + 1, j + 1);
				float temp1 = (float)image.at<uchar>(i - 1, j - 1);
				if (tmep0 < tmep_1 && tmep0 < temp1)
				{
					result.at<uchar>(i - 1, j - 1) = 0;
				}

			}
			//二值化
			result.at<uchar>(i - 1, j - 1) = ((graMag > threshold) ? 255 : 0);
		}
	}
	return;
}

void test(const Mat result,const Mat ans)  /**************************************/
{
	for (int  i = 0; i <= result.rows -1; i++)
	{
		for (int j=0; j <= result.cols-1; j++)
		{
			if (abs(result.at<uchar>(i,j)-ans.at<uchar>(i,j)) < SETNUMBER)  /**********************************/
			{
			
				mciSendString(L"open ./图片/报警声.mp3 alias music1", 0, 0, 0);
				mciSendString(L"play music1", 0, 0, 0);//播放

				MessageBox(hwnd, L"错误", L"检测结果", MB_OK);
				

				return;
			}
		}
	}
	MessageBox(hwnd, L"正确", L"检测结果", MB_OK);
	return;
}

void Initmap()
{
	//界面设置
	initgraph(800, 600);
	setbkcolor(RGB(75, 180, 128));  //设置 black 颜色
	cleardevice();          //删除背景色

	IMAGE img[3];
	loadimage(&img[0],L"图片\\005.jpg",100,100);
	putimage(550,50,&img[0]);
	loadimage(&img[1], L"图片\\006.jpg", 100, 100);
	putimage(550, 250, &img[1]);
	loadimage(&img[2], L"图片\\007.jpg", 100, 100);
	putimage(550, 450, &img[2]);

	settextcolor(RGB(47, 208, 159));
	settextstyle(50, 0, L"楷书");
	outtextxy(650, 65, L"开始");
	outtextxy(650, 265, L"暂停");
	outtextxy(650, 465, L"重置");

	IMAGE targetimg;
		
	loadimage(&targetimg,L"图片\\test.jpg",500,300);
	putimage(0, 0, &targetimg);
	settextcolor(RED);
	settextstyle(20, 0, L"楷书");
	outtextxy(0, 0, L"目标图像");
	
	//中间版面设计
	setlinecolor(BLACK);    //设置  线条 颜色
	setfillcolor(YELLOW);  //设置  填充 颜色
	fillrectangle(0, 295, 500, 300);
}

bool key_down()
{
	//通过WIN32API
	if (GetAsyncKeyState(VK_UP))
	{
	//待测图像
	IMAGE test;
	loadimage(&test, L"图片\\test1.jpg", 500, 300);
	putimage(0, 300, &test);
	outtextxy(0, 310, L"待测图像");
	return 1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		MessageBox(hwnd, L"暂停检测", L"检测结果", MB_OK);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		
	}
	return 0;
}

int  main()
{
	
	Mat image, temp1,temp2, result,result11;
	Mat temp;
	
	medianBlur(temp, temp1, 5);
	
	uchar threshold=100;
	Initmap();
	//目标图像的建立
	image = imread("图片\\001.jpg");


	cvtColor(image, temp1 , COLOR_BGR2GRAY);             //二值化处理
	////中值滤波--核：5*5
	medianBlur(temp1 ,temp2 , 5);
	//
	sobelEdge(temp2, result, threshold);
	
 //   //Canny处理
	Canny(result, result11, 20, 40, 3);
	imwrite("图片\\test.jpg",result11);    //将图片保持在该文件夹中，方便后续调用
	//imshow("后",result11);


	while (1)
	{	
		if (key_down()) {
			int flag = 0;
			//待检测图像
			Mat img, t1, t2, t3, ans;
			//待测图像的建立
			img = imread("图片\\001.jpg");
			cvtColor(img, t1, COLOR_BGR2GRAY);             //二值化处理
			//中值滤波--核：5*5
			medianBlur(t1, t2, 5);
			//Sobel处理
			sobelEdge(t2, t3, threshold);
			//Canny处理
			Canny(t3, ans, 20, 40, 3);
			imwrite("图片\\test1.jpg", ans);    //将图片保持在该文件夹中，方便后续调用

			test(result11, ans);
			if (MessageBox(hwnd, L"是否重新检测", L"检测结束", MB_YESNO) == IDNO)
			{
				break;
			}
		}
		
		//closegraph();
		
	}

//	waitKey(0);
	return 0;
}


