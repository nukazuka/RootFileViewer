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
      stringstream ss(line);

      string tag, words;
      ss >> tag >> words;

      // skip a line which begins with #
      if( tag.substr(0,1) == "#" )
	continue;
      
      if( tag == "" && words == "" )
	continue;
      
      RegisterSetting( tag , words );
    }

  return true;  
}

void ConfigHandler::RegisterSetting( string tag , string words )
{
  if( tag == "data" ) // vector
    vdata_path_.push_back( words );
  else if( tag == "cut" ) // vector
    vcut_.push_back( words );
  else if( tag == "branch" ) // vector
    vbranch_name_.push_back( words );
  else if( tag == "output" )
    output_path_ = words;
  else if( tag == "tree_name" )
    tree_name_   = words;
  else if( tag == "mode" )
    mode_        = words;
  else if( tag == "draw-option" )
    draw_option_ = words;
  else if( tag == "norm" )
    bl_norm_     = Convert2Bool( words );
  else if( tag == "logx" )
    bl_logx_     = Convert2Bool( words );
  else if( tag == "logy" )
    bl_logy_     = Convert2Bool( words );
  else if( tag == "logz" )
    bl_logz_     = Convert2Bool( words );
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
void ConfigHandler::GiveConfigTemplate( string path )
{
  ofstream ofs( path.c_str() );

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

  // template starts from here
  ofs << "data"       << "\t" << "path_to_data.root"      << "\n";
  ofs << "output"     << "\t" << "path_to_output.pdf"     << "\n";
  ofs << "tree_name"  << "\t" << "name_of_tree"           << "\n";
  ofs << "branch_name"<< "\t" << "name_of_branch1"        << "\n";
  ofs << "branch_name"<< "\t" << "name_of_branch2"        << "\n";

  ofs << "# modes are single, ratio, both and test"       << "\n";
  ofs << "mode"       << "\t" << "mode_of_draw"           << "\n";
  ofs << "cut "       << "\t" << "cut1"                   << "\n";
  ofs << "cut "       << "\t" << "cut2"                   << "\n";
  ofs << "draw-option"<< "\t" << "option"                 << "\n";
    
  ofs << "# flags"                                        << "\n";
  ofs << "norm "      << "\t" << "false"                  << "\n";
  ofs << "logx "      << "\t" << "false"                  << "\n";
  ofs << "logy "      << "\t" << "false"                  << "\n";
  ofs << "logz "      << "\t" << "false"                  << "\n";
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
  
  cout << setw( indent ) << "output file"         << " : " << output_path_ << endl;
  cout << setw( indent ) << "name of tree"        << " : " << tree_name_ << endl;
  cout << setw( indent ) << "mode"                << " : " << mode_ << endl;
  cout << setw( indent ) << "Nomalization"        << " : " << bl_norm_ << endl;
  cout << setw( indent ) << "log scale in x axis" << " : " << bl_logx_ << endl;
  cout << setw( indent ) << "log scale in y axis" << " : " << bl_logy_ << endl;
  cout << setw( indent ) << "log scale in z axis" << " : " << bl_logz_ << endl;
    
  cout << line << endl;  
}
