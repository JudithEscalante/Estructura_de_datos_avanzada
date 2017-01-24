#ifndef NODE_H
#define NODE_H
#include "typedef.h"
#include <map>
#include "function.h"



using namespace std;


typedef map<Data, Indexacion*> ChildrenMap;

struct CandidateChild {
	Node* node;
	tdist dist;
};

class Indexacion{
	public:
		Data data;
		tdist radius;
		tdist distToParent;

		virtual ~Indexacion() { };

		Indexacion() ;
		Indexacion(const Indexacion&) ;
		Indexacion(Indexacion&&) ;
		Indexacion& operator=(const Indexacion&) ;
		Indexacion& operator=(Indexacion&&) ;

	protected:
		Indexacion(const Data& data)
			: data(data),
			  radius(0),
			  distToParent(-1)
			{ }

	//verficar radio y distancia al padre mayores q cero
};

class SplitNodeReplacement {
	public:
		NUM_NODES nn= 2 ;
		Node* newNodes[nn];
		SplitNodeReplacement(Node* newNodes[nn]) {
			for(int i = 0; i < nn; ++i) {
				this->newNodes[i] = newNodes[i];
			}

		}
	};

class Node : public Indexacion {
	protected:
		Node(const Data& data) : Indexacion(data) { }

		Node(const Node&) ;
		Node(Node&&) ;
		Node& operator=(const Node&) ;
		Node& operator=(Node&&) ;

		virtual void doAddData(const Data& data, tdist dist, const mtree* mtree) = 0;
		virtual void doRemoveData(const Data& data, tdist dist, const mtree* mtree) throw (DataNotFound) = 0;

	public:
		ChildrenMap children;
		virtual ~Node() {
			for(typename ChildrenMap::iterator i = children.begin(); i != children.end(); ++i) {
				Indexacion* child = i->second;
				delete child;
			}
		}

		void addData(const Data& data, tdist dist, const mtree* mtree) {
			doAddData(data, dist, mtree);
			checkMaxCapacity(mtree);
		}

	
	
		void checkMaxCapacity(const mtree* mtree)  {
			if(children.size() > mtree->maxNodeCapacity) {
				Partition firstPartition;
				for(typename ChildrenMap::iterator i = children.begin(); i != children.end(); ++i) {
					firstPartition.insert(i->first);
				}

				distancias_almacenadas_type distancias_almacenadas(mtree->dist_func);

				Partition secondPartition;
				PromotedPair promoted = mtree->split_func(firstPartition, secondPartition, distancias_almacenadas);

				Node* newNodes[2];
				for(int i = 0; i < 2; ++i) {
					Data promotedData    = (i == 0) ? promoted.first : promoted.second;
					Partition partition = (i == 0) ? firstPartition : secondPartition;

					Node* newNode(promotedData);
					for(typename Partition::iterator j = partition.begin(); j != partition.end(); ++j) {
						const Data& data = *j;
						Indexacion* child = children[data];
						children.erase(data);
						tdist dist = distancias_almacenadas(promotedData, data);
						newNode->addChild(child, dist, mtree);
					}

					newNodes[i] = newNode;
				}

			}       

		}


	class RootNode : public virtual Node {
		void checkDistToParent() const {
			assert(this->distToParent == -1);
		}
	};

	class RootNode : public RootNode, public NonLeafNode {
	public:
		RootNode(const Data& data) : Node(data) {}

	private:
		void removeData(const Data& data, tdist dist, const mtree* mtree) throw (RootNodeReplacement, NodeUnderCapacity, DataNotFound) {
			try {
				Node::removeData(data, dist, mtree);
			} catch(NodeUnderCapacity&) {
				// Promote the only child to root
				Node* theChild = dynamic_cast<Node*>(this->children.begin()->second);
				Node* newRoot;
				if(dynamic_cast<InternalNode*>(theChild) != NULL) {
					newRoot = new RootNode(theChild->data);
				} else {
					assert(dynamic_cast<LeafNode*>(theChild) != NULL);
					newRoot = new RootLeafNode(theChild->data);
				}

				for(typename Node::ChildrenMap::iterator i = theChild->children.begin(); i != theChild->children.end(); ++i) {
					Indexacion* grandchild = i->second;
					tdist dist = mtree->dist_func(newRoot->data, grandchild->data);
					newRoot->addChild(grandchild, dist, mtree);
				}
				theChild->children.clear();

				throw RootNodeReplacement{newRoot};
			}
		}


		size_t getMinCapacity(const mtree* mtree) const {
			return 2;
		}

		void _checkMinCapacity(const mtree* mtree) const {
			assert(this->children.size() >= 2);
		}
	};


	class NonRootNode : public virtual Node {
		size_t getMinCapacity(const mtree* mtree) const {
			return mtree->minNodeCapacity;
		}

		void checkMinCapacity(const mtree* mtree) const {
			assert(this->children.size() >= mtree->minNodeCapacity);
		}
	};


	class LeafNode : public virtual Node {
		void doAddData(const Data& data, tdist dist, const mtree* mtree) {
			Entry* entry = new Entry(data);
			assert(this->children.find(data) == this->children.end());
			this->children[data] = entry;
			assert(this->children.find(data) != this->children.end());
			updateMetrics(entry, dist);
		}

		void addChild(Indexacion* child, tdist dist, const mtree* mtree) {
			assert(this->children.find(child->data) == this->children.end());
			this->children[child->data] = child;
			assert(this->children.find(child->data) != this->children.end());
			updateMetrics(child, dist);
		}

		Node* newSplitNodeReplacement(const Data& data) const {
			return new LeafNode(data);
		}

		void doRemoveData(const Data& data, tdist dist, const mtree* mtree) throw (DataNotFound) {
			if(this->children.erase(data) == 0) {
				this->delete;
			}
		}


	};


	class NonLeafNode : public virtual Node {
		void doAddData(const Data& data, tdist dist, const mtree* mtree) {

			CandidateChild minRadiusIncreaseNeeded = { NULL, -1.0, std::numeric_limits<tdist>::infinity() };
			CandidateChild nearestDist         = { NULL, -1.0, std::numeric_limits<tdist>::infinity() };

			for(typename Node::ChildrenMap::iterator i = this->children.begin(); i != this->children.end(); ++i) {
				Node* child(i->second);
				assert(child != NULL);
				tdist dist = mtree->dist_func(child->data, data);
				if(dist > child->radius) {
					tdist radiusIncrease = dist - child->radius;
					if(radiusIncrease < minRadiusIncreaseNeeded.metric) {
						minRadiusIncreaseNeeded = { child, dist, radiusIncrease };
					}
				} else {
					if(dist < nearestDist.metric) {
						nearestDist = { child, dist, dist };
					}
				}
			}

			CandidateChild chosen = (nearestDist.node != NULL)
			                      ? nearestDist
			                      : minRadiusIncreaseNeeded;

			Node* child = chosen.node;
			
		}



		void doRemoveData(const Data& data, tdist dist, const mtree* mtree) throw (DataNotFound) {
			for(typename Node::ChildrenMap::iterator i = this->children.begin(); i != this->children.end(); ++i) {
				Node* child = dynamic_cast<Node*>(i->second);
				assert(child != NULL);
				if(abs(dist - child->distToParent) <= child->radius) {
					tdist distToChild = mtree->dist_func(data, child->data);
					if(distToChild <= child->radius) {
						child->removeData(data, distToChild, mtree);
						updateRadius(child);
						return;
					}
				}
			}
		}


		Node* balanceChildren(Node* theChild, const mtree* mtree) {
			// Tries to find anotherChild which can donate a grand-child to theChild.

			Node* nearestDonor = NULL;
			tdist distNearestDonor = std::numeric_limits<tdist>::infinity();

			Node* nearestMergeCandidate = NULL;
			tdist distNearestMergeCandidate = std::numeric_limits<tdist>::infinity();

			for(typename Node::ChildrenMap::iterator i = this->children.begin(); i != this->children.end(); ++i) {
				Node* anotherChild = dynamic_cast<Node*>(i->second);
				assert(anotherChild != NULL);
				if(anotherChild == theChild) continue;

				tdist dist = mtree->dist_func(theChild->data, anotherChild->data);
				if(anotherChild->children.size() > anotherChild->getMinCapacity(mtree)) {
					if(dist < distNearestDonor) {
						distNearestDonor = dist;
						nearestDonor = anotherChild;
					}
				} else {
					if(dist < distNearestMergeCandidate) {
						distNearestMergeCandidate = dist;
						nearestMergeCandidate = anotherChild;
					}
				}
			}

			if(nearestDonor == NULL) {
				// Merge
				for(typename Node::ChildrenMap::iterator i = theChild->children.begin(); i != theChild->children.end(); ++i) {
					Indexacion* grandchild = i->second;
					tdist dist = mtree->dist_func(grandchild->data, nearestMergeCandidate->data);
					nearestMergeCandidate->addChild(grandchild, dist, mtree);
				}

				theChild->children.clear();
				this->children.erase(theChild->data);
				delete theChild;
				return nearestMergeCandidate;
			} else {
				// Donate
				// Look for the nearest grandchild
				Indexacion* nearestGrandchild;
				tdist nearestGrandchildDist = std::numeric_limits<tdist>::infinity();
				for(typename Node::ChildrenMap::iterator i = nearestDonor->children.begin(); i != nearestDonor->children.end(); ++i) {
					Indexacion* grandchild = i->second;
					tdist dist = mtree->dist_func(grandchild->data, theChild->data);
					if(dist < nearestGrandchildDist) {
						nearestGrandchildDist = dist;
						nearestGrandchild = grandchild;
					}
				}

				theChild->addChild(nearestGrandchild, nearestGrandchildDist, mtree);
				return theChild;
			}
		}


		
	};



	class InternalNode : public NonRootNode, public NonLeafNode {
	public:
		InternalNode(const Data& data) : Node(data) { }
	};



};


#endif