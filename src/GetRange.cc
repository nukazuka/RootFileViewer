#include "GetRange.hh"

// get some values to draw histogram in same canvas
/*
void GetRange( vector < TTree* >& tr, string branch_name, vector < string >& vcut, double& min, double& max)

// 2 TTree version
void GetRange( TTree* tr1, TTree* tr2, string branch_name, vector < string >& vcut, double& min, double& max)

double GetMin( TH1D* hist )
double GetMax( TH1D* hist )
 */

using namespace std;

void GetRange
(
 vector < TTree* >& tr,
 string branch_name,
 vector < string >& vcut,
 double& min,
 double& max
)
{
  
  const int num = tr.size();
  vector < double > vmin_temp, vmax_temp;

  TH1D* hist[num];

  for( int i=0; i<num; i++)
    {
    
      tr[i]->Draw( branch_name.c_str(), vcut[i].c_str(), "" );
      string hist_name = "hist" + Int2String(i);
      hist[i] = (TH1D*)(gPad->GetPrimitive( "htemp" ))->Clone( hist_name.c_str() );
      
      double max_temp = GetMaxOrMin( hist[i], "max" );
      double min_temp = GetMaxOrMin( hist[i], "min" );

      if( (double)hist[i]->GetBinWidth(0) > (max_temp - min_temp)/4 )
	{

	  vmax_temp.push_back( max_temp + hist[i]->GetBinWidth(0) );
	  vmin_temp.push_back( min_temp + hist[i]->GetBinWidth(0) );
	}
      else
	{

	  vmax_temp.push_back( max_temp );
	  vmin_temp.push_back( min_temp );

	}
    }

  if( min < -1.5 )
    min = floor( *min_element( vmin_temp.begin(), vmin_temp.end() ) );
  else 
    min = *min_element( vmin_temp.begin(), vmin_temp.end() );

  // ceil: Rounds x upward, returning the smallest integral value that is not less than x.
  max = ceil( *max_element( vmax_temp.begin(), vmax_temp.end() ) ) ;

  gPad->Clear();
}

void GetRange( TTree* tr1, TTree* tr2, string branch_name, string cut, double& min, double& max)
{

  vector < string > vcut(2);
  vcut[0] = cut;
  vcut[1] = cut;

  GetRange( tr1, tr2, branch_name, vcut, min, max );
}

void GetRange( TTree* tr1, TTree* tr2, string branch_name, vector < string >& vcut, double& min, double& max)
{
  
  if( vcut.size() != 2 )
    {

      cerr << "void GetRange( TTree* tr1, TTree* tr2, string branch_name, vector < string >& vcut, double& min, double& max)" << endl;
      cerr << " (short version)" << endl;
      cerr << "size of vcut is not 2 but " << vcut.size() << endl;
      cerr << "program is stopped" << endl;
      exit( -1 );
    }

  vector < TTree* > vtree;
  vtree.push_back( tr1 );
  vtree.push_back( tr2 );
  
  GetRange( vtree, branch_name, vcut, min, max );  
}

// return maximum or minimum of histogram
double GetMaxOrMin( TH1D* hist, string mode )
{

  if( mode != "min" && mode != "max" )
    {

      cerr << "GetRange.cc" << endl;
      cerr << "double GetMaxOrMin( TH1D* hist, string mode )" << endl;
      cerr << "2nd argument must be \"min\" or \"max\" but " << mode << " is given" << endl;
      exit( -1 );
    }

  const int kBin = hist->GetNbinsX();
  int counter = 0;
  int bin_index = -1;

  // find the bin which has non 0 entry
  // search begins from the end of histogram
  if( mode == "min" )
    {

      while( hist->GetBinContent( counter ) == 0.0 )
	counter++;    
    }
  else if( mode == "max" )
    {

      while( hist->GetBinContent( kBin - counter ) == 0.0 )
	counter++;
    }

  // back to 1 step before
  counter--;      

  (mode == "min") ? bin_index = counter : bin_index = kBin - counter ;

  double center = hist->GetXaxis()->GetBinCenter( bin_index );
  double width  = hist->GetXaxis()->GetBinWidth( bin_index );

  return center-width;
}
