#ifndef CONFIGHANDLER_HH
#define TEMPLATE_HH

#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"

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
 - norm : a flag of normalization of hist
 - logx : a flag to set x axis to log scale
 - logy : a flag to set x axis to log scale
 - logz : a flag to set x axis to log scale
*/


class ConfigHandler
{
private :
  
  string config_path_ = "";
  vector < string > vdata_path_;
  vector < string > vcut_;
  string output_path_ = "";
  string tree_name_ = "";
  string mode_ = "";
  bool bl_norm_ = false;
  bool bl_logx_ = false;
  bool bl_logy_ = false;
  bool bl_logz_ = false;
  
public :
  ConfigHandler ( string config_path );
};
#endif //TEMPLATE_HH
