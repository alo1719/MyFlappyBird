#include "main.h"
#include "About.h"

#include "Ulit.h"
#include "constant.h"
#include "Graphics.h"
#include "Application.h"
#include "StateMachine.h"
#include <sstream>
#include <fstream>

namespace games
{

	About::About()
	{
		msgFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 18);
		tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
		Graphics::Instance().FontCenter(tipsFont);

		std::wstring &path = GetAppPath();

		// 初始化素材部分
		imgBackground = path + L"Resource\\background.png";
		imgGround = path + L"Resource\\Ground.png";

		groundHeight = MS_GROUNDHEIGHT;
		groundOffset = 0.f;

		ClearState(state);
	}


	About::~About()
	{
		Graphics::Instance().DestroyFontObject(msgFont);
		Graphics::Instance().DestroyFontObject(tipsFont);
	}

	About & About::instance()
	{
		static About stat;
		return stat;
	}

	void About::OnRender()
	{
		D2D1_RECT_F rect{ 0, 0, 800, 600 };
		Graphics::Instance().DrawBitmap(imgBackground, rect);

		// 绘制地板
		for (int i = 0; i <= MS_GROUNDDRAWCOUNT; ++i)
		{
			D2D1_RECT_F ground{
				MS_GROUNDWIDTH * i,
				GAMES_SIZE_H - groundHeight,
				MS_GROUNDWIDTH * i + MS_GROUNDWIDTH,
				GAMES_SIZE_H
			};
			Graphics::Instance().DrawBitmap(imgGround, ground);
		}
	}

	void About::OnRenderUI()
	{
		int width = 300, height = 200;
		float left = (GAMES_SIZE_W - width) >> 1;
		float top = (GAMES_SIZE_H - height - 100) >> 1;
		auto rect = D2D1::RoundedRect(
			D2D1::RectF(left, top, left + width, top + height),
			3.f, 3.f
		);
		Graphics::Instance().FillRoundedRectangle(rect, 0xf0f000, 0.5);

		std::wstringstream stream;
		stream << L"制作人: 张心诣" << std::endl;
		stream << L"学号: 2016141463067" << std::endl;
		std::wstring msg = stream.str();
		Graphics::Instance().DrawText(
			msg.c_str(),
			msg.length(),
			msgFont,
			D2D1::RectF(left + 20, top + 20, left + width - 40, top + height - 70),
			0);
		msg = L"按空格继续";
		Graphics::Instance().DrawText(
			msg.c_str(),
			msg.length(),
			tipsFont,
			D2D1::RectF(left + 20, top + height - 70, left + width - 40, top + height - 20),
			0);
	}

	void About::OnMouseEvent(MouseEvent event, int x, int y)
	{
	}

	void About::OnKeywordEvent(KeywordEvent event, unsigned int state)
	{
		if (event == KeywordEvent::KEYDOWN && state == VK_SPACE)
		{
			StateMachine::Instance().NextState(GameStatus::UI_PANEL_STATUS);
		}
	}

	void About::Run()
	{
	}
}