#include "sgfgameapp.h"

#include "sgfeventlistenermouse.h"
#include "sgfeventdispatcher.h"

class TestGame : public SGF::GameApp
{
public:
	TestGame(HINSTANCE h): SGF::GameApp(h) {}

	virtual int gameInitiate() { 
		OutputDebugString(TEXT("gameInitiate\n")); 
		
		auto mouseListener = SGF::EventListenerMouse::create();
		mouseListener->onMouseMove = [](SGF::EventMouse* event) {
			OutputDebugString(TEXT("onMouseMove\n"));
		};
		_eventDispatcher->addEventListener(mouseListener);

		return 0; 
	}

	virtual void gameUpdate(float dt) {
		Sleep(1); 
	}

	virtual void gameRender(float lag) {  
	
	}

	virtual void gameUninitiate() { 
		OutputDebugString(TEXT("gameUninitiate\n")); 
	}
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto game = std::make_shared<TestGame>(hInstance);

	if (game->initiate() == SGF::ERR_NONE)
	{
		game->run(nCmdShow);
	}

	game->uninitiate();

	return 0;
}
