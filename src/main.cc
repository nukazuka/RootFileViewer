#include "main.hh"
using namespace std;

int main( int argc, char* argv[] )
{
  TStopwatch* sw = new TStopwatch();
  sw->Start();
  Argument* arg = new Argument( argc, argv );
  gErrorIgnoreLevel = 5000;

  TApplication* app = new TApplication( "app" , &argc, argv );
  Drawer* dr = new Drawer( arg );

  sw->Stop();
  sw->Print();
  app->Terminate();
  return 0;
}
