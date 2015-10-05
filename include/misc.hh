#ifndef MISC_HH
#define MISC_HH

#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;
template < typename T >
void ShowVC( vector < T >& vT )
{

  for( unsigned int i=0; i<vT.size(); i++ )
    cout << setw(3) << i << "\t" << vT[i] << endl;

}

template < typename T >
//void ShowVC2D( vector < vector < T > >& vT )
void ShowVC2D( vector <  T  >& vT )
{

  for( unsigned int i=0; i<vT.size(); i++ )
    {

      cout << setw(3) << i << " ";
      for( unsigned int j=0; i<vT[i].size(); j++ )
	{
	  cout << vT[i][j] << " " ;
	}

      cout << endl;
    }

}

#endif
