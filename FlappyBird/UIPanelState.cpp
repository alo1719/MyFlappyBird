#include "main.h"
#include "UIPanelState.h"

#include "Application.h"
#include "UIControl.h"
#include "constant.h"
#include "Ulit.h"
#include "StateMachine.h"
#include "GameState.h"

namespace games
{

    UIPanelState::UIPanelState()
    {
        selectButtonFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 20);
        tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
        Graphics::Instance().FontCenter(selectButtonFont);
        Graphics::Instance().FontCenter(tipsFont);

        std::wstring &path = GetAppPath();

        // 初始化素材部分
        imgBackground = path + L"Resource\\background.png";
        imgGround = path + L"Resource\\Ground.png";

        groundHeight = MS_GROUNDHEIGHT;
        groundOffset = 0.f;

        ClearState(state);
    }

    UIPanelState::~UIPanelState()
    {
        Graphics::Instance().DestroyFontObject(selectButtonFont);
        Graphics::Instance().DestroyFontObject(tipsFont);
    }

    UIPanelState & UIPanelState::instance()
    {
        static UIPanelState state;
        return state;
    }

    void UIPanelState::UpdateGround(float ElapsedTime)
    {
        // 移动地板
        groundOffset = (float)fmod(groundOffset + ElapsedTime * MS_STAGEMOVESPEED, MS_GROUNDWIDTH);
    }

    void UIPanelState::BeginGames()
    {
        StateMachine::Instance().NextState(GameStatus::GAME_STATUS);
    }

    void UIPanelState::Statistics()
    {
        StateMachine::Instance().NextState(GameStatus::UI_STATISTICS);
    }

	void UIPanelState::About()
	{
		StateMachine::Instance().NextState(GameStatus::UI_ABOUT);
	}

    void UIPanelState::Run()
    {
        static DWORD last = GetTickCount();
        DWORD now = GetTickCount();
        float use = (now - last) / 1000.f;
        last = now;
        UpdateGround(use);
    }

    void UIPanelState::OnRender()
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

    void UIPanelState::OnRenderUI()
    {
        UIPrepare(state);

		// 按钮的位置以及颜色
        float left = (GAMES_SIZE_W - 100) >> 1;
        float top = (GAMES_SIZE_H - 150 - 100) >> 1;
        float right = left + 100;
        float bottom = top + 40;
		int32_t color = 0xfcbdf6;

		Graphics::Instance().DrawText(
			L"开始游戏",
			lstrlenW(L"开始游戏"),
			selectButtonFont,
			D2D1::RectF(left, top, right, bottom),
			0x000000);
        if (UIControl::TextButton(state, 1, selectButtonFont, L"", left, top, right, bottom, color))
        {
            BeginGames();
        }

        top += 50;
        bottom = top + 40;
		Graphics::Instance().DrawText(
			L"统计数据",
			lstrlenW(L"统计数据"),
			selectButtonFont,
			D2D1::RectF(left, top, right, bottom),
			0x000000);
        if (UIControl::TextButton(state, 2, selectButtonFont, L"", left, top, right, bottom, color))
        {
            Statistics();
        }

		top += 50;
		bottom = top + 40;
		Graphics::Instance().DrawText(
			L"关于游戏",
			lstrlenW(L"关于游戏"),
			selectButtonFont,
			D2D1::RectF(left, top, right, bottom),
			0x000000);
		if (UIControl::TextButton(state, 3, selectButtonFont, L"", left, top, right, bottom, color))
		{
			About();
		}

        UIFinish(state);
    }

    void UIPanelState::OnMouseEvent(MouseEvent event, int x, int y)
    {
        InitUIState(state, event, x, y);
    }

    void UIPanelState::OnKeywordEvent(KeywordEvent event, unsigned int state)
    {
		if (event == KeywordEvent::KEYDOWN && state == VK_UP)
		{
			BeginGames();
		}
    }
}