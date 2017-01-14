#include "FileManager.hh"

using namespace std;

// constructer
FileManager::FileManager( string file_full_path_arg, bool bl_show_structure )
{

  file_full_path_ =   file_full_path_arg;
  bl_show_structure_ = bl_show_structure;
  Init();

}

// private functions
void FileManager::Init()
{

  file_path_ = GetPathToFile( file_full_path_ );
  file_name_ = GetFileName( file_full_path_ );
  file_suffix_ = GetSuffix( file_name_ );
  structure_ = "";

  cout << file_name_ << endl;
  tf_ = new TFile( file_full_path_.c_str(), "READ" );
  bl_exist_ = tf_->IsOpen();
  bl_same_tree_structure_ = false;
  
  if( bl_exist_ == false )
    {
      cerr << "void FileManager::Init()" << endl;
      cerr << file_full_path_ << " does not exist" << endl;
      exit(-1);
    }
  
  if( file_suffix_ != ".root" )
    {
      cerr << "It's not Root file" << endl;
      Init_NotRootFile();
      return;
    }

  ExtractAllTree();
}

void FileManager::Init_NotRootFile()
{
  structure_ = "";

}

void FileManager::ExtractAllTree()
{

  for( int i=0; i<tf_->GetNkeys(); i++ )
    {

      string key_name = tf_->GetListOfKeys()->At(i)->GetName();
      string class_name = tf_->Get( key_name.c_str() )->ClassName();

      bool bl_folder = false;
      if( class_name == "TDirectoryFile" )
	bl_folder = tf_->GetListOfKeys()->At(i)->IsFolder();

      if( i == 0 && bl_show_structure_ )
	cout << key_name << "\tclass:" << class_name << "\tfolder:" << bl_folder << endl;
      structure_ += key_name + "/";

      // In case of TTree
      if( class_name == "TTree" )
	{
	  vtr_.push_back( (TTree*)tf_->Get( key_name.c_str() ) );
	}
      // In case of TDirectoryFile
      else if( bl_folder )
	{
	  TDirectoryFile* tdf = (TDirectoryFile*)tf_->Get( key_name.c_str() );

	  for( int j=0; j<tdf->GetNkeys(); j++ )
	    {

	      string key_name2 = tdf->GetListOfKeys()->At(j)->GetName();
	      string class_name2 = tdf->Get( key_name2.c_str() )->ClassName();

	      if( i == 0 && bl_show_structure_ )
		cout << "  |- " << class_name2 << "\t" << key_name2 << endl;

	      structure_ += key_name2 + ":";
	      if( class_name2 == "TTree" )
		vtr_.push_back( (TTree*)tdf->Get( key_name2.c_str() ) );

	    }
	}
    }

  for( int i=1; i<vtr_.size(); i++ )
    if( vtr_[0] == vtr_[i] )
      bl_same_tree_structure_ = bl_same_tree_structure_ && true;
    else
      bl_same_tree_structure_ = bl_same_tree_structure_ && false;
}


// public functoins
/*
vector < string > FileManager::ExtractSameTreeName( FileManager* fm )
{
  vector < string > vrtn;

  for( int i=0; i<vtr_.size(); i++ )
    {
      for( int j=0; j<fm->GetTreeNum(); j++ )
	{
	  cout << vtr_[i]->GetName() << " " << fm->GetTree(j)->GetName() << " ";
	  if( (string)vtr_[i]->GetName() == fm->GetTree(j)->GetName() )
	    {
	      cout << "<=== taken!" << endl;;
	      vrtn.push_back( vtr_[i]->GetName() );
	      break;
	    }
	  cout << endl;
	}

    }

  return vrtn;
}

vector < string > FileManager::ExtractSameTreeName( vector < FileManager* >& vfm )
{

  assert( vfm.size() > 0 );

  vector < string > vrtn, vtemp;
  for( int i=1; i<vfm.size(); i++ )
    {
      vtemp = vfm[0]->ExtractSameTreeName( vfm[i] );
      copy( vtemp.begin(), vtemp.end(), back_inserter( vrtn ) );
    }

  return vrtn;
}
*/


TTree* FileManager::GetTree( int tree_id )
{

  TTree* tr = new TTree();

  if( tree_id < 0 || tree_id >= (int)vtr_.size() )
    {
      cerr << "TTree* FileManager::GetTree( int tree_id )" << endl;
      cerr << "tree_id is " << tree_id << " ." << endl;
      cerr << "This is out of range" << endl;
      return tr;
    }
  else
    {
      tr = vtr_[tree_id];
    }
  return tr;
}

void FileManager::GetVectorTree( vector < TTree* >& vtr_arg )
{

  copy( vtr_.begin(), vtr_.end(), back_inserter( vtr_arg ) );
}

bool FileManager::IsSameTreeStructure( FileManager* fm )
{

  if( this->GetTreeNum() != fm->GetTreeNum() )
    return false;

  for( int i=0; i<vtr_.size(); i++ )
    if( vtr_[i]->GetName() != fm->GetTree(i)->GetName() )
      return false;

  return true;
}

void FileManager::Print()
{

  cout << "full path: " << file_full_path_ << endl;
  cout << "path: " << file_path_ << endl;
  cout << "name: " << file_name_ << endl;
  cout << "suffix: " << file_suffix_ << endl;
  cout << "#TTree: " << vtr_.size() << endl;
}

void FileManager::ShowTree()
{

  for( unsigned int i=0; i<vtr_.size(); i++ )
    {

      cout << "No." 
	   << setw(3) << i << " " 
	   << vtr_[i]->GetName() << endl;
    }
}
