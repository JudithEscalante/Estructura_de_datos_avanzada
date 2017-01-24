#ifndef GISTPATH_H
#define GISTPATH_H
#include <vector>
using namespace std;

#define GiSTRootPage 1
#define GIST_MAX_LEVELS 16

class GiSTpath : public GiSTobject {
public:
	GiSTpath () { pages.clear(); }
	GiSTpath (const GiSTpath& path) {
		pages.clear();
		for (int i=0; i<path.pages.size(); i++) {
			pages.push_back(path.pages[i]);
		}
	}

#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const {
		os << '[';
		for (int i=0; i<pages.size(); i++) {
			if (i) {
				os << ' ';
			}
			os << pages[i];
		}
		os << ']';
	}
#endif

	int Level () const { return (pages.size() - 1); }  // level=len-1
	void Clear () { pages.clear(); }
	void MakeParent () { pages.resize(pages.size()-1); }
	void MakeSibling (GiSTpage page) { pages[pages.size()-1] = page; }
	void MakeChild (GiSTpage page) { pages.push_back(page); }
	void MakeRoot () { pages.clear(); pages.push_back(GiSTRootPage); }

	void SplitRoot (GiSTpage page) {
		pages.push_back(0);
		for (int i=pages.size()-1; i>1; i--) {
			pages[i] = pages[i-1];
		}
		pages[1] = page;
		pages[0] = GiSTRootPage;
	}

	GiSTpath& operator = (const GiSTpath& path) {
		pages.clear();
		for (int i=0; i<path.pages.size(); i++) {
			pages.push_back(path.pages[i]);
		}
		return *this;
	}

	int operator == (const GiSTpath& path) const {
		if (pages.size() != path.pages.size()) {
			return(0);
		}
		for (int i=0; i<pages.size(); i++) {
			if (pages[i] != path.pages[i]) {
				return(0);
			}
		}
		return(1);
	}

	GiSTpage Page () const { return pages[pages.size()-1]; }
	GiSTpage Parent () const { return pages.size()>=2 ? pages[pages.size()-2] : 0; }
	int IsRoot () const { return (pages.size() == 1); }

private:
	vector<GiSTpage> pages;
};

#endif
