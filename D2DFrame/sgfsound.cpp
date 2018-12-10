#include "sgfsound.h"
#include "sgfgameapp.h"
#include <tchar.h>
#include <assert.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

namespace SGF
{
	Sound::Sound(const std::shared_ptr<GameApp>& app)
		: _bgPlay(false)
	{

	}

	Sound::~Sound()
	{

	}


	void Sound::closeBackgroundMusic()
	{
		if (_bgPlay) {
			mciSendString(TEXT("close all"), NULL, 0, NULL);
		}
	}


	bool Sound::playBackgroundMusic(const TCHAR* filename, bool loop)
	{
		TCHAR playCommand[1024] = { 0 };
		MCIERROR merr = 0;
		_bgPlay = false;

		if (loop)
		{
			_stprintf(playCommand, TEXT("open %s type MPEGVideo"), filename);
			merr = mciSendString(playCommand, NULL, 0, NULL);
			if (merr != 0) {
				return false;
			}

			_stprintf(playCommand, TEXT("play %s repeat"), filename);
			merr = mciSendString(playCommand, NULL, 0, NULL);
			if (merr != 0) {
				return false;
			}
		}
		else
		{
			_stprintf(playCommand, TEXT("play %s"), filename);
			MCIERROR merr = mciSendString(playCommand, NULL, 0, NULL);
			if (merr != 0) {
				return false;
			}
		}

		_bgPlay = true;

		return true;
	}


	bool Sound::playEffect(const TCHAR* filename, bool block)
	{
		BOOL ret = FALSE;

		if(block)
			ret = PlaySound(filename, NULL, SND_FILENAME);
		else
			ret = PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);

		if (ret)
			return true;

		return false;
	}
}