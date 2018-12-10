#pragma once

#include <Windows.h>
#include <memory>
#include "sgferr.h"

namespace SGF
{
	class GameApp;

	class Sound
	{
	public:
		Sound(const std::shared_ptr<GameApp>& app);
		virtual ~Sound();

		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;

	public:
		bool playBackgroundMusic(const TCHAR* filename, bool loop = true);
		void closeBackgroundMusic();

		bool playEffect(const TCHAR* filename, bool block = false);


	private:
		bool _bgPlay;
	};
}