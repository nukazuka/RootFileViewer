#include "Argument.hh"
using namespace std;

// constructor
Argument::Argument(int argc, char* argv[] )
{

  parser_ = new ArgumentParser( argc, argv );

  if( parser_->IsSpecified( "option" ) )
    Option();

  if( parser_->IsSpecified( "config-template" ) )
    {

      string data_path = parser_->GetArgument( "config-template" );
      string tr_name   = parser_->GetArgument( "tree" );

      if( data_path != "NULL" && tr_name != "NULL" )
	{
	  ConfigHandler::GiveConfigTemplate( data_path , tr_name );
	}
      else
	{
	  cerr << "  config option ON but data path and/or tree name are not given" << endl;
	  ConfigHandler::GiveConfigTemplate();
	}

      exit(1);
    }

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

  if( parser_->IsSpecified( "config" ) == true )
    {
      InitWithConfigFile();
      return;
    }

  data_ = parser_->GetArgument( "data" );
  
  ExtractFileName();

  // get flag
  bl_logx_        = parser_->IsSpecified( "logx" );
  bl_logy_        = parser_->IsSpecified( "logy" );
  bl_both_        = parser_->IsSpecified( "both" );
  bl_ratio_       = parser_->IsSpecified( "ratio" );
  bl_wait_        = parser_->IsSpecified( "wait" );
  bl_tree_        = parser_->IsSpecified( "tree" );
  //  bl_tree_name_   = parser_->IsSpecified( "tree-name" );
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
  //  bl_save_        = parser_->IsSpecified( "save" );
  bl_bin_factor_= parser_->IsSpecified( "bin-factor" );

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

  //  if( bl_tree_ )
  //    specified_tree_name_ = parser_->GetArgument( "tree" );

  
  if( bl_branch_ )
    specified_branch_name_ = parser_->GetArgument( "branch" );

  if( parser_->IsSpecified( "norm-val" ) )
    norm_val_ = String2Double( parser_->GetArgument( "norm-val" ) );
  
  if( parser_->IsSpecified( "min-bin" ) )
    min_bin_num_ = String2Int( parser_->GetArgument( "min-bin" ) );

  if( parser_->IsSpecified( "save" ) )
    save_ = parser_->GetArgument( "save" );
  else
    save_ = "none";

  if( parser_->IsSpecified( "bin-factor" ) )
    bin_factor_ = String2Double( parser_->GetArgument( "bin-factor" ) );
  

  //  bl_test_        = parser_->IsSpecified( "test" );
  //  bl_debug_       = parser_->IsSpecified( "debug" );
  //  bl_in_tra_      = parser_->IsSpecified( "in-tra-mode" );

  ExtractCut();

  // make a vector of FileManager
  for( unsigned int i=0; i<vfile_.size(); i++ )
    vfm_.push_back( new FileManager( vfile_[i] , !IsTree() ) );

  IsTree() ? ExtractTreeID() : AskTreeID();

  cout << tree_id_ << endl;
  cout << vfm_[0]->GetTree( tree_id_ )->GetName() << " is used." << endl;

  if( save_ == "none" )
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

void Argument::InitWithConfigFile()
{

  config_path_ = parser_->GetArgument( "config" );  

  config_ = new ConfigHandler( config_path_ );

  vfile_		= config_->GetData();
  vcut_			= config_->GetCuts();
  bl_norm_		= config_->IsNorm();
  bl_logx_		= config_->IsLogx();
  bl_logy_		= config_->IsLogy();
  //  bl_logz_		= config_->IsLogz();
  tree_name_		= config_->GetTreeName();
  specified_tree_name_	= tree_name_;

  save_ = config_->GetOutputPath();
  if( save_ == "" )
    save_ = ExtractSaveName();
  
  mode_ = config_->GetMode();
  if( mode_ == "ratio" )
    bl_ratio_ = true;
  else if( mode_ == "both" )
    bl_both_ = true;

  // make a vector of FileManager
  for( unsigned int i=0; i<vfile_.size(); i++ )
    vfm_.push_back( new FileManager( vfile_[i] , !IsTree() ) );

  //  IsTree() ? ExtractTreeID() : AskTreeID();
  tree_name_  != "" ? ExtractTreeID() : AskTreeID();
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

      cout << cut << " ";
      cut = Replace( cut , "\\!" , "!" );
      cout << "\t" << cut << endl;
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

  // if specified tree name is number,
  // the number is applied to all TTree
  if( IsNumber( specified_tree_name_ ) )
    {
      // get id
      int id = String2Int( specified_tree_name_ );

      // if id is valid, store it
      if( id > -1 && id < vfm_[0]->GetTreeNum() )
	for( int i=0; i<vfile_.size(); i++ )
	  vtree_id_.push_back( id );

      return;
    }
  // if specified tree name is not number,
  // id number of the tree in each file is searched and stored
  else
    {
      for( int i=0; i<vfile_.size(); i++ )
	{
	  for( int j=0; j<vfm_[i]->GetTreeNum(); j++ )
	    {
	      if( specified_tree_name_ == (string)(vfm_[i]->GetTree(j)->GetName()) )
		{
		  vtree_id_.push_back(j);
		  break;
		}
	    }
	}
    }

  // if no id was found, stop!
  if( vtree_id_.size() == 0 )
    {
      cerr << " void Argument::ExtractTreeID()" << endl;
      cerr << " a number of tree id found is 0" << endl;
      cerr << " check whether tree name is correct or not" << endl;
      exit( -1 );
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
    cut_info = Replace( cut_ , "\\!" , "NOT_" );

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

  if( rtn.size() > 110 )
    //    rtn = dir + "TooLong-" + rtn.substr( dir.size(), 100 ) + ".pdf";
    rtn = dir + "TooLong-" + cut_info.substr( 0, 50)  + ".pdf";

  return rtn;
}

void Argument::AskTreeID()
{

  cout << "Which tree do you want to draw?" << endl;
  ShowTreeInfo();
  cout << "--------------" << endl;
  
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

void Argument::ShowTreeInfo()
{

  bool bl_same_structure = true;
  // loop over FileManager
  for( int i=0; i<vfm_.size()-1; i++ )
    {
      if( vfm_[i]->IsSameTreeStructure( vfm_[i+1] ) == false )
	{
	  bl_same_structure = false;
	  break;
	}
    }

  if( bl_same_structure == true )
    {
      vfm_[0]->ShowTree();
    }
  else
    {
      for( int i=0; i<vfm_.size(); i++ )
	{
	  //	  string stemp( "-" , vfm_[i]->GetName().size() );
	  string stemp( vfm_[i]->GetName().size() , '-');
	  cout << stemp << endl;
	  cout << vfm_[i]->GetName() << endl;
	  vfm_[i]->ShowTree();
	}
    }
  
  //  vector < string > vsame_name = FileManager::ExtractSameTreeName( vfm_ );

  vector < string > vsame_name;// = vfm_[0]->ExtractSameTreeName( vfm_[1] );
  /*
  cout << "number of same name = " << vsame_name.size() << endl;
  //  cout << vsame_name[0] << endl;
  for( int i=0; vsame_name.size(); i++ )
    cout << i << ":" << vsame_name[i] << endl;
  */

  for( int i=0; i<vfm_.size(); i++ )
    for( int j=0; j<vfm_[i]->GetTreeNum(); j++ )
      vsame_name.push_back( vfm_[i]->GetTree(j)->GetName() );
   
}

// public functions

vector < TTree* > Argument::GetVectorTree()
{
  vector < TTree* > vtr;
  for( unsigned int i=0; i<vfm_.size(); i++ )
    vtr.push_back( vfm_[i]->GetTree( vtree_id_[i] ) );

  return vtr;
}

vector < string > Argument::GetVectorCut()
{

  if( vcut_.size() == 0 )
    return vector < string > ( vfile_.size() , "" );

  if( config_->IsEachCut() == false )
    {

      string cut_common = vcut_[0];
      for( int i=1; i<vcut_.size(); i++ )
	cut_common += " && " + vcut_[i];

      return vector < string > ( vfile_.size() , cut_common );
    }

  // each cut mode should be implemented ... ...   
  // dummy vector and rtn
  vector < string > vrtn;
  return vrtn;
}



string Argument::GetFileName( int num )
{

  if( num < 0 || num >= (int)vfile_.size() )
    return "unkwn";

  return vfm_[num]->GetName();
}

void Argument::ShowStatus()
{

  cout << "  - Data File: ";
  for( unsigned int i=0; i<vfm_.size(); i++ )
    cout << vfm_[i]->GetName() << "\t";
  cout << endl;
  
  cout << "  - Save file: "
       << GetSaveName() << endl;
  cout << "  - TTree : "
       << vfm_[0]->GetTree( vtree_id_[0] )->GetName() << " is used." << endl;
  
  // vector < string > vbranch_name = config_->GetBranchName();
  // if( vbranch_name.size() > 0 )
  //   {
  //     cout << "  - name of branch to be drawn : " << endl;
  //     for( int i=0; i<vbranch_name.size(); i++ )
  // 	{
  // 	  cout << "    "
  // 	       << setw(20) << vbranch_name[i]
  // 	       << " ";

  // 	  if( i%2 == 1 )
  // 	    cout << endl;
  // 	}
  //     cout << endl;
  //   }

  config_->Print();
  
  
  //  cout << GetRepeatedWords( "*", 10 ) << endl;
  //  cout << GetRepeatedWords( "*", 10 ) << endl;
}



void Argument::Option()
{

  int length = 80;
  int indent_num = 25;
  string indent( indent_num , ' ' );
  cout << endl;
  cout << "--OPTIONS"
       << GetRepeatedWords( "-", length-8 ) << endl;

  // *** CONFIGURE ******************************************************
  cout << "|" << setw( indent_num ) << " --config : "
       << "You can specify configuration file" << endl;
  cout << "|" << setw( indent_num ) << " --config-template : "
       << "A template of configuration \"template.config\" is " << endl;
  cout << "|" << setw( indent_num ) << " : " << "output and program is exited" << endl;
  cout << "|" << setw( indent_num ) << " : " << "Root file to be drawn is optional." << endl;
  cout << "|" << setw( indent_num ) << " : " << "Name of TTree to be drawn need to be given with --tree option" << endl;

  // *** DATA SELECTION ******************************************************
  cout << "|" << setw( indent_num ) << " --data : "
       << "You can specify data like following:" << endl;
  cout << "|" << indent
       << " --data=\"MC.root|data/MC2.root\"" << endl;
  cout << "|" << indent
       << "or" << endl;
  cout << "|" << indent
       << " --data=\"MC.root\" --data=\"data/MC2.root\"" << endl;
  cout << "|" << indent
       << "where \"|\" is used as a separator." << endl;

  // *** TREE(BRANCH) SELECTION ******************************************************
  cout << "|" 
       << "=== data selection " << GetRepeatedWords("=", length-19 ) << endl;

  cout << "|" << setw( indent_num ) << " --force : "
       << "Ignore differences of structure of Root files" << endl;

  cout << "|" << setw( indent_num ) << " --tree : "
       << "you can specify the tree which will be drawn like following:" << endl;
  cout << "|" << indent
       << "  --tree=\"MC_event\"" << endl;
  cout << "|" << indent
       << "  --tree=1" << endl;
  cout << "|" << indent
       << "If nothing is specifed, this program will ask." << endl;
  cout << "|" << indent
       << "When you give a name of tree, correct trees are chosen" << endl;
  cout << "|" << indent
       << "even if structure of TFiles are not same." << endl;
  
  cout << "|" << setw( indent_num ) << " --except-branch : "
       << "not ready" << endl;

  // *** DRAW SETTING, CUT ******************************************************
  cout << "|" 
       << "=== drawing setting " << GetRepeatedWords("=", length-20 ) << endl;

  cout << "|" << setw( indent_num ) << " --ratio : " 
       << "Ratios of histogram (hist[i]/hist[0]) are drawn" << endl;
  cout << "|" << setw( indent_num ) << " --both : " 
       << "Both of normal histograms and rarios of histograms are drawn" << endl;
  cout << "|" << setw( indent_num ) << " --cut : "
       << "cut is applied." << endl;
  cout << "|" << setw( indent_num ) << " --rand-cut : "
       << "not ready" << endl;

  cout << "|" << setw( indent_num ) << " --norm : " 
       << "Histgrams will be normalized." << endl;
  cout << "|" << indent
       << "To specify a value of nomarization, use --norm-val" << endl;
  cout << "|" << setw( indent_num ) << " --norm-val : " 
       << "Specify a value of normalization" << endl;


  cout << "|" << setw( indent_num ) << " --draw : " 
       << "not ready" << endl;

  cout << "|" << setw( indent_num ) << " --draw-ratio : " 
       << "not ready" << endl;

  cout << "|" << setw( indent_num ) << " --min-bin : "
       << "a number bins at minimum." << endl;
  cout << "|" << setw( indent_num ) << " --bin-factor : " 
       << "factor of bin number" << endl;

  cout << "|" << setw( indent_num ) << " --logx : " 
       << "X axis is set log scale" << endl;
  cout << "|" << setw( indent_num ) << " --logy : "
       << "Y axis is set log scale" << endl;
  cout << "|" << setw( indent_num ) << " --xaxis-time : "
       << "not ready" << endl;

  // *** SPECIAL ******************************************************

  cout << "|" 
       << "=== special " << GetRepeatedWords("=", length-12 ) << endl;

  cout << "|" << setw( indent_num ) << " --s-cut-DY2014 : " 
       << "A random trigger cut is applied to only real data." << endl;


  // *** MISC ******************************************************

  cout << "|" 
       << "=== misc " << GetRepeatedWords("=", length-9 ) << endl;

  cout << "|" << setw( indent_num ) << " --save : " 
       << "Specify name ( & path ) of output file. Users need to take responsibillty of path and suffix." << endl;
  cout << "|" << setw( indent_num ) << " --out-dir : " 
       << "Specify the directory which output file will be put in." << endl;
  cout << "|" << setw( indent_num ) << " --option : " 
       << "A list of options are drawn." << endl;
  cout << "|" << setw( indent_num ) << " --no-overwrite : " 
       << "Progress bar is not overwritten" << endl;

  /*
  cout << "|" << setw( indent_num )
       << " --data" << " : Specify data file like following" << endl;
  cout << "|" << setw( indent_num  )
       << "       " << "   --data=\"RD_all|MC_ver5_nocut\"" << endl;
  cout << "|" << setw( indent_num  )
       << "       " << "   Use \"show-data\" option to check available data" << endl;
  cout << "|" << setw( indent_num  )
       << "       " << "   You can omit directory \"data/\" , suffix \".root\" and keyword \"_UE402\"." << endl;
  */

  cout << GetRepeatedWords( "-", length) << endl;
  cout << endl;

  exit(1);
}
