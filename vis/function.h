#ifndef FUNCS_H_
#define FUNCS_H_
#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include <vector>
#include <iostream>
#include <math>
#include "typedef.h"
using namespace std;

namespace mt { // para diferenciar funciones repetidas
namespace functions { // para diferenciar funciones repetidas 
template <typename Vector> //Sequence == vector
struct distancia_euclidiana  //sacar la distanca entre dos vectores 
{   template <typename Vector>
	d1 operator()(const Vector& dato1, const Vector& dato2)
	{   tdist distancia = 0;
		for(auto i1 = dato1.begin(), i2 = dato2.begin(); i1 != dato1.end()  &&  i2 != dato2.end(); ++i1, ++i2) 
		{
			d1 diferencia = *i1 - *i2;
			distancia += pow (diferencia,2);
		}
		distancia = sqrt(distancia);
		return distancia;
	} 
};


struct elegidos_random {

	template <typename Dato, typename distFunc>
	pair<Dato, Dato> operator()(const set<Dato>& Dato_objects, distFunc& dist_func)
	{
		vector<Dato> elegidos;
		random_sample_n(Dato_objects.begin(), Dato_objects.end(), inserter(elegido, elegidos.begin()), 2);
		if (elegidos.size()==2)
		{
			return {elegidos[0], elegidos[1]};
		}
		else
		  cout<<"No se realizo correctamente " ;

	}
};



struct particion_balanceada {
	
	template <typename Dato, typename distFunc>
	void operator()(const pair<Dato, Dato>& elegidos,
	                set<Dato>& first_partition,
	                set<Dato>& second_partition,
	                distFunc& dist_func
	            ) 
	{
		vector<Dato> queue1(first_partition.begin(), first_partition.end());
		sort(queue1.begin(), queue1.end(),
			[&](const Dato& Dato1, const Dato& Dato2) {
				tdist dist1 = dist_func(Dato1, elegidos.first);
				tdist dist2 = dist_func(Dato2, elegidos.first);
				return dist1 < dist2;
			}
		);

		vector<Dato> queue2(first_partition.begin(), first_partition.end());
		sort(queue2.begin(), queue2.end(),
			[&](const Dato& Dato1, const Dato& Dato2) {
				tdist dist1 = dist_func(Dato1, elegidos.second);
				tdist dist2 = dist_func(Dato2, elegidos.second);
				return dist1 < dist2;
			}
		);

		first_partition.clear();

		typename vector<Dato>::iterator i1 = queue1.begin();
		typename vector<Dato>::iterator i2 = queue2.begin();

		while(i1 != queue1.end()  ||  i2 != queue2.end()) {
			while(i1 != queue1.end()) {
				Dato& Dato = *i1;
				++i1;
				if(second_partition.find(Dato) == second_partition.end()) {
					first_partition.insert(Dato);
					break;
				}
			}

			while(i2 != queue2.end()) {
				Dato& Dato = *i2;
				++i2;
				if(first_partition.find(Dato) == first_partition.end()) {
					second_partition.insert(Dato);
					break;
				}
			}
		}
	}
};


template <typename PromotionFunc, typename PartitionFunc>
struct split_func {
	/** */
	typedef PromotionFunc promotion_func_type;

	/** */
	typedef PartitionFunc partition_func_type;

	PromotionFunc promotion_func;
	PartitionFunc partition_func;

	/** */
	explicit split_func
(
			PromotionFunc promotion_func = PromotionFunc(),
			PartitionFunc partition_func = PartitionFunc()
		)
	: promotion_func(promotion_func),
	  partition_func(partition_func)
	{}


	template <typename Dato, typename distFunc>
	pair<Dato, Dato> operator()(
				set<Dato>& first_partition,
				set<Dato>& second_partition,
				distFunc& dist_func
			) 
	{
		pair<Dato, Dato> promoted = promotion_func(first_partition, dist_func);
		partition_func(promoted, first_partition, second_partition, dist_func);
		return promoted;
	}
};



template <typename Dato, typename distFunc>
class distancias_almacenadas {
public:
	explicit distancias_almacenadas(const distFunc& dist_func)
		: dist_func(dist_func)
		{}

	tdist operator()(const Dato& Dato1, const Dato& Dato2) {
		typename distancias::iterator i = distancias.find(make_pair(Dato1, Dato2));
		if(i != distancias.end()) {
			return i->second;
		}

		i = distancias.find(make_pair(Dato2, Dato1));
		if(i != distancias.end()) {
			return i->second;
		}

		tdist dist = dist_func(Dato1, Dato2);

		distancias.insert(make_pair(make_pair(Dato1, Dato2), dist));
		distancias.insert(make_pair(make_pair(Dato2, Dato1), dist));

		return dist;
	}

private:
	alm_dists distancias;

	const distFunc& dist_func;

};



} 
} 


#endif /* FUNCTION_H_ */