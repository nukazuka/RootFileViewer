#ifndef Arguemnt_hh
#define Arguemnt_hh

#include "MyLibrary.hh"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "ArgumentParser.hh"
#include "FileManager.hh"
#include "misc.hh"

using namespace std;
class Argument
{
private:

  ArgumentParser* parser_;

  /*
  int num_RD_;
  int num_MC_;
  int num_specified_;
  */

  int tree_id_ = -2;
  vector < int > vtree_id_;
  int min_bin_num_ = -1;

  double norm_val_ = 1.0;
  double bin_factor_ = 1.0;
  
  string data_;
  string data_specified_;
  string cut_;
  string draw_;
  string draw_ratio_;
  string option_;
  string suffix_;
  string save_;
  string specified_tree_name_ = "";
  string specified_branch_name_ = "";
  string tree_name_ = "";
  
  bool bl_same_structure_ = true;

  // option boolian
  bool bl_force_	= false;
  bool bl_print_	= false;
  bool bl_logx_		= false;
  bool bl_logy_		= false;
  bool bl_wait_		= false;
  bool bl_ratio_	= false;
  bool bl_both_		= false;
  bool bl_tree_		= false;
  bool bl_branch_	= false;
  bool bl_draw_		= false;
  bool bl_draw_ratio_	= false;
  bool bl_cut_		= false;
  bool bl_norm_		= false;
  bool bl_test_		= false;
  bool bl_out_dir_	= false;
  bool bl_min_bin_	= false;
  bool bl_no_overwrite_ = false;
  //  bool bl_sve_          = false;
  // special option
  bool bl_s_cut_DY2014_ = false;
  bool bl_bin_factor_   = false;
  /*
  bool   bl_RD_rand_cut_;
  bool   bl_test_;
  bool   bl_debug_;
  bool   bl_in_tra_;
  bool   bl_MC_;
  bool   bl_MC_no_text_;
  */

  vector < string > vfile_;
  //  vector < Int_t > vcolors_;
  vector < int > vcolors_;
  vector < string > vtags_;
  vector < string > vcut_;

  vector < FileManager* > vfm_;
    
  void   Init();

  bool CheckFileStructure();
  void ExtractCut();
  void ExtractFileName();
  void ExtractTreeID();
  string ExtractSaveName();

  void ShowTreeInfo();
  // interactive things
  void AskTreeID();
public:

  Argument			(int argc, char* argv[] );
  ~Argument			();
  void Option			();

  void GetVectorTree		( vector < TTree* >& vtr_arg );
  void GetVectorCut		( vector < string >& vcut_arg );
  double GetBinFactor           (){ return bin_factor_;};
  string GetDrawOption		(){ return draw_;};
  string GetDrawRatioOption	(){ return draw_ratio_;};
  string GetFileName		( int num );
  int    GetFileNum		(){ return vfile_.size();};
  string GetSaveName		(){ return save_;};
  double GetNormVal		(){ return norm_val_;};
  int    GetMinBin		(){ return min_bin_num_;};

  // checking option 
  bool IsForce			(){ return bl_force_        ;};
  bool IsPrint			(){ return bl_print_        ;};
  bool IsLogx			(){ return bl_logx_         ;};
  bool IsLogy			(){ return bl_logy_         ;};
  bool IsWait			(){ return bl_wait_         ;};
  bool IsRatio			(){ return bl_ratio_        ;};
  bool IsBoth			(){ return bl_both_         ;};
  bool IsTree			(){ return bl_tree_         ;};
  bool IsBranch		(){ return bl_branch_       ;};
  bool IsNorm			(){ return bl_norm_         ;};
  bool IsTest			(){ return bl_test_         ;};
  bool IsMinBin		(){ return bl_min_bin_      ;};
  bool IsNoOverwrite		(){ return bl_no_overwrite_ ;};

  void ShowStatus		();
};
#endif
