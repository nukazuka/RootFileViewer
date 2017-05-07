#include "GetName.hh"

using namespace std;
// get name of branches which are common both tr1 and tr2

// naive mode
//void GetName( vector < TTree* >& vtr, vector < string >& vname)
vector < string > GetName( vector < TTree* >& vtr )
{
  
  vector < string > vname;
  
  const int num = vtr.size();
  if( num == 0 )
    {
      cerr << "void GetName( vector < TTree* >& vtr, vector < string >& vname)" << endl;
      cerr << "size of 1st argument vector < TTree* >& vtr == 0" << endl;
      cerr << "Somethins wrong" << endl;
      cerr << "Program is stopped" << endl;
      exit(-1);
    }

  // container of branch names
  vector < vector < string > > vvname(num);
  // container of array size of branch
  vector < vector < int    > > vvarray(num); 

  // loop over tree
  for( int i=0; i<num; i++)
    {

      // loop over branches in each tree
      for( int j=0; j<vtr[i]->GetNbranches(); j++)
	{

	  // get & store branch name
	  string name = vtr[i]->GetListOfBranches()->At(j)->GetName();
	  vvname[i].push_back( name );

	  // get & store array size of this branch
	  TLeaf* leaf = (TLeaf*)vtr[i]->GetListOfLeaves()->At(j);
	  int array_num = leaf->GetLen();
	  vvarray[i].push_back( array_num );
	}
    }

  ShowVC2D( vname );

  // if both of real data and MC data have the name of branch, keep it
  // loop over branche in 1st TTree
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

	  // store this branch name anyway
	  // vname.push_back( vvname[0][i] );

	  // if this branch is array, store like :
	  //   branch[0], branch[1], ...
	  if( vvarray[0][i] > 1 )
	    {

	      // loop over array dimension
	      for( int j=0; j<vvarray[0][i]; j++ )
		{
		  stringstream branch_name;
		  branch_name << vvname[0][i]
			      << "[" << j << "]";
		  vname.push_back( branch_name.str() );	  
		}
	    }
	  else
	    {

	      TBranch* br = (TBranch*)vtr[0]->GetBranch( vvname[0][i].c_str() );

	      string class_name = br->GetClassName();

	      if( class_name == "TLorentzVector" )
		{
		  vname.push_back( vvname[0][i]+ ".X()" );
		  vname.push_back( vvname[0][i]+ ".Y()" );
		  vname.push_back( vvname[0][i]+ ".Z()" );
		  vname.push_back( vvname[0][i]+ ".E()" );
		  vname.push_back( vvname[0][i]+ ".Pt()" );
		}
	      else if( class_name == "TVector3" )
		{
		  vname.push_back( vvname[0][i]+ ".X()" );
		  vname.push_back( vvname[0][i]+ ".Y()" );
		  vname.push_back( vvname[0][i]+ ".Z()" );
		  vname.push_back( vvname[0][i]+ ".Pt()" );
		}
	      else
		{
		  vname.push_back( vvname[0][i] );
		}
	      
	    }
	}
    }

  return vname;
}
