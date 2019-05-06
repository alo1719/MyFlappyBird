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
		tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 13);
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
		Graphics::Instance().FillRoundedRectangle(rect, 0xffffff, 0.5);

		std::wstringstream stream;
		stream << L"使用方向上键/方向下键控制引力的方向" << std::endl;
		stream << L"按下Shift键启用引力激增, 可以增大引力并突破最大速度限制" << std::endl;
		stream << L"在开始菜单按下方向上键可以快速开始游戏" << std::endl;
		stream << L"2016141463067张心诣" << std::endl;
		std::wstring msg = stream.str();
		Graphics::Instance().DrawText(
			msg.c_str(),
			msg.length(),
			tipsFont,
			D2D1::RectF(left + 20, top + 20, left + width - 40, top + height - 70),
			0);
		msg = L"按向上/向下键继续";
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
		if (event == KeywordEvent::KEYDOWN && (state == VK_UP || state == VK_DOWN))
		{
			StateMachine::Instance().NextState(GameStatus::UI_PANEL_STATUS);
		}
	}

	void About::Run()
	{
	}
}