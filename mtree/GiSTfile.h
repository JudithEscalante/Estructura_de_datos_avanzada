#ifndef GISTFILE_H
#define GISTFILE_H

#include "GiSTstore.h"


class GiSTfile: public GiSTstore {
public:
  GiSTfile (): GiSTstore() {}

  void Create (const char *filename);
  void Open (const char *filename);
  void Close ();

  void Read (GiSTpage page, char *buf);
  void Write (GiSTpage page, const char *buf);
  GiSTpage Allocate ();
  void Deallocate (GiSTpage page);
  void Sync () {}
  int PageSize () const { return 4096; }

private:
  int fileHandle;
};

#endif
