#pragma once

namespace SGF
{
	class Render
	{
	public:
		virtual ~Render() {};

		virtual int Initiate(HWND hwnd) = 0;									//初始化
		virtual void Uninitiate() = 0;											//反初始化

		virtual bool BeginRender() = 0;
		virtual void RenderFPS(float dt) = 0;									//开始渲染
		virtual void EndRender() = 0;

		virtual void Resize(unsigned int w, unsigned int h) = 0;				//调整大小
		virtual void FocusChange(bool act) = 0;									//改变焦点

		virtual void GetDesktopDpi(float* dpiX, float* dpiY) = 0;
	};
}
