#pragma once

#include <Windows.h>
#include <memory>
#include <functional>

namespace SGF
{
	class Render;
	class EventDispatcher;
	class WinApp;
}


struct Modules
{
	std::weak_ptr<SGF::Render> render;
	std::weak_ptr<SGF::EventDispatcher> eventDispatcher;
	std::weak_ptr<SGF::WinApp> winApp;
};


struct GameInterface
{
	std::function<void(Modules*)> init;
	std::function<void(float dt)> update;
	std::function<void(float dt)> render;
	std::function<void(Modules*)> uninit;
};


//获取接口
typedef GameInterface*(*GetInterfaceFun)();