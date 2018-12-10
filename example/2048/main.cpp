#include "2048.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto game = std::make_shared<City2048>(hInstance);

	if (game->initiate() == SGF::ERR_NONE)
	{
		game->run(nCmdShow);
	}

	game->uninitiate();

	return 0;
}
