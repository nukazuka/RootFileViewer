#include "ConfigHandler.hh"

using namespace std;
ConfigHandler::ConfigHandler( string config_path )
{
  Init( config_path );  
}

///////////////////////////////////////////////////////////////
// private functions                                         //
///////////////////////////////////////////////////////////////

// initialization
void ConfigHandler::Init( string config_path )
{

  config_path_ = config_path;

  bool bl_success = ReadConfigFile();

  if( bl_success == false )
    exit(-1);

}

bool ConfigHandler::ReadConfigFile()
{

  ifstream ifs( config_path_.c_str() );
  if( ifs.fail() )
    {
      cerr << "void ConfigFile::ReadConfigFile()" << endl;
      cerr << config_path_ << " is not found." << endl;
      cerr << "Program is stopped" << endl;
      return false;
    }


  string line;
  while( !ifs.eof() )
    {

      getline( ifs, line );
      if( line == "" )
	continue;

      stringstream ss(line);

      vector < string > vword;
      while( !ss.eof() )
	{
	  string stemp;
	  ss >> stemp;
	  vword.push_back( stemp );
	}
      if( vword.size() == 0 )
	continue;
      
      // skip a line which begins with #
      if( vword[0].substr(0,1) == "#" )
	continue;

      string tag = vword[0];
      vword.erase( vword.begin() );
      
      RegisterSetting( tag, vword );
    }

  // remove specified branch name
  for( int i=0; i<vbranch_except_.size(); i++ )
    vbranch_name_.erase( remove( vbranch_name_.begin() ,vbranch_name_.end() , vbranch_except_[i]) , vbranch_name_.end() );
  
  return true;  
}

void ConfigHandler::RegisterSetting( string tag, vector < string > vword  )
{

  if( vword.size() == 0 )
    vword.push_back( "NULL" );
  
  if( tag == "data" ) // vector
    vdata_path_.push_back( vword[0] );
  else if( tag == "cut" ) // vector
    vcut_.push_back( vword[0] );
  else if( tag == "branch" ) // vector
    {
      
      vbranch_name_.push_back( vword[0] );
      
      BranchConfig* bc = new BranchConfig( vword ); // size 4 assumed
      vbranch_config_.push_back( bc );
    }
  else if( tag == "branch_except" ) // vector
    vbranch_except_.push_back( vword[0] );
  else if( tag == "output" )
    output_path_ = vword[0];
  else if( tag == "tree_name" )
    tree_name_   = vword[0];
  else if( tag == "mode" )
    mode_        = vword[0];
  else if( tag == "draw-option" )
    draw_option_ = vword[0];
  else if( tag == "norm" )
    bl_norm_     = Convert2Bool( vword[0] );
  else if( tag == "logx" )
    bl_logx_     = Convert2Bool( vword[0] );
  else if( tag == "logy" )
    bl_logy_     = Convert2Bool( vword[0] );
  else if( tag == "logz" )
    bl_logz_     = Convert2Bool( vword[0] );
}

bool ConfigHandler::Convert2Bool( string words )
{
  if( words == "true" || words == "True" || words == "TRUE" )
    return true;
  else if( words == "false" || words == "False" || words == "FLASE" )
    return false;
  else if( words == "0" )
    return false;

  return true;
}

///////////////////////////////////////////////////////////////
// public functions                                         //
///////////////////////////////////////////////////////////////
void ConfigHandler::GiveConfigTemplate( string data_path , string tr_name )
{
  ofstream ofs( "template.config" );

  // title
  ofs << "#############################################"  << "\n";
  ofs << "##  template of config file for rfv        ##"  << "\n";
  ofs << "#############################################"  << "\n";

  // short introduction
  ofs << "# a line begins with \"#\" is ignored         #"<< "\n";
  ofs << "# available tags are:                       #"  << "\n";
  ofs << "# - data        - output      - tree_name   #"  << "\n";
  ofs << "# - mode        - cut         - norm        #"  << "\n";
  ofs << "# - logx        - logy        - logz        #"  << "\n";
  //  ofs << "#" << "\n";
  ofs << "#############################################"  << "\n\n";

  ofs << "output"     << "\t" << "path_to_output.pdf"     << "\n";
  ofs << "# branch_except nothing"                                  << "\n";
  
  // template starts from here
  if( data_path != "" && tr_name != "" )
    ofs << GiveConfigTemplate_Tree( data_path , tr_name ) << "\n";
  else
    {

      ofs << "data"       << "\t" << "path_to_data.root"      << "\n";
      ofs << "tree_name"  << "\t" << "name_of_tree"           << "\n";
      ofs << "branch"     << "\t"
	  << "name\tbin-num\txmin\txmax"                      << "\n";
    }
  

  ofs << "# modes are single, ratio, both and test"       << "\n";
  ofs << "mode"       << "\t" << "both"                   << "\n";
  ofs << "# cut "       << "\t" << "cut"                    << "\n";
  ofs << "draw-option"<< "\t" << "HIST"                   << "\n";
    
  ofs << "# flags"                                        << "\n";
  ofs << "norm "      << "\t" << "true"                   << "\n";
  ofs << "logx "      << "\t" << "false"                  << "\n";
  ofs << "logy "      << "\t" << "true"                   << "\n";
  ofs << "logz "      << "\t" << "false"                  << "\n";

  ofs << "#############################################"  << "\n\n";  
  ofs.close();

  cout << "  - template of config : template.config was made." << endl;
}

string ConfigHandler::GiveConfigTemplate_Tree( string data_path , string tr_name )
{

  stringstream ss;
  ss << "data"       << "\t" << data_path     << "\n";
  ss << "tree_name"  << "\t" << tr_name       << "\n";

  TFile* tf = new TFile( data_path.c_str() , "READ" );
  TTree* tr = (TTree*)tf->Get( tr_name.c_str() );

  vector < TTree* > vtr(1);
  vtr[0] = tr;

  vector < string > vname = GetName( vtr );

  ss << "\n##########################################\n";
  for( int i=0; i<vname.size(); i++ )
    ss << "branch" << "\t" << vname[i] << "\n";

  return ss.str();
}

void ConfigHandler::Print()
{

  int indent = 20;
  string line( 50 , '=' );
  
  cout << line << endl;
  cout << "    ConfigHandler" << endl;
  cout << line << endl;
  
  cout << setw( indent ) << "config file"         << " : " << config_path_ << endl;

  cout << setw( indent ) << "data files"          << " : " << endl;
  for( int i=0; i<vdata_path_.size() ; i++ )
    cout << setw( indent ) << " " << " : " << vdata_path_[i] << endl;
  
  cout << setw( indent ) << "cuts"          << " : " << endl;
  for( int i=0; i<vcut_.size(); i++ )
    cout << setw( indent ) << " " << " : " << vcut_[i] << endl;

  if( vbranch_name_.size() != 0 )
    {
      cout << setw( indent ) << "Branch"        << " : " << endl;
      for( int i=0; i<vbranch_config_.size(); i++ )
	vbranch_config_[i]->Print();
    }
  
  cout << setw( indent ) << "output file"         << " : " << output_path_ << endl;
  cout << setw( indent ) << "name of tree"        << " : " << tree_name_ << endl;
  cout << setw( indent ) << "mode"                << " : " << mode_ << endl;
  cout << setw( indent ) << "Nomalization"        << " : " << bl_norm_ << endl;
  cout << setw( indent ) << "log scale in x axis" << " : " << bl_logx_ << endl;
  cout << setw( indent ) << "log scale in y axis" << " : " << bl_logy_ << endl;
  cout << setw( indent ) << "log scale in z axis" << " : " << bl_logz_ << endl;
    
  cout << line << endl;  
}
