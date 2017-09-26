#include "FaExample.h"

#include "../CommonInterfaces/CommonExampleInterface.h"
#include "../CommonInterfaces/CommonGUIHelperInterface.h"

int main(int argc, char* argv[])
{
	DummyGUIHelper noGfx;

	CommonExampleOptions options(&noGfx);
	CommonExampleInterface*    example = FaExampleCreateFunc(options);

	example->initPhysics();
	example->stepSimulation(1.f/60.f);
	example->exitPhysics();

	delete example;

	return 0;
}


