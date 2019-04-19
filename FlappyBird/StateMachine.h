#pragma once

namespace games
{
	using std::map;

	enum class GameStatus
	{
		NONE,
        GAME_STATUS,
        UI_PANEL_STATUS,
        UI_STATISTICS,
	};

    class IStateMachine;

	//
	// state machine manager
	// 
	class StateMachine
	{
	public:
		StateMachine();
		~StateMachine();

		static StateMachine &Instance();

		void Run();
		void NextState(GameStatus state);
		void RegisteStateMachine(GameStatus state, IStateMachine *machine);
	
	private:
        void StateMachineInit();
		void NextState();

	private:
		map<GameStatus, IStateMachine*> m_StateHash;
		GameStatus m_CurrentStatus;	// ��ǰ״̬
		GameStatus m_NextStatus;	// ��һ��״̬
		bool next = false;			// ��ʾ�û������� NextState(GameStatus state) ����
	};

}

