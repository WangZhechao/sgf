#pragma once

namespace SGF
{
	class Render
	{
	public:
		virtual ~Render() {};

		virtual int Initiate(HWND hwnd) = 0;									//��ʼ��
		virtual void Uninitiate() = 0;											//����ʼ��

		virtual bool BeginRender() = 0;
		virtual void RenderFPS(float dt) = 0;									//��ʼ��Ⱦ
		virtual void EndRender() = 0;

		virtual void Resize(unsigned int w, unsigned int h) = 0;				//������С
		virtual void FocusChange(bool act) = 0;									//�ı佹��

		virtual void GetDesktopDpi(float* dpiX, float* dpiY) = 0;
	};
}
