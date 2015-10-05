#ifndef GETNAME_HH
#define GETNAME_HH

// get name of branches which are common both tr1 and tr2


#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "TTree.h"

#include "misc.hh"

using namespace std;
// naive mode
void GetName(  vector < TTree* >& tr, vector < string >& vname);

#endif
