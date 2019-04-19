#include "main.h"

#include "StateMachine.h"

#include "BaseStateMachine.h"
#include "GameState.h"
#include "UIPanelState.h"
#include "Statistics.h"

namespace games
{
	StateMachine::StateMachine()
	{
		StateMachineInit();
	}

	StateMachine::~StateMachine()
	{
	}

	StateMachine & StateMachine::Instance()
	{
		static StateMachine machine;
		return machine;
	}

	void StateMachine::Run()
	{
		// ���ողż�¼�µ�״̬�ͷ���Դ��������һ��״̬׼����Դ
		if (next) NextState();

		auto &state_machine = m_StateHash[m_CurrentStatus];
		if (state_machine != nullptr)
		{
			state_machine->Run();
		}
	}

	void StateMachine::NextState(GameStatus state)
	{
		// ��¼��״̬��ȷ����ȫ�ͷ���Դ
		next = true;
		m_NextStatus = state;
	}

	void StateMachine::StateMachineInit()
	{
        RegisteStateMachine(GameStatus::GAME_STATUS, &games::GameState::instance());
        RegisteStateMachine(GameStatus::UI_PANEL_STATUS, &games::UIPanelState::instance());
        RegisteStateMachine(GameStatus::UI_STATISTICS, &games::Statistics::instance());
        // ��ʼ״̬
        NextState(GameStatus::UI_PANEL_STATUS);
	}

	void StateMachine::RegisteStateMachine(GameStatus state, IStateMachine * machine)
	{
		m_StateHash[state] = machine;
	}

	void StateMachine::NextState()
	{
		auto &cur_state = m_StateHash[m_CurrentStatus];
		auto &next_state = m_StateHash[m_NextStatus];

		// ��һ��״̬�ͷŵ�ռ����Դ����ܵ�����һ��״̬��
		if (cur_state != nullptr)
			cur_state->PrevNextState();
		if (next_state != nullptr)
			next_state->PrevRunState();

		m_CurrentStatus = m_NextStatus;
		next = false;
	}
}
