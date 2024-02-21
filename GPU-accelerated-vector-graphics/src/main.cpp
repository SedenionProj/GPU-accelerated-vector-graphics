#include <seden.h>

int main()
{
	Seden::init(1280, 720, "vector graphics");

	while (Seden::isRunning())
	{
		Seden::clear();

		Seden::display();
	}

	Seden::terminate();
}