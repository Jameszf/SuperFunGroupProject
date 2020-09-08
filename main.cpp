#include <iostream>
#include "renderingtool.hpp"

int main()
{
	//FreeConsole();
	rtl::init(800, 600, false, "Slay the Spaghetti");
	rtl::setClearColor(125.f, 75.f, 235.f);
	while (!rtl::shouldExit())
	{
		rtl::clearBuffers();
		//render

		//endrender
		rtl::flipDisplay();
		rtl::poll();
	}
	rtl::terminate();
	return 0;
}
	