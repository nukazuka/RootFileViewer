#include "ArgumentParser.hh"
using namespace std;

ArgumentParser::ArgumentParser( int argc, char* argv[] )
{

  // loop over argv
  for( int i=1; i<argc; i++)
    {

      string argument = argv[i];
      string type = "", state = "";

      // for long version ( ex. --data="data.dat"
      if( argument.find("--") != string::npos )
	{

	  // 
	  if( argument.find("=") != string::npos )
	    {

	      long_version( argument, type, state );
	    }
	  else
	    {

	      type = argument.substr( 2 , argument.size() -2 );
	      state = "NULL";
	    }


	}
      else if( argument.substr( 0, 1 ) == "-" )
	{

	  // for short version 

	  // value specified version ( ex. -o output.pdf)
	  if ( i+1 < argc && ((string)argv[i+1]).substr(1,1) != "-" )
	    {

	      type = short_version(argv[i]);
	      state = argv[i+1];
	      i++;
	    }
	  // no value mode ( ex. -h )
	  else
	    {

	      type = short_version( argv[i] );
	      state = "NULL";
	    }
	}
      
      if( find(vtype_.begin(), vtype_.end(), type ) != vtype_.end() )
	{
	  MergeArgument( type, state );
	}
      else
	{
	  vtype_.push_back( type );
	  varg_.push_back( state );
	}
    }

}

// private
void ArgumentParser::long_version( string arg, string& type, string& arg_rt )
{

  type = arg.substr( 2 , arg.find_first_of("=")-2 );
  arg_rt = arg.substr( arg.find_first_of("=")+1, arg.size() - arg.find_first_of("=") );
}

string ArgumentParser::short_version( string arg )
{

  return arg.substr( 1, 1 );
}


int ArgumentParser::GetIndex( string type )
{

  int rtn = -1;

  for( unsigned int i=0; i<vtype_.size(); i++)
    {

      if( vtype_[i] == type )
	rtn = i;

    }

  return rtn;
}

void ArgumentParser::MergeArgument( string type, string state )
{

  vector < string >::iterator ite = find( vtype_.begin(), vtype_.end(), type );

  int index = (int)( ite - vtype_.begin() );
  replace( varg_.begin(), varg_.end(), varg_[index], varg_[index]+"|"+state );

}

// public 
string ArgumentParser::GetArgument( string type )
{
  
  int index = GetIndex( type );
  string arg = "";

  if( index == -1 )
    arg = "NULL";
  else
    arg = varg_[index];

  return arg;
}

bool ArgumentParser::IsSpecified( string type )
{

  int index = GetIndex( type );
  string arg = "";

  if( index == -1 )
    arg = "NPOS";
  else
    arg = varg_[index];

  // if 
  //   - arg != NULL  -> this type was specified. Return "ture"
  //   - arg == NULL  -> this type was not specified. Return "false"
  bool rtn = (arg != "NPOS") ? true : false;
  //  cout << "ArgumentParser::IsSpecified(" << type << ") = " << rtn << endl;

  return rtn;
}

void ArgumentParser::ShowAll()
{

  cout << "A number of argument : " << vtype_.size() << endl;
  
  for( unsigned int i=0; i<vtype_.size(); i++ )
    {

      cout << "No. " << i << "\t"
	   << vtype_[i] << "\t" 
	   << "\"" << varg_[i] << "\"" 
	   << endl;
    }
  //  exit(-1);
}
