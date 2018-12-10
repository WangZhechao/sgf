#pragma once

#include <Windows.h>
#include <memory>
#include "sgferr.h"

namespace SGF
{
	class BaseWindow;
	class D2DRender;
	class EventDispatcher;
	class Sound;

	class GameApp: public std::enable_shared_from_this<GameApp>
	{
	public:
		GameApp(HINSTANCE);
		virtual ~GameApp();

	public:
		virtual ERR_TYPE initiate();
		virtual void uninitiate();
		virtual void run(int nCmdShow = SW_SHOWNORMAL);
		virtual std::shared_ptr<GameApp> getPtr();

	public:
		
		virtual int gameInitiate() = 0;
		virtual void gameUpdate(float dt) = 0;
		virtual void gameRender(float lag) = 0;
		virtual void gameUninitiate() = 0;


	public:
		const HINSTANCE& getHINSTANCE() const;
		std::shared_ptr<BaseWindow> getWindow() const;
		std::shared_ptr<D2DRender> getRender() const;
		std::shared_ptr<EventDispatcher> getEventDispatcher() const;
		std::shared_ptr<Sound> getSound() const;

	protected:
		HINSTANCE _hinstance;
		std::shared_ptr<BaseWindow> _baseWindow;
		std::shared_ptr<D2DRender> _d2dRender;
		std::shared_ptr<EventDispatcher> _eventDispatcher;
		std::shared_ptr<Sound> _sound;
	};
}
