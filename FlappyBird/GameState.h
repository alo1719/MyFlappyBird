#pragma once

#include <memory>
#include "Graphics.h"
#include "BaseStateMachine.h"
#include "MusicManager.h"

template<typename Type> class ThreadPool;

namespace games
{
    class Sprite;
    class Animation;

    // ��Ϸ״̬
    enum GAMESTATE
    {
        GAMESTATE_READY,   // ׼��
        GAMESTATE_PLAYING, // ��Ϸ����
        GAMESTATE_DIYING,  // ����
        GAMESTATE_DIED     // ����
    };

    // ��¼��ײĿ��
    enum COLLISIONRESULT
    {
        COLLISIONRESULT_NONE,         // ����ײ
        COLLISIONRESULT_TOP_PIPE,     // ���Ϸ�ˮ����ײ
        COLLISIONRESULT_BOTTOM_PIPE,  // ���·�ˮ����ײ
        COLLISIONRESULT_GROUND        // �������ײ
    };

    class GameState : public BaseStateMachine
    {
        struct Point 
        {
            int x;
            int y;
        };

        struct PipeData
        {
            Point pos;
            bool Passed;
            int Index;
        };
        
    private:

        GameState();
        virtual ~GameState();

    public:
        static GameState &instance();

        virtual void Run();
        virtual void OnRender();
        virtual void OnRenderUI();
        virtual void OnMouseEvent(MouseEvent event, int x, int y);
        virtual void OnKeywordEvent(KeywordEvent event, unsigned int state);

        void setAIEnable(bool state = true);
    private:
        void DrawMap();
        void DrawPipes();
        void DrawPath();
        void DrawBrid();
        void DrawCollisionBox();
        
        void initialize();
        void initBirdPosition();
        void Begin();
        void Restart();
        void Died();

        void FloatBird(double ElapsedTime);
        void UpdateBird(double ElapsedTime);
        void UpdatePipes(double ElapsedTime);  // ����ˮ�ܵ�λ��
        void UpdateGround(double ElapsedTime); // ���µذ��λ��
        void UpdatePathLogger(double ElapsedTime); // ����·����¼��

        void CreatePipe();
        COLLISIONRESULT CheckCollision(int &tPipeIndex);

        void CheckScore();
        void PassPipe();

        void GetBirdState(float &horizontal, float &vertical, float &ground_distance);

        void PlaySound(Music *music);
    private:
        std::wstring imgBackground;
        std::wstring imgGround;
        std::wstring imgPipeTop;
        std::wstring imgPipeBottom;
        std::wstring imgGetReady;
        std::wstring imgGamesOver;

        std::shared_ptr<Sprite> birds;
        std::shared_ptr<Animation> fly;

        // �������Ϣ
        Point birdPos;
        int birdWidth;
        int birdHeight;
        float birdV;
        float birdRotation;
        D2D1_SIZE_F birdSize;

        float groundOffset;

        int pipeIndex;
        int pipeTimer;
        std::vector<PipeData> pipes;

        float birdPathLoggerTime;     // ·����¼������
        std::vector<std::pair<float, Point>> birdPath;

        // 
        bool keyDown;

        int score;

        // 
        COLLISIONRESULT collisionResult;

        bool died;
        GAMESTATE gameState;
        FontHandle tipsFont;
        FontHandle scoreFont;

        D2D1_RECT_F getReadImageSize;
        D2D1_RECT_F gameOverImageSize;

        Music *music_die;
        Music *music_hit;
        Music *music_wing;
        Music *music_point;
        MusicManager * pMusicManager;	//���ֹ������ָ��
    };

}