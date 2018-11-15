#include "Application.h"

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	Application app{};
	return app.Run();
}