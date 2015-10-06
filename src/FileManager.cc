#include "FileManager.hh"

using namespace std;

// constructer
FileManager::FileManager( string file_full_path_arg )
{

  file_full_path_ =   file_full_path_arg;
  //  cout << "FileManager::  constructer" << endl;
  Init();

}

// private functions
void FileManager::Init()
{

  file_path_ = GetPathToFile( file_full_path_ );
  file_name_ = GetFileName( file_full_path_ );
  file_suffix_ = GetSuffix( file_name_ );
  structure_ = "";

  tf_ = new TFile( file_full_path_.c_str(), "READ" );
  bl_exist_ = tf_->IsOpen();

  if( bl_exist_ == false )
    {
      cerr << "void FileManager::Init()" << endl;
      cerr << file_full_path_ << " does not exist" << endl;
      exit(-1);
    }
  
  if( file_suffix_ != ".root" )
    {
      cout << "It's not Root file" << endl;
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
      bool bl_folder = tf_->GetListOfKeys()->At(i)->IsFolder();

      structure_ += key_name + "/";
      if( bl_folder )
	{
	  TDirectoryFile* tdf = (TDirectoryFile*)tf_->Get( key_name.c_str() );

	  for( int j=0; j<tdf->GetNkeys(); j++ )
	    {

	      string key_name2 = tdf->GetListOfKeys()->At(j)->GetName();
	      string class_name2 = tdf->Get( key_name2.c_str() )->ClassName();

	      structure_ += key_name2 + ":";
	      if( class_name2 == "TTree" )
		vtr_.push_back( (TTree*)tdf->Get( key_name2.c_str() ) );

	    }
	}

    }



}


// public functoins

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
