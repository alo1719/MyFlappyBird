#pragma once

#include "ResourceManager.h"

namespace games
{
	using std::map;
	using std::vector;
	using std::wstring;
	using std::unordered_map;

    typedef int FontHandle;

	enum class RenderLevel
	{
		FIRST,
		SECOND,
		THIRED
	};

	//
	// note: not release resource in ~Graphics.
	// 
	// introduce��
	//		����Direct2D���ṩ�˻���ͼ�β����뵥ɫ��ˢ��
	//		������Դ��Ϊ�豸�޹���Դ���豸�����Դ��
	//		�豸�����Դ��λͼ����ˢ���豸�޹���Դ�����塣
	//		�豸�����Դ�ڳ��ִ���ʱ��Ҫ����������Դ�����޹�
	//		��Դ����Ҫ�˲����������豸�����Դ��Ӧ����ע���
	//		ʱ��ص������н������롣
	//		Render ���ֲ���֡�ص����������ⲿ����ע��Render�¼���
	//		Graphics ����ÿ�� Render �е���ָ���Ļص����������У�
	//		Render ��Ϊ���ֲ�ͬ���ȼ��� Render�����ڸ������ȼ���ͬ
	//		�Ľ��档
	//
	class Graphics
	{
		template<class Interface>
		inline void SafeRelease(
			Interface **ppInterfaceToRelease
			)
		{
			if (*ppInterfaceToRelease != NULL)
			{
				(*ppInterfaceToRelease)->Release();
				(*ppInterfaceToRelease) = NULL;
			}
		}

	public:

		// singleton 
		static Graphics &Instance();

		// call methods for instantiating drawing resources 
		// and binding HWND to window drawing resource.
		// in:
		//		hwnd - windows getInstance
		//		changeDPI - change window size to DPI.
		// 
		HRESULT initialize(HWND hwnd, bool changeDPI = false);

		// Draw content.
		HRESULT OnRender();

		// Resize the render target.
		void OnResize(UINT width_, UINT height_);
		D2D1_SIZE_F RenderSize() { return m_pRenderTarget->GetSize(); }

		bool GetBitmapSize(const wstring &path, D2D1_SIZE_F &rect);

		// font resource.
		FontHandle CreateFontObject(
			const wchar_t *font_name, 
			float size, 
			const wchar_t *local = L"zh-cn"
			);
		void FontCenter(FontHandle handle);
		bool DestroyFontObject(FontHandle handle);
		void DestroyAllFontObject();

		// ��Ҫ�����ͼƬ��������Դ����������
		void DestroyDeviceResource();

		// render callback
		void RegisterRenderCallback(RenderLevel level, std::function<void()> callback);
		void DestroyRenderCallback();

		// Draw OP.
		bool DrawLine(
			D2D1_POINT_2F from,
			D2D1_POINT_2F to,
			INT32 rgb,
			FLOAT alpha = 1.0f,
			FLOAT strokeWidth = 1.0f
			);

		bool DrawRectangle(
			D2D1_RECT_F &rect,
			INT32 rgb,
			FLOAT alpha = 1.0f,
			FLOAT strokeWidth = 1.0f
			);

		bool FillRectangle(
			D2D1_RECT_F &rect,
			INT32 rgb,
			FLOAT alpha = 1.0f
			);

		bool FillRoundedRectangle(
			const D2D1_ROUNDED_RECT &roundedRect,
			INT32 rgb,
			FLOAT alpha = 1.0f
			);

		bool DrawRoundedRectangle(
			const D2D1_ROUNDED_RECT &roundedRect,
			INT32 rgb,
			FLOAT alpha = 1.0f,
			FLOAT strokeWidth = 1.0f
			);

		bool DrawEllipse(
			const D2D1_ELLIPSE &ellipse,
			INT32 rgb,
			FLOAT alpha = 1.0f,
			FLOAT strokeWidth = 1.0f
			);

		bool FillEllipse(
			const D2D1_ELLIPSE &ellipse,
			INT32 rgb,
			FLOAT alpha = 1.0f
			);

		bool DrawText(
			const wchar_t *wstring,
			UINT wstringLength,
			FontHandle textFormat,
			D2D1_RECT_F &layoutRect,
			INT32 rgb,
			FLOAT alpha = 1.0f
			);

		bool DrawBitmap(
			const wstring &path,
			D2D1_RECT_F &destinationRectangle,
			D2D1_RECT_F &sourceRectangle,
			FLOAT opacity = 1.0f
			);

        bool DrawRotationBitmap(
            const wstring &path,
            D2D1_RECT_F &destinationRectangle,
            FLOAT rotaion,
            D2D1_POINT_2F point,
            FLOAT opacity = 1.0f);

        bool DrawRotationBitmap(
            const wstring &path,
            D2D1_RECT_F &destinationRectangle,
            D2D1_RECT_F &sourceRectangle,
            FLOAT rotaion,
            D2D1_POINT_2F point,
            FLOAT opacity = 1.0f);

		bool DrawBitmap(
			const wstring &path,
			D2D1_RECT_F &destinationRectangle,
			FLOAT opacity = 1.0f
			);

	private:

        Graphics();
        ~Graphics();

		// initialize device-independent resources.
		HRESULT CreateDeviceIndependentResources();

		// initialize device-dependent resources.
		HRESULT CreateDeviceResources();

		// Release device-dependent resource.
		void DiscardDeviceResources();

		// check the bitmap resources is existance.
		bool CheckBitmapResource(const wstring &str);
		bool CheckFontResource(FontHandle handle);

		HRESULT LoadBitmapFromFile(
			PCWSTR uri,
			UINT destinationWidth,
			UINT destinationHeight,
			ID2D1Bitmap **ppBitmap
			);

		ID2D1Bitmap *LoadBitmap(const wstring &path);

	private:
		HWND hwnd_;
		ID2D1Factory* m_pDirect2dFactory = nullptr;
		IDWriteFactory* m_pDWriteFactory = nullptr;
		ID2D1HwndRenderTarget* m_pRenderTarget = nullptr;
		
		// bitmap resoruces,
		unordered_map<wstring, ID2D1Bitmap*> bitmapList;

		// fron resources manager.
		ResourceLibrary<FontHandle, IDWriteTextFormat*> textFormatList;

		// render state, BeginDraw is call if it true.
		bool renderState = false;
	
		// Render callback.
		vector<std::function<void()>> firstRenderCallbackList;
		vector<std::function<void()>> secondRenderCallbackList;
		vector<std::function<void()>> thirdRenderCallbackList;
	};

}

