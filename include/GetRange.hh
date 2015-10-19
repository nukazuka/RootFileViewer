#ifndef GETRANGE_HH
#define GETRANGE_HH

// get some values to draw histogram in same canvas
/*
void GetRange( vector < TTree* >& tr, string branch_name, vector < string >& vcut, double& min, double& max)

// 2 TTree version
void GetRange( TTree* tr1, TTree* tr2, string branch_name, vector < string >& vcut, double& min, double& max)

double GetMin( TH1D* hist )
double GetMax( TH1D* hist )
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include "TTree.h"
#include "TH1.h"
#include "TPad.h"

#include "MyLibrary.hh"

using namespace std;

// vector < TTree* > and vector < string > vcut version, most flexible
void GetRange( vector < TTree* >& tr, string branch_name, vector < string >& vcut, double& min, double& max);

// 2 TTree* and string cut version
void GetRange( TTree* tr1, TTree* tr2, string branch_name, string cut, double& min, double& max);

// 2 TTree* and vector < string > vcut version
void GetRange( TTree* tr1, TTree* tr2, string branch_name, vector < string >& vcut, double& min, double& max);

double GetMaxOrMin( TH1D* hist, string mode );

double GetMin( TH1D* hist );
double GetMax( TH1D* hist );


#endif
