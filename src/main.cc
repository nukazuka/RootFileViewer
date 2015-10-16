#include "main.hh"
using namespace std;

int main( int argc, char* argv[] )
{

  gErrorIgnoreLevel = 5000;
  TApplication* app = new TApplication( "app" , &argc, argv );

  Argument* arg = new Argument( argc, argv );
  cout << "here3" << endl;

  Drawer* dr = new Drawer( arg );
  cout << "here4" << endl;

  app->Terminate();
  return 0;
}
