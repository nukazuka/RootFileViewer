#include "Drawer.hh"

using namespace std;

Drawer::Drawer( Argument* arg )
{

  arg_ = arg;

  Init();

  c_->Print( (save_+"[").c_str() );
  DrawInfo();
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

void Drawer::DrawInfo()
{

  TLatex* tex = new TLatex();
  double position = 0;

  // file
  tex->SetTextSizePixels( 36 );
  tex->DrawLatexNDC( 0.0 , 0.95 , "Data Files:" );
  position += tex->GetTextSize() * 1.2;
  tex->SetTextSizePixels( 28 );
  position += tex->GetTextSize() * 1.05;

  for( int i=0; i < arg_->GetFileNum(); i++ )
    {

      unsigned int  x, y;
      tex->GetBoundingBox( x , y , false );
      cout << x << "\t" << y << endl;
      tex->SetTextColor( kColor_[i] );
      //      tex->DrawLatexNDC( 0.05 , 0.9 - 0.1*i , arg_->GetFileName(i).c_str() );
      //      tex->DrawLatex( 0 , height - 28*1.2*i , arg_->GetFileName(i).c_str() );
      //      tex->DrawLatex( 0 ,  28*1.2*i , arg_->GetFileName(i).c_str() );
      string text = "  - " + arg_->GetFileName(i);
      tex->DrawLatex( 0.0 ,  1.0 - position , text.c_str() );
      position += tex->GetTextSize() * 1.05;
   }

  // tree
  position += tex->GetTextSize() * 1.05;
  tex->SetTextSizePixels( 36 );
  tex->SetTextColor( kBlack );
  position += tex->GetTextSize() * 1.05;
  tex->DrawLatex( 0.0 , 1.0 - position , ((string)"Tree: "+vtr_[0]->GetName() ).c_str() );

  position += tex->GetTextSize() * 1.05;


  // cut
  if( vcut_[0] != "" )
    {
      tex->DrawLatexNDC( 0.0 , 1.0 - position , "Cut: " );
      for( unsigned int i=0; i < vcut_.size(); i++ )
	{
	  
	  string text = "  - " + vcut_[i];
	  tex->DrawLatex( 0.0 , 1.0 - position , text.c_str() );
      position += tex->GetTextSize() * 1.05;
      
	}
    }
  else 
    {
      tex->DrawLatexNDC( 0.0 , 1.0 - position , "Cut: Nothing" );
    }

  //  c_->Print( save_.c_str(), "Title: Info" );
  c_->Print( save_.c_str() );

}


void Drawer::Draw()
{

  cout << "cut:" << endl;
  ShowVC( vcut_ );
  
  for( int i=0; i<branch_num_; i++ )
    {
      
      cout << setw( 25 ) << left << vbranch_name_[i] ;
      if( i%5 == 2 ) 
	cout << endl;

      vector < TH1D* > vhist;
      GetVectorHist( vbranch_name_[i], vhist );

      if( arg_->IsBoth() )
	{
	  c_->Divide( 2, 1 );

	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  false );
	  DrawPad( c_->cd(2), vhist, vbranch_name_[i],  true );
	}
      else if( arg_->IsRatio() )
	{
	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  true );
	}
      else
	{
	  DrawPad( c_->cd(1), vhist, vbranch_name_[i],  false );
	}

      string page_title = "Title: " + Replace( vbranch_name_[i], "tex", "txe" );
      c_->Print( save_.c_str(), page_title.c_str() );

      if(0)
	if( vbranch_name_[i] == "Hit_FI01X1__" )
	  c_->WaitPrimitive();

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
      //      for( unsigned int i=vtr_.size()-1; i<vtr_.size(); i-- )
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

  stringstream ss, ss_title;
  ss << branch_name << ">>htemp" 
     << "(" 
     << bin << ", " 
     << xmin << ", " 
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

  TH1D* hist = (TH1D*)gDirectory->Get( "htemp" );

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
