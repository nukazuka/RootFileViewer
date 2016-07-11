#include "Argument.hh"
using namespace std;

// constructor
Argument::Argument(int argc, char* argv[] )
{

  parser_ = new ArgumentParser( argc, argv );

  if( parser_->IsSpecified( "option" ) )
    Option();

  Init();
  if( vfile_.size() == 0 )
    {
      cerr << "No data file " << endl;
      cerr << "Program is stopped " << endl;
      exit(-1) ;
    }

  ShowStatus();
}

// private 
void Argument::Init()
{

  data_ = parser_->GetArgument( "data" );

  ExtractFileName();

  // get flag
  bl_logx_        = parser_->IsSpecified( "logx" );
  bl_logy_        = parser_->IsSpecified( "logy" );
  bl_both_        = parser_->IsSpecified( "both" );
  bl_ratio_       = parser_->IsSpecified( "ratio" );
  bl_wait_        = parser_->IsSpecified( "wait" );
  bl_tree_        = parser_->IsSpecified( "tree" );
  bl_branch_      = parser_->IsSpecified( "branch" );
  bl_draw_        = parser_->IsSpecified( "draw" );
  bl_draw_ratio_  = parser_->IsSpecified( "draw-ratio" );
  bl_cut_         = parser_->IsSpecified( "cut" );
  bl_norm_        = parser_->IsSpecified( "norm" );
  bl_test_        = parser_->IsSpecified( "test" );
  bl_force_       = parser_->IsSpecified( "force" );
  bl_out_dir_     = parser_->IsSpecified( "out-dir" );
  bl_min_bin_     = parser_->IsSpecified( "min-bin" );
  bl_no_overwrite_= parser_->IsSpecified( "no-overwrite" );

  // special option
  bl_s_cut_DY2014_ = parser_->IsSpecified( "s-cut-DY2014" );

  cut_ = (bl_cut_ ? parser_->GetArgument("cut") : "" ) ;
  draw_ = ( bl_draw_ ? parser_->GetArgument( "draw" ) : "HIST E" );
  draw_ratio_ = ( bl_draw_ratio_ ? parser_->GetArgument( "draw-ratio" ) :  "P E" );

  suffix_ = 
    parser_->IsSpecified( "suffix" ) ? 
    "_" + parser_->GetArgument( "suffix" ) : "";

  // get value
  if( bl_tree_ )
    specified_tree_name_ = parser_->GetArgument( "tree" );

  if( bl_branch_ )
    specified_branch_name_ = parser_->GetArgument( "branch" );

  if( parser_->IsSpecified( "norm-val" ) )
    norm_val_ = String2Double( parser_->GetArgument( "norm-val" ) );
  
  if( parser_->IsSpecified( "min-bin" ) )
    min_bin_num_ = String2Int( parser_->GetArgument( "min-bin" ) );

  //  bl_test_        = parser_->IsSpecified( "test" );
  //  bl_debug_       = parser_->IsSpecified( "debug" );
  //  bl_in_tra_      = parser_->IsSpecified( "in-tra-mode" );

  ExtractCut();

  // make a vector of FileManager
  for( unsigned int i=0; i<vfile_.size(); i++ )
    {

      FileManager* fm = new FileManager( vfile_[i] , !IsTree() );
      vfm_.push_back( fm );
    }

  IsTree() ? ExtractTreeID() : AskTreeID();

  cout << vfm_[0]->GetTree( tree_id_ )->GetName() << " is used." << endl;

  save_ = ExtractSaveName();
}

bool Argument::CheckFileStructure()
{

  for( unsigned int i=1; i<vfm_.size(); i++ )
    {
      if( vfm_[i-1]->GetFileStructure() != vfm_[i]->GetFileStructure() )
	bl_same_structure_ = false;
    }

  if( bl_same_structure_ == false && bl_force_ == false )
    {
      cerr << "structure of files are different" << endl;
      exit(-1);
    }

  return true;
}

void Argument::ExtractCut()
{

  string cut = "";
  for( unsigned int i=0; i<vfile_.size(); i++ )
    {

      if( bl_cut_ )
	cut = parser_->GetArgument( "cut" );
      else
	cut = "";

      // random trigger cut for DY2014
      // cut is apllied only real data
      bool bl_temp = ( vfile_[i].find("RD") != string::npos );
      if( bl_s_cut_DY2014_ && bl_temp )
	{

	  if( cut == "" )
	    cut = "TriggerBit[10]";
	  else
	    cut = cut + "&&TriggerBit[10]";
	}
      vcut_.push_back( cut );
    }
}

void Argument::ExtractFileName()
{

  string stemp  = data_ ;
  string stemp2 = "";

  string delimiter = "|" ;
  while( stemp.find(delimiter) != string::npos )
    {

      stemp2 = stemp.substr( 0, stemp.find_first_of(delimiter) );
      stemp = stemp.substr( stemp.find_first_of(delimiter)+1 ,
			    stemp.size() - stemp.find_first_of(delimiter) );
      vfile_.push_back( stemp2 );
    }

  if( stemp != "" )
    vfile_.push_back( stemp );
}

void Argument::ExtractTreeID()
{

  for( int i=0; i<vfm_[0]->GetTreeNum(); i++ )
    {

      if( specified_tree_name_ == vfm_[0]->GetTree(i)->GetName() )
	{
	  tree_id_ = i;
	  return;
	}
    }

  if( IsNumber( specified_tree_name_ ) )
    {

      int id = String2Int( specified_tree_name_ );
      if( id > -1 && id < vfm_[0]->GetTreeNum() )
	tree_id_ = id;

    }
}

// output name is :
// ${path}/${files}_${tree}_${setting}_${cut}.pdf
//
// where 
//    ${path} : path to the directory which output file will be put in
//    ${files}: informaton of data files.
//    ${tree}   : information of tree
//    ${setting}: information of setting
//    ${cut}    : information of cut
string Argument::ExtractSaveName()
{

  // for test mode
  if( bl_test_ )
    return "test.pdf";

  // set directory where output file is put
  string dir = "";
  if( bl_out_dir_ )
    dir = parser_->GetArgument( "out-dir" );

  if( dir != "" )
    if( dir.substr( dir.size()-1, 1 ) != "/" )
      dir += "/";

  // get a basename of file
  string file_info = "" ;
  for( unsigned int i=0; i<vfm_.size(); i++ )
    file_info += GetBaseName( vfm_[i]->GetName() ) + "-";

  file_info = file_info.substr( 0,file_info.size()-1 );

  // get a name of object
  string tree_info = (vfm_[0]->GetTree( tree_id_ ))->GetName();

  // B: both, R: ratio, N: normal mode
  string setting_info = "";
  if( bl_both_ )
    setting_info = "B";
  else if( bl_ratio_ )
    setting_info = "R";
  else
    setting_info = "N";

  // if normalized mdoe, add value 
  if( bl_norm_ )
    setting_info += "norm" + Double2String( norm_val_ );

  // log infomation 
  if( bl_logx_ && bl_logy_ )
    setting_info += "logxy";
  else if( bl_logx_ )
    setting_info += "logx";
  else if( bl_logy_ )
    setting_info += "logy";

  // cut infomation       
  string cut_info = "";
  if( bl_cut_ )
    cut_info = cut_;

  // special infomation
  if( bl_s_cut_DY2014_ )
    cut_info = "DY2014" + cut_info;

  // replace spacial charactors
  cut_info = Replace4Cut( cut_info );

  string rtn = "" ;
  if( cut_info != "" )
    rtn = dir
      + file_info + "-" 
      + tree_info + "-"
      + setting_info + "-"
      + cut_info     + ".pdf";
  else
    rtn = dir
      + file_info + "-" 
      + tree_info + "-"
      + setting_info + ".pdf";

  /*
  if( rtn.size() > 110 )
    rtn = dir + "TooLong-" + rtn.substr( dir.size(), 100 ) + ".pdf";
  */

  return rtn;
}

void Argument::AskTreeID()
{

  cout << "Which tree do you want to draw?" << endl;
  vfm_[0]->ShowTree();
  cout << -1 << " exit\n" << endl;
  
  int id = -2;
  bool bl_exit = false;
  while ( !bl_exit )
    {
      cout << "Please type the number...\n> " ;
      
      cin >> id;
      
      if( id == -1 )
	{
	  cout << "OK. Good Bye" << endl;
	  exit(-1);
	}

      if( !(id < -1 || id >= vfm_[0]->GetTreeNum() ) )
	{
	  tree_id_ = id;
	  bl_exit = true;
	}
      else
	  cout << "A number must be -1 - " << vfm_[0]->GetTreeNum()-1 << endl;

    }
}

// public functions

void Argument::GetVectorTree( vector < TTree* >& vtr_arg )
{

  for( unsigned int i=0; i<vfm_.size(); i++ )
    vtr_arg.push_back( vfm_[i]->GetTree( tree_id_ ) );
}

void Argument::GetVectorCut( vector < string >& vcut_arg )
{

  copy( vcut_.begin(), vcut_.end(), back_inserter( vcut_arg ) );
}

string Argument::GetFileName( int num )
{

  if( num < 0 || num >= (int)vfile_.size() )
    return "unkwn";

  return vfm_[num]->GetName();
}

void Argument::ShowStatus()
{

  cout << "Data File: ";
  for( unsigned int i=0; i<vfm_.size(); i++ )
    cout << vfm_[i]->GetName() << "\t";
  cout << endl;
  
  cout << "Save file: " << GetSaveName() << endl;

  //  cout << GetRepeatedWords( "*", 10 ) << endl;
  //  cout << GetRepeatedWords( "*", 10 ) << endl;
}



void Argument::Option()
{

  int length = 80;
  int indent = 20;
  cout << endl;
  cout << "--OPTIONS"
       << GetRepeatedWords( "-", length-8 ) << endl;

  // --data
  // *** DATA SELECTION ******************************************************
  cout << "|" << setw( indent ) << " --data : "
       << "You can specify data like following" << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "--data=\"MC.root|data/MC2.root\"" << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "\"|\" is used as a separator." << endl;

  // *** TREE(BRANCH) SELECTION ******************************************************
  cout << "|" 
       << "=== data selection " << GetRepeatedWords("=", length-19 ) << endl;

  cout << "|" << setw( indent ) << " --force : "
       << "Ignore differences of structure of Root files" << endl;

  cout << "|" << setw( indent ) << " --tree : "
       << "you can specify the tree which will be drawn like following:" << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "--tree=\"MC_event\"" << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "--tree=1" << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "If nothing is specifed, this program will ask." << endl;

  cout << "|" << setw( indent ) << " --except-branch : "
       << "not ready" << endl;

  // *** DRAW SETTING, CUT ******************************************************
  cout << "|" 
       << "=== drawing setting " << GetRepeatedWords("=", length-20 ) << endl;

  cout << "|" << setw( indent ) << " --ratio : " 
       << "Ratios of histogram (hist[i]/hist[0]) are drawn" << endl;
  cout << "|" << setw( indent ) << " --both : " 
       << "Both of normal histograms and rarios of histograms are drawn" << endl;
  cout << "|" << setw( indent ) << " --cut : "
       << "cut is applied." << endl;
  cout << "|" << setw( indent ) << " --rand-cut : "
       << "not ready" << endl;

  cout << "|" << setw( indent ) << " --norm : " 
       << "Histgrams will be normalized." << endl;
  cout << "|" << GetRepeatedWords( " " , indent )
       << "To specify a value of nomarization, use --norm-val" << endl;
  cout << "|" << setw( indent ) << " --norm-val : " 
       << "Specify a value of normalization" << endl;


  cout << "|" << setw( indent ) << " --draw : " 
       << "not ready" << endl;

  cout << "|" << setw( indent ) << " --draw-ratio : " 
       << "not ready" << endl;

  cout << "|" << setw( indent ) << " --logx : " 
       << "X axis is set log scale" << endl;
  cout << "|" << setw( indent ) << " --logy : "
       << "Y axis is set log scale" << endl;
  cout << "|" << setw( indent ) << " --xaxis-time : "
       << "not ready" << endl;
  cout << "|" << setw( indent ) << " --min-bin : "
       << "a number bins at minimum." << endl;

  // *** SPECIAL ******************************************************

  cout << "|" 
       << "=== special " << GetRepeatedWords("=", length-12 ) << endl;

  cout << "|" << setw( indent ) << " --s-cut-DY2014 : " 
       << "A random trigger cut is applied to only real data." << endl;


  // *** MISC ******************************************************

  cout << "|" 
       << "=== misc " << GetRepeatedWords("=", length-9 ) << endl;

  cout << "|" << setw( indent ) << " --save : " 
       << "not ready" << endl;
  cout << "|" << setw( indent ) << " --out-dir : " 
       << "Specify the directory which output file will be put in." << endl;
  cout << "|" << setw( indent ) << " --option : " 
       << "A list of options are drawn." << endl;
  cout << "|" << setw( indent ) << " --no-overwrite : " 
       << "Progress bar is not overwritten" << endl;


  /*
  cout << "|" << setw( indent )
       << " --data" << " : Specify data file like following" << endl;
  cout << "|" << setw( indent  )
       << "       " << "   --data=\"RD_all|MC_ver5_nocut\"" << endl;
  cout << "|" << setw( indent  )
       << "       " << "   Use \"show-data\" option to check available data" << endl;
  cout << "|" << setw( indent  )
       << "       " << "   You can omit directory \"data/\" , suffix \".root\" and keyword \"_UE402\"." << endl;
  */

  cout << GetRepeatedWords( "-", length) << endl;
  cout << endl;

  exit(1);
}

/*
void Argument::ShowDataFile()
{

  vector < string > vfile;
  vector < int > vlength;
  string dir_name = "data/";
  DIR* dp = opendir( dir_name.c_str() );
  if( dp == NULL)
    {
      cout << "=== ERROR ====" << endl;
      cout << "where is data directory?" << endl;
      cout << "plase go to parent directory of data" << endl;
      exit( -1 );
    }
  else
    {

      struct dirent* dent;

      do
	{

	  dent = readdir( dp );
	  if( dent != NULL )
	    {

	      string name = dent->d_name;
	      vlength.push_back( name.size() );
	      if( name.find( ".root" )  != string::npos )
		vfile.push_back( dir_name + name );
	    }
	} while(dent!=NULL);

      closedir(dp);
    }
  
  sort( vfile.begin() , vfile.end() );
  int longest = *max_element( vlength.begin(), vlength.end() );
  int width_base = longest*2 + 3*2 + 1;

  cout << GetRepeatedWords( "=" , width_base  ) << endl;

  string comment = "  A List Of Data Files";

  cout << "|" << comment << GetRepeatedWords( " " , width_base - comment.size() -2 ) << "|" << endl;
  cout << "|" << GetRepeatedWords( "-" , width_base - 2 ) << "|" <<  endl;

  for( int i=0; i<vfile.size(); i++ )
    {

      if( i%2 == 0 )
	cout << "| " ;

      cout << setw( longest ) << left << Replace( vfile[i], "data/", "" ) 
	   << " | ";

      if( i%2 == 1 )
	cout << endl;
    }

  if( vfile.size() % 2 == 1 )
    cout << GetRepeatedWords( " " , longest+1) <<  "|" << endl;

  cout << GetRepeatedWords( "=" , width_base ) << endl;
  exit(1);
}
*/
