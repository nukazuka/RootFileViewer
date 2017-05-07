#ifndef CONFIGHANDLER_HH
#define CONFIGHANDLER_HH

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "MyLibrary.hh"
#include "GetName.hh"

using namespace std;

class BranchConfig
{
public:
  string name_ = "" ;
  int bin_     = -1;
  double xmin_ = -9999.9;
  double xmax_ = -9999.9;

  BranchConfig(){};

  BranchConfig( string name, int bin, double xmin, double xmax )
  {
    name_ = name;
    bin_  = bin;
    xmin_ = xmin;
    xmax_ = xmax;     
  }
  
  BranchConfig( const vector < string >& vword )
  {

    if( vword.size() > 0 )
      name_ = vword[0];
    else
      name_ = "";
    
    if( vword.size() > 1 )
      bin_ = String2Int( vword[1] );
    else
      bin_ = -1;
    
    if( vword.size() > 2 )
      xmin_ = String2Double( vword[2] );
    else
      xmin_ = -9999.9;

    if( vword.size() > 3 )
      xmax_ = String2Double( vword[3] );
    else
      xmax_ = -9999.9;
  }

  void Print()
  {
    cout << "    BranchConfig: "
	 << setw(20) << name_ << " "
	 << setw(4)  << bin_  << " "
	 << setw(4)  << xmin_ << " "
	 << setw(4)  << xmax_
	 << endl;
  }
};
  
/*! 
  @file ConfigHandler.hh
  @brief a class to handle a config file
  @detail the format of a config file should be .....
  line: <tag> <words>

  tags are :
  - data : a path to the data
  - cut : cut to be applied
  - output : a path to the output file ( should be pdf )
  - tree_name : a name of TTree to be drawn
  - mode
      - single             : normal mode, hist of data are drawn in a pad
      - ratio(comparison)  : ratio of data (data[i] / data[0]) are drawn in a pad
      - both               : hist of data and ratio of data are drawn in each pad
      - test               : only the first branch is drawn in single mode
 - norm : a flag of normalization of hist
 - logx : a flag to set x axis to log scale
 - logy : a flag to set x axis to log scale
 - logz : a flag to set x axis to log scale
 - draw-option : a option for draw
*/

class ConfigHandler
{
private :
  
  string config_path_ = "";
  vector < string > vdata_path_;
  vector < string > vcut_;
  vector < string > vbranch_name_;
  vector < string > vbranch_except_; // these branches should not be drawn, excepted is better?
  vector < BranchConfig* > vbranch_config_;
  
  string output_path_ = "";
  string tree_name_ = "";
  string mode_ = "";
  string draw_option_ = "";
  
  bool bl_norm_ = false;
  bool bl_logx_ = false;
  bool bl_logy_ = false;
  bool bl_logz_ = false;

  bool bl_each_cut_ = false;
  // functions
  void Init( string config_path );
  bool ReadConfigFile();
  void RegisterSetting( string tag, vector < string > vword  );

  bool Convert2Bool( string words );

public :
  ConfigHandler ( string config_path );

  static void GiveConfigTemplate( string data_path = "" , string tr_name = "" );
  static string GiveConfigTemplate_Tree( string data_path , string tr_name );
  
  void Print();

  vector < BranchConfig* > GetBranchConfig(){ return vbranch_config_;   };
  string GetConfigPath			(){ return  config_path_;	};
  vector < string > GetBranchName	(){ return vbranch_name_;       };
  vector < string > GetBranchExcept	(){ return vbranch_except_;     };
  vector < string > GetData		(){ return vdata_path_;		};
  vector < string > GetCuts		(){ return vcut_;		};
  string GetDrawOption			(){ return  draw_option_;	};
  string GetOutputPath			(){ return  output_path_;	};
  string GetTreeName			(){ return  tree_name_;		};
  string GetMode			(){ return  mode_;		};
  bool IsEachCut			(){ return  bl_each_cut_;       }; // not ready
  bool IsNorm				(){ return  bl_norm_;		};
  bool IsLogx				(){ return  bl_logx_;		};
  bool IsLogy				(){ return  bl_logy_;		};
  bool IsLogz				(){ return  bl_logz_;		};  
};
#endif // CONFIGHANDLER_HH
