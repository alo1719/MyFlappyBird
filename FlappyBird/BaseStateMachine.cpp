#include "main.h"

#include "BaseStateMachine.h"

#include "Graphics.h"
#include "Application.h"


namespace games
{
	void BaseStateMachine::PrevRunState()
	{
		Graphics::Instance().DestroyDeviceResource();
		// ��UI֮��ע����Ϣ�ص���ȷ���¼����سɹ�
		RegisterEventCallback();
		RegisterRenderCallback();
	}

	void BaseStateMachine::PrevNextState()
	{
		DestroyEventCallback();
		DestroyRenderCallback();
	}

	void BaseStateMachine::RegisterEventCallback()
	{
		Application::Instance().mouseEventRegister(
			[&](MouseEvent event, int x, int y) {
			OnMouseEvent(event, x, y);
		});
		Application::Instance().keywordEventRegister(
			[&](KeywordEvent event, unsigned int state) {
			OnKeywordEvent(event, state);
		});
	}

	void BaseStateMachine::DestroyEventCallback()
	{
		Application::Instance().releaseEventRegister();
	}

	void BaseStateMachine::RegisterRenderCallback()
	{
		Graphics::Instance().RegisterRenderCallback(RenderLevel::FIRST, [&] { OnRender(); });
        Graphics::Instance().RegisterRenderCallback(RenderLevel::THIRED, [&] { OnRenderUI(); });
	}

	void BaseStateMachine::DestroyRenderCallback()
	{
		Graphics::Instance().DestroyRenderCallback();
	}
}

