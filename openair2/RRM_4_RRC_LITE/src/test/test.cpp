// ===========================================================================
#include <stdio.h>
#include <signal.h>
// ===========================================================================
#include "RRMMessageHandler.h"
#include "TestScenario.h"
// ===========================================================================
static	bool quit = false;
// ===========================================================================
void sigint(int)
// ===========================================================================
{
  quit = true;
}
// ===========================================================================
int main(int argc,char *argv[])
// ===========================================================================
{

  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, sigint);

  // start RRC server
  RRMMessageHandler::Instance();

  // start scenario
  TestScenario::Start(TestScenario::SCENARIO_ALL_IS_OK);

  RRMMessageHandler::Instance()->Join(&quit);
  fprintf(stderr, "\nRRM LTE Exiting...\n");
  return 0;
}


