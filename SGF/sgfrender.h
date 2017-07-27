#pragma once

namespace SGF
{
	typedef void TextFormat;

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
		virtual void GetSize(SIZE& size) = 0;

	public:
		//未完善接口
		virtual TextFormat* CreateTextFormat() = 0;
		virtual void DestoryTextFormat(TextFormat*) = 0;
		virtual void SetTextColor(TextFormat* format, DWORD rgb) = 0;
		virtual void DrawText(int x, int y, const TCHAR* text, const TextFormat* format) = 0;
	};
}
