#include "Drawer.hh"

using namespace std;

Drawer::Drawer( Argument* arg )
{

  arg_ = arg;
  Init();
  
  c_->Print( (save_+"[").c_str() );
  //  DrawInfo(); // to be fixed later
  Draw();

  c_->Print( (save_+"]").c_str() );
  
}

// private function

void Drawer::Init()
{

  if( arg_->IsConfigure() == true )
    config_ = arg_->GetConfigHandler();

  // making canvas
  if( arg_->GetMode() == "both" )
    c_ = GetCanvas( "canvas", "comparison", false);
  else
    c_ = GetCanvas( "canvas", "square", false );

  save_ = arg_->GetSaveName();
  vtr_  = arg_->GetVectorTree();
  vcut_ = arg_->GetVectorCut();

  //  GetName( vtr_, vbranch_name_ );
  if( arg_->GetConfigHandler()->GetBranchName().size() == 0 )
    {
      vbranch_name_ = GetName( vtr_ );
      
      vector < string > vbranch_except;
      // if some branches should be ignored
      if( arg_->GetConfigHandler()->GetBranchExcept().size() != 0 )
	{

	  vbranch_except = arg_->GetConfigHandler()->GetBranchExcept();
	  
	  for( int i=0; i<vbranch_except.size(); i++ )
	    vbranch_name_.erase( remove( vbranch_name_.begin() ,vbranch_name_.end() , vbranch_except[i]) , vbranch_name_.end() );

	}
      
    }
  else
    {
      
      vbranch_name_ = arg_->GetConfigHandler()->GetBranchName();
      vbranch_config_ = arg_->GetConfigHandler()->GetBranchConfig();
    }

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

  c_->Print( save_.c_str(), "Title: Info" );
  //  c_->Print( save_.c_str() );

}

void Drawer::Draw()
{

  string carrige_return = "\r";
  if( arg_->IsNoOverwrite() )
    carrige_return = "\n";

  int loop_num = branch_num_;
  if( arg_->GetMode() == "test" )
    loop_num = 1;
  
  for( int i=0; i<loop_num; i++ )
    {

      string bar = GetRepeatedWords( "=" , 2 * 10 * i / branch_num_ ) + ">";
      cout << flush << carrige_return
	   << " ["
	   << setw(20) << left << bar
	   << "] "
	   << setw(5) << setprecision(4) << right 
	   << 1.*i / branch_num_ * 100 << "%"
	   << "\t" << vbranch_name_[i] << "                 ";

      vector < TH1D* > vhist;

      BranchConfig* branch_config = new BranchConfig();
      if( arg_->GetConfigHandler()->GetBranchConfig().size() != 0 )
	branch_config = arg_->GetConfigHandler()->GetBranchConfig()[i];
      
      if( branch_config->bin_ == -1 )
	branch_config = MakeBranchConfig( vbranch_name_[i] );

      vhist = GetVectorHist( branch_config );
      
      if( arg_->IsBoth() )
	{
	  c_ = GetCanvas( "canvas", "comparison", false);
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

  cout << endl;
  cout << "\r" << " [" << GetRepeatedWords("=", 20) << "] 100% Finished !\n" << endl;
}

void Drawer::DrawPad( TVirtualPad* pad , vector < TH1D* >& vhist, string branch_name, bool ratio )
{

  PadSetting();

  TGaxis::SetMaxDigits(3);

  MultiHist* mh = new MultiHist( branch_name , branch_name, vhist );
  mh->SetMargins( 0 );
  mh->SetMarginTop( 0.15 );
  mh->SetMarginLeft( 0.15 );
  mh->SetMarginRight( 0.15 );
  mh->SetDrawNoEntry( true );
  mh->SetStatsBoxPoint( 0.9 , 0.85 );
  mh->SetStatsFormat( 1111 );

  if( ratio == false )
    {

      gPad->SetLogx( arg_->IsLogx() );
      gPad->SetLogy( arg_->IsLogy() );

      string option = arg_->GetConfigHandler()->GetDrawOption();
      mh->Draw( option , 0.90, 0.9 - 0.1*vtr_.size() , 1.0, 0.9 );

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

      MultiHist* mh_ratio = new MultiHist( "mh_ratio", "ratio" );
      mh_ratio->SetRatioMode( true );
      mh_ratio->SetIncludeErrorBar( true );
      mh_ratio->SetDrawNoEntry( true );
      mh_ratio->SetYmax( 2.0 );
      mh_ratio->SetYmin( 0.0 );
      mh_ratio->SetRatioNormalize( true );
      mh_ratio->SetLabelSizeX( 0.13 );
      mh_ratio->SetLabelSizeY( 0.08 );
      
      mh_ratio->AddBaseHist( vhist[0] );
      
      string option = arg_->GetDrawRatioOption();

      for( unsigned int i=1; i<vtr_.size(); i++ )
	mh_ratio->Add( vhist[i] );
      
      // trick to get same frame, should be improved
      for( unsigned int i=0; i<vtr_.size(); i++ )
	mh->Add( vhist[i] );
      mh->DrawFrame();

      mh_ratio->SetXmin( mh->GetXmin() );
      mh_ratio->SetXmax( mh->GetXmax() );
      mh_ratio->Draw( option );
    }

  DrawTitle( gPad, 0.07 );

  gPad->Update();
}

void Drawer::GetVectorHist( string branch_name, vector < TH1D* >& vhist )
{

  double xmin = 0.0, xmax = 1.0;
  GetRange( vtr_, branch_name, vcut_, xmin, xmax);
  
  // set a number of bin
  //  int bin = xmax - xmin; // not good
  int bin = sqrt( vtr_[0]->GetEntries( vcut_[0].c_str() ) ) * arg_->GetBinFactor();
  
  // if #min. bin is specified, check it
  if( arg_->IsMinBin() == true )
    {
      if( bin < arg_->GetMinBin() )
	bin = arg_->GetMinBin();
    }
  else
    {
      if( bin <= 1 )
	bin = 100;
      
      while( bin > 500 )
	bin = bin/10;
    }

  if( branch_name.find("Hit_DC") != string::npos )
    bin = bin/2;

  // if this branch is boolian, change #bin, xmin and xmax
  string branch_name_corrected
    = branch_name.substr( 0 , branch_name.find( "[" ) );

  TLeaf* leaf = vtr_[0]->GetLeaf( branch_name_corrected.c_str() );

  if( leaf != nullptr )
    if( (string)leaf->GetTypeName() == "Bool_t" )
      {
	bin  = 2;
	xmin = 0;
	xmax =2;
	
      }
  
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

vector < TH1D* > Drawer::GetVectorHist( BranchConfig* branch_config )
{

  vector < TH1D* > vhist;
  for( unsigned int i=0; i<vtr_.size(); i++ )
    {

      vhist.push_back( GetHist( i,
				arg_->GetFileName(i),
				vtr_[i], 
				branch_config->name_, 
				vcut_[i],
				branch_config->bin_, 
				branch_config->xmin_, 
				branch_config->xmax_
				)
		       );

      int color =  config_->GetColor()[i];
      if( color != -1 )
	HistSetting( vhist[i] , GetColor( color ), 2, 20, 1, 62 );
      else
	HistSetting( vhist[i] , GetColor( i ), 2, 20, 1, 62 );
	      
    }
  return vhist;
}

TH1D* Drawer::GetHist( int num, string file_name, TTree* tr, string branch_name, string cut, int bin, double xmin, double xmax )
{

  // this hist naming cause warning since same name will be used for all branches
  // for display of stats box, I don't fix it
  string name = GetBaseName(file_name);
  //  string name = GetBaseName(file_name) + branch_name + Int2String( num );
  string title = branch_name;

  stringstream ss, ss_title;
  TH1D* hist = new TH1D( name.c_str() , title.c_str(), bin, xmin, xmax );
  ss << branch_name << ">>" << name;
  
  int entry = tr->Draw( ss.str().c_str(), cut.c_str(), "goff" );
  hist->Fill( true );
  
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

  SetHist( hist, num );

  if( arg_->IsNorm() )
    {
      hist->Scale( arg_->GetNormVal() / hist->Integral() );
    }

  return hist;
}

BranchConfig* Drawer::MakeBranchConfig( string branch_name )
{

  double xmin = 0.0, xmax = 1.0;
  GetRange( vtr_, branch_name, vcut_, xmin, xmax);

  // set a number of bin
  //  int bin = xmax - xmin; // not good
  int bin = sqrt( vtr_[0]->GetEntries( vcut_[0].c_str() ) ) * arg_->GetBinFactor();
  
  // if this branch is boolian, change #bin, xmin and xmax
  string branch_name_corrected
    = branch_name.substr( 0 , branch_name.find( "[" ) );

  TLeaf* leaf = vtr_[0]->GetLeaf( branch_name_corrected.c_str() );

  if( leaf != nullptr )
    if( (string)leaf->GetTypeName() == "Bool_t" )
      {
	bin  = 2;
	xmin = 0;
	xmax =2;
	
      }
  return new BranchConfig( branch_name , bin, xmin, xmax );
}

void Drawer::SetHist( TH1D* hist, int num )
{

  hist->Sumw2();
  hist->SetLineWidth( 2 );

  if( num < 10 )
    {
      //      hist->SetLineColor( kColor_[num] );
      //      hist->SetMarkerColor( kColor_[num] );
      HistSetting( hist , GetColor(num) , 2 );
    }
  else
    {
      hist->SetLineColor( kBlack );
      hist->SetMarkerColor( kBlack );
    }


}
