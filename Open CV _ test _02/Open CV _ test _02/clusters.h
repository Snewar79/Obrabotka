#pragma once
#include <vector>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>


using namespace std;
using namespace cv;

template <class T>
T pow_m(T ch, int dig);


class Cluster {
public:
	vector<POINT> scores;


	bool active_flag;

	Cluster()
	{
		active_flag = true;
		
	}


	int mid_r;
	int mid_g;
	int mid_b;



	int curX, curY;//���������� �������� ���������
	int lastX, lastY;//���������� ����������� ��������
	size_t Size() { return scores.size(); }//�������� ������ �������
	inline void Add(POINT pt) { scores.push_back(pt); }//��������� ������� � ��������
	void SetCenter();
	void Clear();//������ ������
	static Cluster* Bind(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src, int c_dig, int d_dig, int c_coef, int d_coef);
	static void InitialCenter(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src);
	Mat Start(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src, int c_dig, int d_dig, int c_coef, int d_coef);
	inline POINT& at(unsigned i) { return scores.at(i); }//������  � ��������� �������



	void resetMidColor(Mat &src)
	{
		if (scores.size() == 0) return;

		int sum_r = 0;
		int sum_g = 0;
		int sum_b = 0;
		for (int i = 0; i < scores.size(); i++)
		{
			sum_r += src.at<Vec3b>(scores[i].x, scores[i].y).val[2];
			sum_g += src.at<Vec3b>(scores[i].x, scores[i].y).val[1];
			sum_b += src.at<Vec3b>(scores[i].x, scores[i].y).val[0];
		}

		mid_r = sum_r / scores.size();
		mid_g = sum_g / scores.size();
		mid_b = sum_b / scores.size();

	}





};



void Cluster::InitialCenter(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src) {

	int size = vpt.size();
	int step = size / k;
	int steper = 0;

	for (int i = 0; i < k; i++, steper += step) {
		clusarr[i].curX = vpt[steper].x;
		clusarr[i].curY = vpt[steper].y;

		//Vec3b color = src.at<Vec3b>(m, n);
	//	uchar blue = color.val[0];
	//	uchar green = color.val[1];
	//	uchar red = color.val[2];

		clusarr[i].mid_r = src.at<Vec3b>(clusarr[i].curX, clusarr[i].curY).val[2];
		clusarr[i].mid_g = src.at<Vec3b>(clusarr[i].curX, clusarr[i].curY).val[1];
		clusarr[i].mid_b = src.at<Vec3b>(clusarr[i].curX, clusarr[i].curY).val[0];
	}
}




void Cluster::SetCenter() {
	int sumX = 0, sumY = 0;
	int i = 0;
	int size = Size();
	
	if (size == 0)
	{
	//	active_flag = false;
	//	return;
	}


	for (; i<size; sumX += scores[i].x, i++);//the centers of mass by x
	i = 0;
	for (; i<size; sumY += scores[i].y, i++);//the centers of mass by y
	lastX = curX;
	lastY = curY;
	curX = sumX / size;
	curY = sumY / size;
}

void Cluster::Clear() {
	scores.clear();
}







Cluster * Cluster::Bind(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src, int c_dig, int d_dig, int c_coef, int d_coef) {
	for (int j = 0; j < k; j++)
		clusarr[j].Clear();// ������ ������� ����� ��������������
	int size = vpt.size();
	for (int i = 0; i < size; i++) {// ��������� ���� �� ���� �������� ���������
	
		int cur_r = src.at<Vec3b>(vpt[i].x, vpt[i].y).val[2];
		int cur_g = src.at<Vec3b>(vpt[i].x, vpt[i].y).val[1];
		int cur_b = src.at<Vec3b>(vpt[i].x, vpt[i].y).val[0];
		
		//int intensCurrPix = (src.at<Vec3b>(vpt[i].x, vpt[i].y).val[0] + src.at<Vec3b>(vpt[i].x, vpt[i].y).val[1] + src.at<Vec3b>(vpt[i].x, vpt[i].y).val[2]) / 3;

		int col_diff_r = cur_r - clusarr[0].mid_r;
		int col_diff_g = cur_g - clusarr[0].mid_g;
		int col_diff_b = cur_b - clusarr[0].mid_b;

		//int min_col_diff = std::abs(col_diff_r) + std::abs(col_diff_g) + std::abs(col_diff_b);
		// �������� ����������
		int min_col_diff = sqrt(
			pow(cur_r - clusarr[0].mid_r, 2) + pow(cur_g - clusarr[0].mid_g, 2) + pow(cur_r - clusarr[0].mid_b, 2));

		min_col_diff = std::abs(col_diff_b) + std::abs(col_diff_g) + std::abs(col_diff_b);


		int min = sqrt(
			pow((float)clusarr[0].curX - vpt[i].x, 2) + pow((float)clusarr[0].curY - vpt[i].y, 2)
		); // �������������� ����������
		



		min = pow_m(min, d_dig);
		min_col_diff = pow_m(min_col_diff, c_dig);

		double itog = min * d_coef  + min_col_diff * c_coef;





		//int intensZeroCentr = (src.at<Vec3b>(clusarr[0].curX, clusarr[0].curY).val[0] + src.at<Vec3b>(clusarr[0].curX, clusarr[0].curY).val[1] +
		//	src.at<Vec3b>(clusarr[0].curX, clusarr[0].curY).val[2]) / 3;





		Cluster * cl = &clusarr[0];
		for (int j = 1; j < k; j++) 
		{
		
			
			
			
			
			int tmp = sqrt(
				pow((float)clusarr[j].curX - vpt[i].x, 2) + pow((float)clusarr[j].curY - vpt[i].y, 2)
			); // �������������� ����������
			
			


			int col_diff_r_t = cur_r - clusarr[j].mid_r;
			int col_diff_g_t = cur_g - clusarr[j].mid_g;
			int col_diff_b_t = cur_b - clusarr[j].mid_b;

			int min_col_diff_t = sqrt(
				pow(cur_r - clusarr[j].mid_r, 2) + pow(cur_g - clusarr[j].mid_g, 2) + pow(cur_r - clusarr[j].mid_b, 2));

			min_col_diff_t = std::abs(col_diff_b_t) + std::abs(col_diff_g_t) + std::abs(col_diff_b_t);

			// �������� ���������� � ��������� i-�
			tmp = pow_m(tmp, d_dig);
			min_col_diff_t = pow_m(min_col_diff_t, c_dig);


			double itog_t = tmp * d_coef + min_col_diff_t * c_coef;

			
			//int intensOtherCentr = (src.at<Vec3b>(clusarr[j].curX, clusarr[j].curY).val[0] + src.at<Vec3b>(clusarr[j].curX, clusarr[j].curY).val[1] +
			//	src.at<Vec3b>(clusarr[j].curX, clusarr[j].curY).val[2]) / 3;







			if (itog > itog_t) { itog = itog_t; cl = &clusarr[j]; }// ���� ����������� �������
		}
		cl->Add(vpt[i]);// ��������� � ���� ������� ������� ������� �������
	}
	return clusarr;
}








Mat Cluster::Start(int k, Cluster * clusarr, vector<POINT>& vpt, Mat &src, int c_dig, int d_dig, int c_coef, int d_coef) {
	Cluster::InitialCenter(k, clusarr, vpt, src);
	for (;;) {//��������� �������� ����
		int chk = 0;
		Cluster::Bind(k, clusarr, vpt, src, c_dig, d_dig, c_coef, d_coef);//��������� ����� � ����������

		for (int i = 0; i < k; i++)
		clusarr[i].resetMidColor(src); //������������� ������� ����� � ��������

		for (int j = 0; j < k; j++)//����������� ����� ���������� ���������� 
			clusarr[j].SetCenter();
		for (int p = 0; p < k; p++) {
			
			int ry, rx;

			ry = (clusarr[p].curY - clusarr[p].lastY) * (clusarr[p].curY - clusarr[p].lastY);
			rx = (clusarr[p].curX - clusarr[p].lastX) * (clusarr[p].curX - clusarr[p].lastX);
			//��������� �� ��������� �� ��� � ����������� ����-��
		//	if (clusarr[p].curX == clusarr[p].lastX && clusarr[p].curY == clusarr[p].lastY)
			//	chk++;
			if (clusarr[p].active_flag)
			{
				if (rx < 50 && ry < 50) // 4 ������� �� ������� �� ��������
					chk++;
			}
			else chk++;


		}


		if (chk == k) break;//return;//���� �� ������� � �����
	}


	Mat out;
	out = src.clone();

	for (int i = 0; i < k; i++) {
		for (int j = 0; j < clusarr[i].scores.size(); j++) {
			int x = clusarr[i].scores[j].x;
			int y = clusarr[i].scores[j].y;

		//	out.at<Vec3b>(x, y).val[2] = clusarr[i].mid_r;
			//out.at<Vec3b>(x, y).val[1] = clusarr[i].mid_g;
			//out.at<Vec3b>(x, y).val[0] = clusarr[i].mid_b;

			out.at<Vec3b>(x, y) = Vec3b(uchar(50 + 20 * i), uchar(30 + 20 * i), uchar(10 + 20 * i));



		}
	}
	return out;
}



template <class T>
T pow_m(T ch,int dig)
{
	T out = ch;
	for (int i = 1; i < dig; i++)
		out *= ch;

	return out;
}