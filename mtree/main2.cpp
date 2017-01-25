#include <iostream>
using namespace std;
#include <vector>
#include "Mtree.h"
#define treshold 0.8
#define limit 10

vector<d1> imagen; //vector a buscar
vector<vector<d1>> vectorr;
int main(){
	Mtree arbolito();
	for (vector<vector<d1>>::iterator it = vectorr.begin() ; it != vectorr.end(); ++it)
	{
		arbolito.add(*it);
	}
	query resultados;
	resultados=arbolito.get_nearest(imagen,treshold,limit);

	for (vector<result_item>::iterator it = (resultados.result).begin() ; it != (resultados.result).end(); ++it)
	{
		show->*it.data;
	}

}

