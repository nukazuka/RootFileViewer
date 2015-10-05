#include "GetName.hh"

using namespace std;
// get name of branches which are common both tr1 and tr2

// naive mode
void GetName( vector < TTree* >& vtr, vector < string >& vname)
{

  const int num = vtr.size();
  if( num == 0 )
    {
      cerr << "void GetName( vector < TTree* >& vtr, vector < string >& vname)" << endl;
      cerr << "size of 1st argument vector < TTree* >& vtr == 0" << endl;
      cerr << "Somethins wrong" << endl;
      cerr << "Program is stopped" << endl;
      exit(-1);
    }

  vector < vector < string > > vvname(num);

  // loop over tree
  for( int i=0; i<num; i++)
    {

      // loop over branches in each tree
      for( int j=0; j<vtr[i]->GetNbranches(); j++)
	{

	  string name = vtr[i]->GetListOfBranches()->At(j)->GetName();
	  vvname[i].push_back( name );
	}
    }

  ShowVC2D( vname );

  // if both of real data and MC data have the name of branch, keep it
  // loop over branched in 1st TTree
  for( unsigned int i=0; i<vvname[0].size(); i++)
    {

      unsigned int counter = 0;
      // loop over TTrees from j=1
      for( unsigned int j=1; j<vvname.size(); j++)
	{

	  // loop over branches in j-th TTree
	  for( unsigned int k=0; k<vvname[j].size(); k++)
	    {

	      if( vvname[0][i] == vvname[j][k] )
		counter++;

	    }
	}

      // if the name of branch is common among TTrees, 
      // counter is equal to (# of TTree -1 )
      if( counter == vvname.size()-1 )
	{
	  vname.push_back( vvname[0][i] );
	}

    }
}
