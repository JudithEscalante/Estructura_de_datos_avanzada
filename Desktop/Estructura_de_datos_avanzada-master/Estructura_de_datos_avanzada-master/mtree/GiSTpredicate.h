

#ifndef GISTPREDICATE_H
#define GISTPREDICATE_H

#include <iostream>
using namespace std;
#include "GiSTdefs.h"
#include "GiSTentry.h"


class GiSTentry;

class GiSTpredicate: public GiSTobject {
public:
	GiSTobjid IsA () const { return GISTPREDICATE_CLASS; }
	virtual int Consistent (const GiSTentry& entry) const=0;
	virtual ~GiSTpredicate () {}
};

class PtrPredicate: public GiSTpredicate {
public:
	PtrPredicate (GiSTpage page): page(page) {}
	int Consistent (const GiSTentry& entry) const;
	GiSTobject *Copy () const;
#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const;
#endif

private:
	GiSTpage page;
};

class AndPredicate: public GiSTpredicate {
public:
	AndPredicate (GiSTpredicate *q1, GiSTpredicate *q2): q1(q1), q2(q2) {}
	int Consistent (const GiSTentry& entry) const { return q1->Consistent(entry) && q2->Consistent(entry); }
#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const { os << "(" << *q1 << " and " << *q2 << ")"; }
#endif
	GiSTobject *Copy () const { return new AndPredicate((GiSTpredicate*)q1->Copy(), (GiSTpredicate*)q2->Copy()); }
	~AndPredicate() {
		if (q1) {
			delete q1;
		}
		if (q2) {
			delete q2;
		}
	}

private:
	GiSTpredicate *q1, *q2;
};

class OrPredicate: public GiSTpredicate {
public:
	OrPredicate (GiSTpredicate *q1, GiSTpredicate *q2): q1(q1), q2(q2) {}
	int Consistent (const GiSTentry& entry) const { return q1->Consistent(entry) || q2->Consistent(entry); }
#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const { os << "(" << *q1 << " or " << *q2 << ")"; }
#endif
	GiSTobject *Copy () const { return new OrPredicate((GiSTpredicate*)q1->Copy(), (GiSTpredicate*)q2->Copy()); }
	~OrPredicate () {
		if (q1) {
			delete q1;
		}
		if (q2) {
			delete q2;
		}
	}

private:
	GiSTpredicate *q1, *q2;
};

class NotPredicate: public GiSTpredicate {
public:
	NotPredicate (GiSTpredicate *q): q(q) {}
	int Consistent (const GiSTentry& entry) const { return !q->Consistent(entry); }
	GiSTobject *Copy () const { return new NotPredicate((GiSTpredicate*)q->Copy()); }
#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const { os << "(not " << *q << ")"; }
#endif
	~NotPredicate () {
		if (q) {
			delete q;
		}
	}

private:
	GiSTpredicate *q;
};

class TruePredicate: public GiSTpredicate {
public:
	TruePredicate () {}
	int Consistent (const GiSTentry& entry) const { return 1; }
	GiSTobject *Copy () const { return new TruePredicate; }
#ifdef PRINTING_OBJECTS
	void Print (ostream& os) const { os << "true"; }
#endif
};

#endif
