#ifndef MTREE_H_
#define MTREE_H_

#include "node.h"
#include <iterator>
#include <limits>
#include <map>
#include <queue>
#include <utility>
#include "funcs.h"
typedef DistFunc dist_func_type;
typedef SplitFunc    split_func_type;
typedef functions::distancias_almacenadas<Data, DistFunc> distancias_almacenadas_type;


template <
	typename Data,
	typename DistFunc = ::functions::distancia_euclideana,
	typename SplitFunc = ::functions::split_func<
	        ::functions::elegidos_random,
	        ::functions::particion_balanceada
		>
>

class Entry : public Indexacion {
public:
	Entry(const Data& data) : Indexacion(data) { }
};

class mtree {
private:

	typedef std::pair<Data, Data> PromotedPair;
	typedef std::set<Data> Partition;


	tsize minNodeCapacity;
	tsize maxNodeCapacity;
	Node* root;

public:

	mtree(
			tsize min_node_capacity = 2,
			tsize max_node_capacity = -1,
			const DistFunc& dist_func = DistFunc(),
			const SplitFunc& split_func = SplitFunc()
		)
		: minNodeCapacity(min_node_capacity),
		  maxNodeCapacity(max_node_capacity),
		  root(NULL),
		  dist_func(dist_func),
		  split_func(split_func)
	{
		if(max_node_capacity == tsize(-1)) {
			this->maxNodeCapacity = 2 * min_node_capacity - 1;
		}
	}


	~mtree() {
		delete root;

	}


	void add(const Data& data) {
		if(root == NULL) {
			root = new RootLeafNode(data);
			root->addData(data, 0, this);
		} else {
			tdist dist = dist_func(data, root->data);

			Node* newRoot = new RootNode(root->data);
			delete root;
			root = newRoot;
			for(int i = 0; i < SplitNodeReplacement::NUM_NODES; ++i) {
				Node* newNode = e.newNodes[i];
				tdist dist = dist_func(root->data, newNode->data);
				root->addChild(newNode, dist, this);
			}
			root->addData(data, dist, this);
			
		}
	}

	//limit=10

	query get_nearest(const Data& query_data, tdist range, tsize limit) const {
		return {this, query_data, range, limit};
	}


}

class result_item {
	public:
		Data data;

		tdist dist;

		result_item();

		~result_item();

		result_item& operator=(result_item&& ri) {
			if(this != &ri) {
				data = std::move(ri.data);
				dist = ri.dist;
			}
			return *this;
		}
};

class query {
public:

	query() ;

	query(const mtree* _mtree, const Data& data, tdist range, tsize limit)
		: _mtree(_mtree), data(data), range(range), limit(limit)
		{}


private:
	const mtree* _mtree;
	Data data;
	tdist range;
	tsize limit;
	vector<result_item> results;
};





#endif /* MTREE_H_ */
