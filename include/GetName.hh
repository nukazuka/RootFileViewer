#ifndef GETNAME_HH
#define GETNAME_HH

// get name of branches which are common both tr1 and tr2


#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "TTree.h"
#include "TLeaf.h"

#include "misc.hh"

using namespace std;
// naive mode

/*!
  @fn void GetName(  vector < TTree* >& tr, vector < string >& vname);
  @brief get branch names which are common between all TTrees
*/
//void GetName(  vector < TTree* >& tr, vector < string >& vname);
vector < string > GetName(  vector < TTree* >& tr );

#endif
