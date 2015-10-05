#ifndef FILEMANAGER_HH
#define FILEMANAGER_HH

#include <string>
#include <iterator>

#include "TFile.h"
#include "TDirectoryFile.h"

#include "MyLibrary.hh"

using namespace std;

class FileManager
{
private:
  string file_full_path_;
  string file_path_;
  string file_name_;
  string file_suffix_;
  string structure_;
  //  vector < string > vtr_name;
  vector < TTree* > vtr_;

  TFile* tf_;

  bool bl_exist_;
  
  void Init();
  void Init_NotRootFile();

  void ExtractAllTree();
public:

  FileManager( string file_full_path_arg);

  string GetName(){ return file_name_;};
  string GetPath(){ return file_path_;};
  //  string GetSuffix(){ return file_suffix_;};
  string GetFileStructure(){ return structure_;};
  TTree* GetTree( int tree_id );
  int    GetTreeNum(){ return vtr_.size();};
  void   GetVectorTree( vector < TTree* >& vtr_arg );

  bool   IsExist(){ return bl_exist_;};

  void   Print();
  void   ShowTree();
};

#endif
