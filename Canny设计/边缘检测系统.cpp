#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <graphics.h>
#include <mmsystem.h>   //��ý��
#include <Windows.h>
#pragma  comment(lib,"winmm.lib")

using namespace std;
using namespace cv;

#define MAX_fault  500
#define SETNUMBER  120     //ͼ��ԱȲ���

HWND  hwnd;   //����һ�����ھ��

//ͼ��ֱ�Ӿ��ʵ��Sobel
void sobelEdge(const Mat image, Mat &result, uchar threshold)
{
	CV_Assert(image.channels() == 1);
	//��ʼ��ˮƽ������
	Mat sobelx = (Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	//��ʼ����ֱ������
	Mat sobely = (Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	//��ʼ��45�Ⱥ�����
	Mat sobel45 = (Mat_<float>(3, 3) << 2, 1, 0, 1, 0, -1, 0, -1, -2);
	//��ʼ��135�Ⱥ�����
	Mat sobel135 = (Mat_<float>(3, 3) << 0, -1, -2, 1, 0, -1, 2, 1, 0);
	result = Mat::zeros(image.rows - 2, image.cols - 2, image.type());
	double graMag = 0;
	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{//�������е�����
			float edgex = 0, edgey = 0, edge45 = 0, edge135 = 0;
			//��������ˮƽ�봹ֱ�ݶ�
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
			////�����ݶ�ģ��
			graMag = sqrt(pow(edgex, 2) + pow(edgey, 2) + pow(edge45, 2) + pow(edge135, 2));

			//�Ż�����ȡ�ݶ�ͼ��
			/*
			   ʵ�ֲ��裺
			   1�����жϸõ����Ǹ��������ݶ�����
			   2�����жϸõ��ڸ÷����Ƿ��Ǽ�Сֵ
			*/

			//��x���������

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

			//��y���������
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

			//��45���������
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

			//��135���������
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
			//��ֵ��
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
			
				mciSendString(L"open ./ͼƬ/������.mp3 alias music1", 0, 0, 0);
				mciSendString(L"play music1", 0, 0, 0);//����

				MessageBox(hwnd, L"����", L"�����", MB_OK);
				

				return;
			}
		}
	}
	MessageBox(hwnd, L"��ȷ", L"�����", MB_OK);
	return;
}

void Initmap()
{
	//��������
	initgraph(800, 600);
	setbkcolor(RGB(75, 180, 128));  //���� black ��ɫ
	cleardevice();          //ɾ������ɫ

	IMAGE img[3];
	loadimage(&img[0],L"ͼƬ\\005.jpg",100,100);
	putimage(550,50,&img[0]);
	loadimage(&img[1], L"ͼƬ\\006.jpg", 100, 100);
	putimage(550, 250, &img[1]);
	loadimage(&img[2], L"ͼƬ\\007.jpg", 100, 100);
	putimage(550, 450, &img[2]);

	settextcolor(RGB(47, 208, 159));
	settextstyle(50, 0, L"����");
	outtextxy(650, 65, L"��ʼ");
	outtextxy(650, 265, L"��ͣ");
	outtextxy(650, 465, L"����");

	IMAGE targetimg;
		
	loadimage(&targetimg,L"ͼƬ\\test.jpg",500,300);
	putimage(0, 0, &targetimg);
	settextcolor(RED);
	settextstyle(20, 0, L"����");
	outtextxy(0, 0, L"Ŀ��ͼ��");
	
	//�м�������
	setlinecolor(BLACK);    //����  ���� ��ɫ
	setfillcolor(YELLOW);  //����  ��� ��ɫ
	fillrectangle(0, 295, 500, 300);
}

bool key_down()
{
	//ͨ��WIN32API
	if (GetAsyncKeyState(VK_UP))
	{
	//����ͼ��
	IMAGE test;
	loadimage(&test, L"ͼƬ\\test1.jpg", 500, 300);
	putimage(0, 300, &test);
	outtextxy(0, 310, L"����ͼ��");
	return 1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		MessageBox(hwnd, L"��ͣ���", L"�����", MB_OK);
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
	//Ŀ��ͼ��Ľ���
	image = imread("ͼƬ\\001.jpg");


	cvtColor(image, temp1 , COLOR_BGR2GRAY);             //��ֵ������
	////��ֵ�˲�--�ˣ�5*5
	medianBlur(temp1 ,temp2 , 5);
	//
	sobelEdge(temp2, result, threshold);
	
 //   //Canny����
	Canny(result, result11, 20, 40, 3);
	imwrite("ͼƬ\\test.jpg",result11);    //��ͼƬ�����ڸ��ļ����У������������
	//imshow("��",result11);


	while (1)
	{	
		if (key_down()) {
			int flag = 0;
			//�����ͼ��
			Mat img, t1, t2, t3, ans;
			//����ͼ��Ľ���
			img = imread("ͼƬ\\001.jpg");
			cvtColor(img, t1, COLOR_BGR2GRAY);             //��ֵ������
			//��ֵ�˲�--�ˣ�5*5
			medianBlur(t1, t2, 5);
			//Sobel����
			sobelEdge(t2, t3, threshold);
			//Canny����
			Canny(t3, ans, 20, 40, 3);
			imwrite("ͼƬ\\test1.jpg", ans);    //��ͼƬ�����ڸ��ļ����У������������

			test(result11, ans);
			if (MessageBox(hwnd, L"�Ƿ����¼��", L"������", MB_YESNO) == IDNO)
			{
				break;
			}
		}
		
		//closegraph();
		
	}

//	waitKey(0);
	return 0;
}


