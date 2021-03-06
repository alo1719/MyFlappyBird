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

    // 游戏状态
    enum GAMESTATE
    {
        GAMESTATE_READY,   // 准备
        GAMESTATE_PLAYING, // 游戏进行
        GAMESTATE_DIYING,  // 垂死
        GAMESTATE_DIED     // 死亡
    };

    // 记录碰撞目标
    enum COLLISIONRESULT
    {
        COLLISIONRESULT_NONE,         // 无碰撞
        COLLISIONRESULT_TOP_PIPE,     // 与上方水管碰撞
        COLLISIONRESULT_BOTTOM_PIPE,  // 与下方水管碰撞
        COLLISIONRESULT_GROUND        // 与地面碰撞
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
        void UpdatePipes(double ElapsedTime);  // 更新水管的位置
        void UpdateGround(double ElapsedTime); // 更新地板的位置
        void UpdatePathLogger(double ElapsedTime); // 更新路径记录器

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
		std::wstring imgPipeTop2;
		std::wstring imgPipeTop3;
		std::wstring imgPipeTop4;
        std::wstring imgPipeBottom;
		std::wstring imgPipeBottom2;
		std::wstring imgPipeBottom3;
		std::wstring imgPipeBottom4;
        std::wstring imgGetReady;
        std::wstring imgGamesOver;

        std::shared_ptr<Sprite> birds;
        std::shared_ptr<Animation> fly;

        // 鸟相关信息
        Point birdPos;
        int birdWidth;
        int birdHeight;
		int birdSpeed;
        float birdV;
        float birdRotation;
		float gravityV;
		bool isDashing;
        D2D1_SIZE_F birdSize;

        float groundOffset;

        int pipeIndex;
        int pipeTimer;
        std::vector<PipeData> pipes;

        float birdPathLoggerTime;     // 路径记录计数器
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
        MusicManager * pMusicManager;	//音乐管理对象指针
    };

}