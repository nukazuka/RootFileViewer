#ifndef DRAWER_HH
#define DRAWER_HH

#include "TCanvas.h"
#include "TTree.h"
#include "TPad.h"
#include "TLatex.h"

#include "Argument.hh"
#include "GetName.hh"
#include "misc.hh"
#include "GetRange.hh"

#include "MyLibrary.hh"
class Drawer
{
private:
  Argument* arg_;
  vector < TTree* > vtr_;
  vector < string > vcut_;
  vector < string > vbranch_name_;
  TCanvas* c_;

  int    branch_num_;
  string save_;
  const int kColor_[10] = { kBlack   , kRed      , kBlue    , kMagenta+1 ,
				   kAzure+1 , kYellow-3 , kGreen+2 , kOrange+2  ,
				   kTeal-5  , kPink-1   };




  void Init();
  //  void Draw();
  void Draw();
  void DrawInfo();
  void DrawPad( TVirtualPad* pad, vector < TH1D* >& vhist, string branch_name, bool ratio);

  void GetVectorHist( string branch_name,vector < TH1D* >& vhist );
  TH1D* GetHist( int num, string file_name, TTree* tr, string branch_name, string cut, int bin, double xmin, double xmax );
  
  void SetHist( TH1D* hist, int num );
public:

  Drawer( Argument* arg );

};

#endif
