#include "waterwave.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto game = std::make_shared<WaterWave>(hInstance);

	if (game->initiate() == SGF::ERR_NONE)
	{
		game->run(nCmdShow);
	}

	game->uninitiate();

	return 0;
}
