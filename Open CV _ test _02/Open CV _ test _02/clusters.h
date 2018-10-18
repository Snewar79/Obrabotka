#pragma once
#include <vector>
#include <stdlib.h>
#include <windows.h>


using namespace std;

class Cluster {
public:
	vector<POINT> scores;

	int curX, curY;//координаты текущего центроида
	int lastX, lastY;//координаты предыдущего центоида
	size_t Size() { return scores.size(); }//получаем размер вектора
	inline void Add(POINT pt) { scores.push_back(pt); }//Добавляем пиксель к кластеру
	void SetCenter();
	void Clear();//Чистим вектор
	static Cluster* Bind(int k, Cluster * clusarr, vector<POINT>& vpt);
	static void InitialCenter(int k, Cluster * clusarr, vector<POINT>& vpt);;
	static void Start(int k, Cluster * clusarr, vector<POINT>& vpt);
	inline POINT& at(unsigned i) { return scores.at(i); }//Доступ  к элементам вектора
};



void Cluster::InitialCenter(int k, Cluster * clusarr, vector<POINT>& vpt) {

	int size = vpt.size();
	int step = size / k;
	int steper = 0;

	for (int i = 0; i < k; i++, steper += step) {
		clusarr[i].curX = vpt[steper].x;
		clusarr[i].curY = vpt[steper].y;
	}
}




void Cluster::SetCenter() {
	int sumX = 0, sumY = 0;
	int i = 0;
	int size = Size();
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







Cluster * Cluster::Bind(int k, Cluster * clusarr, vector<POINT>& vpt) {
	for (int j = 0; j < k; j++)
		clusarr[j].Clear();// Чистим кластер перед использованием
	int size = vpt.size();
	for (int i = 0; i < size; i++) {// Запускаем цикл по всем пикселям множества
		int min = sqrt(
			pow((float)clusarr[0].curX - vpt[i].x, 2) + pow((float)clusarr[0].curY - vpt[i].y, 2)
		);
		Cluster * cl = &clusarr[0];
		for (int j = 1; j < k; j++) {
			int tmp = sqrt(
				pow((float)clusarr[j].curX - vpt[i].x, 2) + pow((float)clusarr[j].curY - vpt[i].y, 2)
			);
			if (min > tmp) { min = tmp; cl = &clusarr[j]; }// Ищем близлежащий кластер
		}
		cl->Add(vpt[i]);// Добавляем в близ лежащий кластер текущий пиксель
	}
	return clusarr;
}










void Cluster::Start(int k, Cluster * clusarr, vector<POINT>& vpt) {
	Cluster::InitialCenter(k, clusarr, vpt);
	for (;;) {//Запускаем основной цикл
		int chk = 0;
		Cluster::Bind(k, clusarr, vpt);//Связываем точки с кластерами
		for (int j = 0; j < k; j++)//Высчитываем новые координаты центроидов 
			clusarr[j].SetCenter();
		for (int p = 0; p<k; p++)//Проверяем не совпадают ли они с предыдущими цент-ми
			if (clusarr[p].curX == clusarr[p].lastX && clusarr[p].curY == clusarr[p].lastY)
				chk++;
		if (chk == k) return;//Если да выходим с цикла
	}
}