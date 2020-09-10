#include "renderingtool.hpp"
#include "gstruct.hpp"

int main()
{
	rtl::init(800, 600, false, "Slay the Spaghetti");
	//FreeConsole();
	if (!sysc::pathIsValid("cfg"))
	{
		sysc::raise("cfg folder missing");
	}
	if (!sysc::pathIsValid("assets"))
	{
		sysc::raise("assets folder missing");
	}
	rtl::setClearColor(125.f, 75.f, 235.f);

	//

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
	