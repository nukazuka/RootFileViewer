#include "Drawer.hh"

using namespace std;

Drawer::Drawer( Argument* arg )
{

  arg_ = arg;

  Init();

  c_->Print( (save_+"[").c_str() );
  Draw();
  c_->Print( (save_+"]").c_str() );

}



// private function

void Drawer::Init()
{

  // making canvas
  if( arg_->IsBoth() == true )
    c_ = GetCanvas( "canvas", "double", false);
  else
    c_ = GetCanvas( "canvas", "square", false );

  save_ = arg_->GetSaveName();
  arg_->GetVectorTree( vtr_ );
  arg_->GetVectorCut( vcut_ );

  GetName( vtr_, vbranch_name_ );
  //  ShowVC( vbranch_name_ );

  if( arg_->IsTest() )
    branch_num_ = 1;
  else
    branch_num_ = vbranch_name_.size();
}

void Drawer::Draw()
{

  cout << "cut:" << endl;
  ShowVC( vcut_ );
  
  for( int i=0; i<branch_num_; i++ )
    {
      
      cout << setw( 25 ) << left << vbranch_name_[i] ;
      if( i%3 == 2 ) 
	cout << endl;

      vector < TH1D* > vhist;
      GetVectorHist( vbranch_name_[i], vhist );

      if( arg_->IsBoth() )
	{
	  c_->Divide( 2, 1 );

	  cout << "Double mode" << endl;
	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  false );
	  DrawPad( c_->cd(2), vhist, vbranch_name_[i],  true );
	}
      else if( arg_->IsRatio() )
	{
	  cout << "Ratio mode" << endl;
	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  true );
	}
      else
	{
	  cout << "Normal mode" << endl;
	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  false );
	}

      //      vhist.erase( vhist.begin(), vhist.end() );
      // for( unsigned int j=0; j<vhist.size(); j++ )
      // 	{
      // 	  //	  cout << j << endl;
      // 	  delete vhist[j];
      // 	}


      //vector < TH1D* >().swap( vhist );

      // for( unsigned int j=0; j<vhist.size(); j++ )
      // 	{
      // 	  TObject* old = gROOT->FindObject( vhist[j]->GetName() );

      // 	  if(old)
      // 	  delete old;
      // 	}

      string page_title = "Title: " + Replace( vbranch_name_[i], "tex", "txe" );
      c_->Print( save_.c_str(), page_title.c_str() );
      c_->Clear();

    }
  



}

void Drawer::DrawPad( TVirtualPad* pad , vector < TH1D* >& vhist, string branch_name, bool ratio )
{

  PadSetting();

  if( ratio == false )
    {

      gPad->SetLogx( arg_->IsLogx() );
      gPad->SetLogy( arg_->IsLogy() );

      string option = arg_->GetDrawOption();
      for( unsigned int i=0; i<vtr_.size(); i++ )
	{

	  if( i==1 )
	    option += " SAMES";

	  vhist[i]->Draw( option.c_str() );

	  DrawStats( vhist[i], 0.85, 0.8 - 0.1*i, 1.0, 0.9 - 0.1*i);
	}
    }
  else
    {

      if( vtr_.size() < 2 )
	{

	  cerr << "void Drawer::DrawPad( TVirtualPad* pad , vector < TH1D* >& vhist, string branch_name, bool ratio )" << endl;
	  cerr << "A nuber of data file is less than 2" << endl;
	  cerr << "So a ratio cannot be defilend" << endl;
	  cerr << "Program is stoppped" << endl;
	  exit( -1 );
	}

      TH1D* htemp[vtr_.size()];
      htemp[0] = (TH1D*)vhist[0]->Clone();
      string option = arg_->GetDrawRatioOption();

      for( unsigned int i=1; i<vtr_.size(); i++ )
	{

	  htemp[i] = (TH1D*)vhist[i]->Clone();

	  if( gPad->GetCanvasID() > 1 )
	    htemp[i]->SetTitle( "Ratio" );
	  else
	    htemp[i]->SetTitle( ((string)vhist[i]->GetTitle() + " ratio" ).c_str() );


	  htemp[i]->SetLineWidth( 1 );
	  htemp[i]->SetMarkerStyle(7);
	  htemp[i]->Divide( htemp[0] );

	  htemp[i]->Draw( option.c_str() );
	  option += "SAMES";
	  DrawStats( htemp[i], 0.85, 0.8 - 0.1*(i-1), 1.0, 0.9 - 0.1*(i-1) );
	}

    }

  DrawTitle( gPad, 0.07 );
  gPad->Update();
}

void Drawer::GetVectorHist( string branch_name, vector < TH1D* >& vhist )
{

  double xmin = 0.0, xmax = 1.0;
  GetRange( vtr_, branch_name, vcut_, xmin, xmax);
  int bin = xmax - xmin;

  if( bin <= 1 )
    bin = 100;

  while( bin > 500 )
    bin = bin/10;

  if( branch_name.find("Hit_DC") != string::npos )
    bin = bin/2;

  for( unsigned int i=0; i<vtr_.size(); i++ )
    {

      vhist.push_back( GetHist( i, arg_->GetFileName(i),
			      vtr_[i], 
			      branch_name.c_str(),
			      vcut_[i],
			      bin, xmin, xmax 
				)
		       );
      }
}

TH1D* Drawer::GetHist( int num, string file_name, TTree* tr, string branch_name, string cut, int bin, double xmin, double xmax )
{

  string name = GetBaseName(file_name);
  string title = branch_name;
  //  TH1D* hist = new TH1D( name.c_str(), title.c_str(), bin, xmax, xmin );

  //  cout << "GetHist, " << file_name << "\t" << name << "\t" << hist->GetNbinsX() << endl;
  /*
  stringstream ss, ss_title;
  ss << branch_name << ">>" << name;

  cout << ss.str() << endl;
  tr->Draw( ss.str().c_str(), cut.c_str(), "goff" );
  cout << "GetHist, " << file_name << "\t" << hist->GetNbinsX() << endl;
  */

  stringstream ss, ss_title;
  ss << branch_name << ">>temp" 
     << "(" 
     << bin << "," 
     << xmin << "," 
     << xmax << ")";

  int entry = tr->Draw( ss.str().c_str(), cut.c_str(), "goff" );

  if( entry == 0 )
    {
      cerr << "TH1D* Drawer::GetHist( int num, string file_name, TTree* tr, string branch_name, string cut, int bin, double xmin, double xmax )" << endl;

      int width = 30;
      cerr << setw(width) << "int num: " << num << endl;
      cerr << setw(width) << "string file_name: " << file_name << endl;
      cerr << setw(width) << "TTree* tr: " << tr->GetName() << endl;
      cerr << setw(width) << "string branch_name: " << branch_name << endl;
      cerr << setw(width) << "string cut: " << cut << endl;
      cerr << setw(width) << "int bin: " << bin << endl;
      cerr << setw(width) << "double xmin: " << xmin << endl;
      cerr << setw(width) << "double xmax: " << xmax << endl;

      cerr << "The result of draw:" << endl;
      cerr << "Draw(" << ss.str() << "\n, " << cut << "\n, goff) == 0" << endl;
      exit(-1);
    }

  TH1D* hist = (TH1D*)gDirectory->Get( "temp" );
  hist->SetName( name.c_str() );
  hist->SetTitle( title.c_str() );

  SetHist( hist, num );

  if( arg_->IsNorm() )
    hist->Scale( arg_->GetNormVal() / hist->Integral() );

  return hist;
}

void Drawer::SetHist( TH1D* hist, int num )
{

  hist->Sumw2();
  hist->SetLineWidth( 2 );

  if( num < 10 )
    {
      hist->SetLineColor( kColor_[num] );
      hist->SetMarkerColor( kColor_[num] );
    }
  else
    {
      hist->SetLineColor( kBlack );
      hist->SetMarkerColor( kBlack );
    }


}
