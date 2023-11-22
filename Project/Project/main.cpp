#include "Menu.h"
#include "HomeAutomation.h"
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HomeAutomation ha;
	return 0;
}
