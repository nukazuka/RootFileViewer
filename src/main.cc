#include "main.hh"
using namespace std;

int main( int argc, char* argv[] )
{

  Argument* arg = new Argument( argc, argv );
  gErrorIgnoreLevel = 5000;

  TApplication* app = new TApplication( "app" , &argc, argv );
  Drawer* dr = new Drawer( arg );

  app->Terminate();
  return 0;
}
