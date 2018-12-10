#include "sgfgameapp.h"
#include "sgfbasewindow.h"
#include "sgfd2drender.h"
#include "sgfeventdispatcher.h"
#include "sgfsound.h"

namespace SGF
{

	GameApp::GameApp(HINSTANCE hinstance)
		: _hinstance(hinstance)
	{
		
	}

	GameApp::~GameApp()
	{

	}

	std::shared_ptr<GameApp> GameApp::getPtr()
	{
		return shared_from_this();
	}

	const HINSTANCE& GameApp::getHINSTANCE() const
	{
		return _hinstance;
	}

	std::shared_ptr<BaseWindow> GameApp::getWindow() const
	{
		return _baseWindow;
	}

	std::shared_ptr<D2DRender> GameApp::getRender() const
	{
		return _d2dRender;
	}

	std::shared_ptr<EventDispatcher> GameApp::getEventDispatcher() const
	{
		return _eventDispatcher;
	}

	std::shared_ptr<Sound> GameApp::getSound() const
	{
		return _sound;
	}

	ERR_TYPE GameApp::initiate()
	{
		_d2dRender = std::make_shared<D2DRender>(getPtr());
		_baseWindow = std::make_shared<BaseWindow>(getPtr());
		_eventDispatcher = std::make_shared<EventDispatcher>();
		_sound = std::make_shared<Sound>(getPtr());

		ERR_TYPE ret = _baseWindow->initiate();
		if (ret != ERR_NONE) {
			_d2dRender->uninitiate();
			return ret;
		}

		ret = _d2dRender->initiate();
		if (ret != ERR_NONE) {
			return ret;
		}

		return ERR_NONE;
	}


	void GameApp::uninitiate()
	{
		if (_d2dRender) {
			_d2dRender->uninitiate();
		}

		if (_baseWindow) {
			_baseWindow->uninitiate();
		}
	}


	//ÔËÐÐ
	void GameApp::run(int nCmdShow)
	{
		if (_baseWindow) {
			_baseWindow->run(nCmdShow);
		}
	}
}