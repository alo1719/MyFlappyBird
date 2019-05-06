#pragma once

#include "UIBase.h"
#include "BaseStateMachine.h"
#include "Graphics.h"

namespace games
{

	class About : public BaseStateMachine
	{
		About();
		virtual ~About();
	public:

		static About &instance();

		virtual void Run();
		virtual void OnRender();
		virtual void OnRenderUI();
		virtual void OnMouseEvent(MouseEvent event, int x, int y);
		virtual void OnKeywordEvent(KeywordEvent event, unsigned int state);

	private:
		UIState state;

		int groundHeight;
		float groundOffset;

		std::wstring imgBackground;
		std::wstring imgGround;
		std::wstring configFileName;

		FontHandle msgFont;
		FontHandle tipsFont;
	};
}