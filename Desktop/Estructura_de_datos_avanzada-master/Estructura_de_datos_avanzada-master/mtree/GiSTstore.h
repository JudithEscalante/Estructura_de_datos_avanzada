

#ifndef GISTSTORE_H
#define GISTSTORE_H

#include "GiSTdefs.h"



class GiSTstore {
public:
	GiSTstore (): isOpen(0) {}
	virtual void Create (const char *filename)=0;  // pure virtual function
	virtual void Open (const char *filename)=0;
	virtual void Close ()=0;
	virtual void Read (GiSTpage page, char *buf)=0;
	virtual void Write (GiSTpage page, const char *buf)=0;
	virtual GiSTpage Allocate ()=0;
	virtual void Deallocate (GiSTpage page)=0;
	virtual void Sync ()=0;
	virtual int IsOpen () { return isOpen; }
	virtual int PageSize () const=0;
	virtual ~GiSTstore () { }
protected:
	void SetOpen (int o) { isOpen = o; }
private:
	int isOpen;
};

#endif
