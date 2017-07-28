#pragma once

typedef void TextFormat;

enum SGF_FONT_STYLE
{
	SGF_FS_NORMAL,
	SGF_FS_OBLIQUE,
	SGF_FS_ITALIC
};

enum SGF_FONT_WEIGHT
{
	SGF_FW_THIN = 100,
	SGF_FW_NORMAL = 400,
	SGF_FW_MEDIUM = 500,
	SGF_FW_BOLD = 700,
	SGF_FW_HEAVY = 900
};

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
		virtual void GetSize(SIZE& size) = 0;

	public:
		//未完善接口
		virtual TextFormat* CreateTextFormat(const TCHAR* fontfamily, int fontsize, 
			SGF_FONT_STYLE fontstyle, SGF_FONT_WEIGHT fontweight) = 0;
		virtual void DestoryTextFormat(TextFormat*) = 0;
		virtual void SetTextColor(TextFormat* format, DWORD rgb) = 0;
		virtual void DrawText(int x, int y, const TCHAR* text, const TextFormat* format) = 0;
	};
}
