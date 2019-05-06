#include "main.h"
#include "GameState.h"

#include "Graphics.h"
#include "Animation.h"
#include "Sprite.h"
#include "Ulit.h"
#include "Application.h"
#include "TimeCounter.h"
#include "Collision.h"
#include "Statistics.h"
#include "StateMachine.h"
#include "constant.h"
#include "ThreadPool.h"

using games::KeywordEvent;

namespace games
{
	// 返回自上次调用TimeTick后经过的时间
    double TimeTick() 
    {
        static double last = TimeCounter::instance().getTick();
        double now = TimeCounter::instance().getTick();
        double use = (now - last) / 1000;
        last = now;
        return use;
    }

    void GameState::PlaySound(Music *music)
    {
        MusicParam MP;
        MP.ID = music->GetID();
        MP.StartPosition = 0;
        MP.Play = true;
		// g_MusicMessageQueue为与音乐线程进行通信的消息队列
        g_MusicMessageQueue.push(MP);
    }

    GameState::GameState()
    {
        tipsFont = Graphics::Instance().CreateFontObject(L"微软雅黑", 14);
        Graphics::Instance().FontCenter(tipsFont);
        scoreFont = Graphics::Instance().CreateFontObject(TEXT("微软雅黑"), 26.f);
        Graphics::Instance().FontCenter(scoreFont);

        initialize();
    }

    GameState::~GameState()
    {
        Graphics::Instance().DestroyFontObject(tipsFont);
        Graphics::Instance().DestroyFontObject(scoreFont);

        // 通知子线程退出
        MusicParam MP;
        MP.Exit = true;
        g_MusicMessageQueue.push(MP);

		// 等待子线程退出
        WaitForSingleObject(g_hThreadForMusic, INFINITE);
    }

    GameState & GameState::instance()
    {
        static GameState game;
        return game;
    }

	// 初始化游戏内容
    void GameState::initialize()
    {
        std::wstring &path = GetAppPath();

        std::wstring soundDie = path + L"Resource\\sounds\\die.wav";
        std::wstring soundHit = path + L"Resource\\Sounds\\hit.wav";
        std::wstring soundWing = path + L"Resource\\sounds\\wing.wav";
        std::wstring soundPoint = path + L"Resource\\sounds\\point.wav";

		// 初始化音乐管理对象
        pMusicManager = new MusicManager();

        music_die = new Music(soundDie);
        music_hit = new Music(soundHit);
        music_wing = new Music(soundWing);
        music_point = new Music(soundPoint);

        pMusicManager->AddMusic(music_die);
        pMusicManager->AddMusic(music_hit);
        pMusicManager->AddMusic(music_wing);
        pMusicManager->AddMusic(music_point);

		// 创建线程
        g_hThreadForMusic = (HANDLE)_beginthreadex(NULL, 0, pMusicManager->ThreadForPlayMusic, NULL, 0, NULL);

        // 初始化素材部分
        imgBackground = path + L"Resource\\background.png";
        imgGround = path + L"Resource\\Ground.png";
        imgPipeTop = path + L"Resource\\pipe_top.png";
		imgPipeTop2 = path + L"Resource\\pipe_top2.png";
		imgPipeTop3 = path + L"Resource\\pipe_top3.png";
		imgPipeTop4 = path + L"Resource\\pipe_top4.png";
        imgPipeBottom = path + L"Resource\\pipe_bottom.png";
		imgPipeBottom2 = path + L"Resource\\pipe_bottom2.png";
		imgPipeBottom3 = path + L"Resource\\pipe_bottom3.png";
		imgPipeBottom4 = path + L"Resource\\pipe_bottom4.png";
        imgGetReady = path + L"Resource\\ready.png";
        imgGamesOver = path + L"Resource\\gamesover.png";

        D2D1_SIZE_F size;
        Graphics::Instance().GetBitmapSize(imgGetReady, size);

        getReadImageSize = D2D1::RectF(
            (GAMES_SIZE_W - size.width) / 2,
            (GAMES_SIZE_H - size.height) / 2,
            (GAMES_SIZE_W + size.width) / 2,
            (GAMES_SIZE_H + size.height) / 2
        );

        Graphics::Instance().GetBitmapSize(imgGamesOver, size);

        gameOverImageSize = {
            (GAMES_SIZE_W - size.width) / 2,
            (GAMES_SIZE_H - size.height - 100) / 2,
            (GAMES_SIZE_W + size.width) / 2,
            (GAMES_SIZE_H + size.height - 100) / 2,
        };

        // 初始化动画
        fly = std::make_shared<Animation>();
        for (int i = 0; i < 4; ++i)
        {
            TCHAR filename[MAX_PATH];
            wsprintf(filename, L"%sResource\\Shuttle\\%d.png", path.c_str(), i);
            fly->addSpriteFrameWithFileName(filename);
        }
        fly->setRestoreOriginalFrame(true);
        fly->setDelayPerUnit(480);

        // 初始化Sprite
        std::wstring birdImage = path + L"Resource\\Shuttle\\0.png";
        Graphics::Instance().GetBitmapSize(birdImage, birdSize);
        birds = std::make_shared<Sprite>();
        birds->runAction(fly.get());

        Restart();
    }

	// 初始化位置
    void GameState::initBirdPosition()
    {
        birdHeight = birdSize.height;
        birdWidth = birdSize.width;
        birdPos.x = MS_BIRDCENTERX;
        birdPos.y = (GAMES_SIZE_H - MS_GROUNDHEIGHT) / 2.f;
        int left = birdSize.width / 2, top = birdSize.height / 2;
        birds->setPosition(
            birdPos.x - left,
            birdPos.y - top,
            birdPos.x + left,
            birdPos.y + top);
    }

    void GameState::Begin()
    {
		// 是否按下按键
        keyDown = false;

        pipeIndex = 0;
        pipeTimer = TimeCounter::instance().Schedule(
            1, MS_PIPECREATETIME * 850, [this] { CreatePipe(); });

		birdSpeed = MS_STAGEMOVESPEED;
        birdV = 0.f;
		isDashing = false;
        birdPathLoggerTime = 0.f;
		gravityV = MS_GRAVITY;

        birds->runAction(fly.get());
    }

    void GameState::CreatePipe()
    {
        PipeData tData;

        int y = rand() % (int)(MS_PIPECREATEYMAX - MS_PIPECREATEYMIN) + MS_PIPECREATEYMIN;

        // 计算空隙中心创建位置
        ++pipeIndex;
        tData.pos.x = GAMES_SIZE_W + MS_PIPEWIDTH / 2.f;
        tData.pos.y = y;
        tData.Index = pipeIndex;
        tData.Passed = false;

        pipes.push_back(tData);
    }

    void GameState::Restart()
    {
        score = 0;

		birdSpeed = MS_STAGEMOVESPEED;
        birdV = 0.f;
		isDashing = false;
		gravityV = MS_GRAVITY;
        initBirdPosition();
        birds->setRotation(0.f, D2D1::Point2F(birdPos.x, birdPos.y));

        birdPathLoggerTime = 0.f;
        birdPath.clear();

        pipeIndex = 0;
        pipes.clear();

        gameState = GAMESTATE_READY;
        groundOffset = 0.f;
        TimeCounter::instance().Release(pipeTimer);
    }

    void GameState::Died()
    {
        fly->end();
		// 显示上次得分
        Statistics::instance().LastScore(score);
    }

    void GameState::Run()
    {
        float ElapsedTime = TimeTick();
        switch (gameState)
        {
        case games::GAMESTATE_READY:
            FloatBird(ElapsedTime);
            break;
        case games::GAMESTATE_PLAYING:
        {
			// 更新4大组件
            UpdateBird(ElapsedTime);
            UpdatePipes(ElapsedTime);
            UpdateGround(ElapsedTime);
            UpdatePathLogger(ElapsedTime);

            // 经过Pipe检测
            CheckScore();

            // 碰撞检测
            int tPipeIndex;
            collisionResult = CheckCollision(tPipeIndex);

            switch (collisionResult)
            {
            case COLLISIONRESULT_TOP_PIPE:
            case COLLISIONRESULT_BOTTOM_PIPE:
                PlaySound(music_hit);
                PlaySound(music_die);
                gameState = GAMESTATE_DIYING;
                break;
			// 撞到地板直接死亡并结束游戏
            case COLLISIONRESULT_GROUND:
                PlaySound(music_hit);
                gameState = GAMESTATE_DIED;
                Died();
                break;
            }
            break;
        }
        case games::GAMESTATE_DIYING:
        {
			// 创建小鸟位置的椭圆
            D2D1_ELLIPSE ellipse{
                { birdPos.x, birdPos.y },
                MS_BIRDBOUNDINGCIRCLESIZE,
                MS_BIRDBOUNDINGCIRCLESIZE };

            // 检查与地面的碰撞
            D2D1_POINT_2F p1{ 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
                p2{ GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
            if (Collision::CircleLineHitTest(ellipse, p1, p2))
            {
                gameState = GAMESTATE_DIED;
                Died();
            }
            else
            {
                PlaySound(music_hit);
				birdV = 600;
				gravityV = MS_GRAVITY;
                UpdateBird(ElapsedTime);
            }
            break;
        }
        case games::GAMESTATE_DIED:
            break;
        default:
            break;
        }
    }

    void GameState::OnRender()
    {
		// 绘制4大组件
        DrawMap();
        DrawPipes();
        DrawPath();
        DrawBrid();

// 只有在Debug模式下才绘制碰撞框
#ifdef _DEBUG
        DrawCollisionBox();
#endif
    }

    void GameState::OnRenderUI()
    {
		// 绘制主界面分数
        TCHAR temp[32] = { 0 };
        wsprintf(temp, TEXT("%d"), score);
        Graphics::Instance().DrawText(
            temp, 
            lstrlenW(temp), 
            scoreFont, 
			// RectF参数为left, top, right, bottom
            D2D1::RectF(300, 0, 500, 100), 
            0xfcbdf6);

        if (gameState == GAMESTATE_READY)
        {
            Graphics::Instance().DrawBitmap(imgGetReady, getReadImageSize);
        }
        else if (gameState == GAMESTATE_DIED)
        {
            D2D1_RECT_F rect = gameOverImageSize;
            Graphics::Instance().DrawBitmap(imgGamesOver, rect);

			// 在图像下方绘制提示字体
            rect.top = rect.bottom + 10;
            rect.bottom += 40;
            wstring msg = L"按向上/向下键继续";
            Graphics::Instance().DrawText(
                msg.c_str(), msg.length(), tipsFont, rect, 0);
        }
    }

    void GameState::OnMouseEvent(MouseEvent event, int x, int y)
    {
    }

    void GameState::OnKeywordEvent(KeywordEvent event, unsigned int state)
    {
        if (gameState == GAMESTATE_PLAYING)
        {
			// 按下了向上键
            if (event == KeywordEvent::KEYDOWN && state == VK_UP)
            {
                if (!keyDown && gravityV > 0)
                {
                    PlaySound(music_wing);
					gravityV = -gravityV;
                    keyDown = true;
                }
            }
			// 按下了向下键
			else if (event == KeywordEvent::KEYDOWN && state == VK_DOWN)
			{
				if (!keyDown && gravityV < 0)
				{
					PlaySound(music_wing);
					gravityV = -gravityV;
					keyDown = true;
				}
			}
			// 按下Shift键
			else if (event == KeywordEvent::KEYDOWN && state == VK_SHIFT)
			{
				if (gravityV < 0)
					gravityV = -2 * MS_GRAVITY;
				else
					gravityV = 2 * MS_GRAVITY;
				isDashing = true;
			}
			// 方向键弹起
			else if (event == KeywordEvent::KEYRUP && (state == VK_UP || state == VK_DOWN))
			{
				keyDown = false;
			}
			// Shift键弹起
			else if (event == KeywordEvent::KEYRUP && state == VK_SHIFT)
			{
				if (gravityV < 0)
					gravityV = -MS_GRAVITY;
				else
					gravityV = MS_GRAVITY;
				isDashing = false;
			}
        }
        else if (gameState == GAMESTATE_READY)
        {
            if (event == KeywordEvent::KEYDOWN && (state == VK_UP || state == VK_DOWN))
            {
                Begin();
                gameState = GAMESTATE_PLAYING;
            }
        }
        else if (gameState == GAMESTATE_DIED)
        {
            if (event == KeywordEvent::KEYDOWN && (state == VK_UP || state == VK_DOWN))
            {
				// 显示计分板并重置游戏
                StateMachine::Instance().NextState(GameStatus::UI_STATISTICS);
                Restart();
            }
        }
    }

    void GameState::DrawMap()
    {
        D2D1_RECT_F rect{ 0, 0, 800, 600 };
        Graphics::Instance().DrawBitmap(imgBackground, rect);

        // 绘制地板
        for (int i = 0; i <= MS_GROUNDDRAWCOUNT; ++i)
        {
            D2D1_RECT_F ground { 
                MS_GROUNDWIDTH * i - groundOffset,
                GAMES_SIZE_H - MS_GROUNDHEIGHT,
                MS_GROUNDWIDTH * (i + 1) - groundOffset, 
                GAMES_SIZE_H
            };
            Graphics::Instance().DrawBitmap(imgGround, ground);
        }
    }

    void GameState::DrawPipes()
    {
        // 绘制管子
        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F topScreen, topImage, bottomScreen, bottomImage;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                topScreen.left = 0;
                topImage.left = MS_PIPEWIDTH / 2 - i->pos.x;
                bottomScreen.left = 0;
                bottomImage.left = MS_PIPEWIDTH / 2 - i->pos.x;
            }
            else
            {
                topScreen.left = i->pos.x - MS_PIPEWIDTH / 2;
                topImage.left = 0;
                bottomScreen.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottomImage.left = 0;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                topScreen.right = GAMES_SIZE_W;
                topImage.right = MS_PIPEWIDTH - i->pos.x - MS_PIPEWIDTH / 2 + GAMES_SIZE_W;
                bottomScreen.right = GAMES_SIZE_W;
                bottomImage.right = MS_PIPEWIDTH - i->pos.x - MS_PIPEWIDTH / 2 + GAMES_SIZE_W;
            }
            else
            {
                topScreen.right = i->pos.x + MS_PIPEWIDTH / 2;
                topImage.right = MS_PIPEWIDTH;
                bottomScreen.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottomImage.right = MS_PIPEWIDTH;
            }

			topScreen.top = 0;
			topScreen.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;
			topImage.top = MS_PIPEHEIGHT - topScreen.bottom;
			topImage.bottom = MS_PIPEHEIGHT;
			bottomScreen.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
			bottomScreen.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;
			bottomImage.top = 0;
			bottomImage.bottom = bottomScreen.bottom - bottomScreen.top;

			if (i->Index % 2 == 1)
			{
				if (i->pos.y % 4 == 0)
					Graphics::Instance().DrawBitmap(imgPipeTop, topScreen, topImage);
				else if (i->pos.y % 4 == 1)
					Graphics::Instance().DrawBitmap(imgPipeTop2, topScreen, topImage);
				else if (i->pos.y % 4 == 2)
					Graphics::Instance().DrawBitmap(imgPipeTop3, topScreen, topImage);
				else
					Graphics::Instance().DrawBitmap(imgPipeTop4, topScreen, topImage);
			}
            
			if ((i->Index % 2 == 0) || ((i->Index % 5 == 0) && (i->Index > 10)) 
				|| ((i->Index % 3 == 0)&&(i->Index > 30)))
			{
				if (i->pos.y % 4 == 0)
					Graphics::Instance().DrawBitmap(imgPipeBottom, bottomScreen, bottomImage); 
				else if (i->pos.y % 4 == 1)
					Graphics::Instance().DrawBitmap(imgPipeBottom2, bottomScreen, bottomImage); 
				else if (i->pos.y % 4 == 2)
					Graphics::Instance().DrawBitmap(imgPipeBottom3, bottomScreen, bottomImage);
				else
					Graphics::Instance().DrawBitmap(imgPipeBottom4, bottomScreen, bottomImage);
			}
        }
    }

    void GameState::DrawPath()
    {
        // 绘制鸟的路径
        for (auto i = birdPath.begin(); i != birdPath.end(); ++i)
        {   
            int tPathColor = MS_PATHCOLOR;
            int alpha = 1.f;
            if (i->first > MS_PATHSTARTFADEOUTTIME)
                alpha = ((i->first - MS_PATHSTARTFADEOUTTIME) / (MS_PATHFADEOUTTIME - MS_PATHSTARTFADEOUTTIME));

            D2D1_ELLIPSE ellipse{ i->second.x, i->second.y, MS_PATHSIZE, MS_PATHSIZE };
            Graphics::Instance().FillEllipse(ellipse, tPathColor, alpha);
        }
    }

    void GameState::DrawBrid()
    {
        birds->draw();
    }

    void GameState::DrawCollisionBox()
    {
        D2D1_ELLIPSE ellipse{
            { birdPos.x, birdPos.y },
            MS_BIRDBOUNDINGCIRCLESIZE,
            MS_BIRDBOUNDINGCIRCLESIZE };

        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F top, bottom;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                top.left = 0;
                bottom.left = 0;
            }
            else
            {
                top.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottom.left = i->pos.x - MS_PIPEWIDTH / 2;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                top.right = GAMES_SIZE_W;
                bottom.right = GAMES_SIZE_W;
            }
            else
            {
                top.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottom.right = i->pos.x + MS_PIPEWIDTH / 2;
            }

            top.top = 0;
            top.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;

            bottom.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
            bottom.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;

			if (i->Index & 1 == 1)
			{
				Graphics::Instance().DrawRectangle(top, 0xff0000);
			}

			if ((i->Index % 2 == 0) || ((i->Index % 5 == 0) && (i->Index > 10))
				|| ((i->Index % 3 == 0) && (i->Index > 30)))
			{
				Graphics::Instance().DrawRectangle(bottom, 0xff0000);
			}
        }

		// 绘制地板碰撞盒
        D2D1_POINT_2F p1{ 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
            p2{ GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
        Graphics::Instance().DrawLine(p1, p2, 0xff0000);

		// 绘制小鸟碰撞圆
        Graphics::Instance().DrawEllipse(ellipse, 0xff0000);
    }

	// 使小鸟上下浮动
    void GameState::FloatBird(double ElapsedTime)
    {
        static int count = 0, step = 1;
        static float time = 0.f;

        time += ElapsedTime;
        if (time > 0.05f)
        {
            time = 0.f;
            birdPos.y += step;
            birds->setPosition(
                birdPos.x - birdWidth / 2,
                birdPos.y - birdHeight / 2,
                birdPos.x + birdWidth / 2,
                birdPos.y + birdHeight / 2);

            if (++count >= 10)
            {
                step = -step;
                count = 0;
            }
        }
    }

    void GameState::UpdateBird(double ElapsedTime)
    {
        // 改变鸟的垂直位置
		birdPos.y += birdV * ElapsedTime;
		// 禁止越界
        if (birdPos.y - MS_BIRDBOUNDINGCIRCLESIZE < 0.f)
            birdPos.y = MS_BIRDBOUNDINGCIRCLESIZE;
        if (birdPos.y + MS_GROUNDHEIGHT > GAMES_SIZE_H)
            birdPos.y = GAMES_SIZE_H - MS_GROUNDHEIGHT;
        birdV += gravityV * ElapsedTime;
		D2D1_POINT_2F p1{ 0, 0 },
			p2{ GAMES_SIZE_W, 0 };
		D2D1_ELLIPSE ellipse{
			{ birdPos.x, birdPos.y },
			MS_BIRDBOUNDINGCIRCLESIZE,
			MS_BIRDBOUNDINGCIRCLESIZE };
		if (Collision::CircleLineHitTest(ellipse, p1, p2))
		{
			// 撞顶之后反弹
			birdV = -birdV;
			birdPos.y += 3;
		}
		// 限制最大速度
		if (!isDashing)
		{
			if (birdV >= MS_BIRDMAXV)
				birdV = MS_BIRDMAXV;
			if (birdV <= -MS_BIRDMAXV)
				birdV = -MS_BIRDMAXV;
		}
		else
		{
			if (birdV >= 1.8 * MS_BIRDMAXV)
				birdV = 1.8 * MS_BIRDMAXV;
			if (birdV <= -1.8 * MS_BIRDMAXV)
				birdV = -1.8 * MS_BIRDMAXV;
		}
        

        birds->setPosition(
            birdPos.x - birdWidth / 2, 
            birdPos.y - birdHeight / 2, 
            birdPos.x + birdWidth / 2, 
            birdPos.y + birdHeight / 2);

        // 计算鸟的旋转
        float tBirdTargetRotation = 0.f;
        if (birdV <= MS_BIRD_BEGIN_DROP)
            tBirdTargetRotation = -MS_BIRDMAXROTATION;
        else if (birdV > MS_BIRD_BEGIN_DROP && birdV < MS_BIRDDROPV)
            tBirdTargetRotation = birdV / MS_BIRDDROPV * (MS_BIRDMAXROTATION + MS_BIRDMINROTATION) - MS_BIRDMAXROTATION;
        else if (birdV >= MS_BIRDDROPV)
            tBirdTargetRotation = MS_BIRDMINROTATION;

        // 计算过渡旋转值(ease-out)
        birdRotation = (birdRotation * (MS_BIRDROTATIONWEIGHT - 1.f) + tBirdTargetRotation) / MS_BIRDROTATIONWEIGHT;
        birds->setRotation(birdRotation / 3.1415926f * 180, D2D1::Point2F(birdPos.x, birdPos.y));
    }

    void GameState::UpdatePipes(double ElapsedTime)
    {
        // 移动管子
        auto i = pipes.begin();
        while (i != pipes.end())
        {
            // 计算当前管子位置
            i->pos.x -= ElapsedTime * birdSpeed;

            // 移除越界管子
            if (i->pos.x + MS_PIPEWIDTH / 2.f < 0.f)
                i = pipes.erase(i);
            else
                ++i;
        }
    }

    void GameState::UpdateGround(double ElapsedTime)
    {
        // 移动地板
	    groundOffset = (float)fmod(groundOffset + ElapsedTime * birdSpeed, MS_GROUNDWIDTH);
    }

    void GameState::UpdatePathLogger(double ElapsedTime)
    {
        // 记录鸟的位置
        birdPathLoggerTime += ElapsedTime;
        if (birdPathLoggerTime > MS_PATHLOGTIME)
        {
            birdPath.push_back(std::pair<float, Point>(0.f, birdPos));

            birdPathLoggerTime = 0.f;
        }

        // 更新鸟的路径点
        auto i = birdPath.begin();
        while (i != birdPath.end())
        {
            i->first += ElapsedTime;
            i->second.x -= birdSpeed * ElapsedTime;

            if (i->first > MS_PATHFADEOUTTIME)
                i = birdPath.erase(i);
            else
                ++i;
        }
    }

    COLLISIONRESULT GameState::CheckCollision(int &PipeIndex)
    {
        PipeIndex = -1;
        D2D1_ELLIPSE ellipse{
            { birdPos.x, birdPos.y },
            MS_BIRDBOUNDINGCIRCLESIZE,
            MS_BIRDBOUNDINGCIRCLESIZE };

        // 检查与水管的碰撞
	    for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            // 计算上下水管绘制位置
            D2D1_RECT_F top, bottom;
            if (i->pos.x - MS_PIPEWIDTH / 2 < 0.f)
            {
                top.left = 0;
                bottom.left = 0;
            }
            else
            {
                top.left = i->pos.x - MS_PIPEWIDTH / 2;
                bottom.left = i->pos.x - MS_PIPEWIDTH / 2;
            }

            if (i->pos.x + MS_PIPEWIDTH / 2 > GAMES_SIZE_W)
            {
                top.right = GAMES_SIZE_W;
                bottom.right = GAMES_SIZE_W;
            }
            else
            {
                top.right = i->pos.x + MS_PIPEWIDTH / 2;
                bottom.right = i->pos.x + MS_PIPEWIDTH / 2;
            }

            top.top = 0;
            top.bottom = i->pos.y - MS_PIPEGAPHEIGHT / 2.f;

            bottom.top = i->pos.y + MS_PIPEGAPHEIGHT / 2.f;
            bottom.bottom = GAMES_SIZE_H - MS_GROUNDHEIGHT;

            // 进行顶部碰撞检测
			if (i->Index & 1 == 1)
			{
				if (Collision::OBBCircleHitTest(top, ellipse))
				{
					PipeIndex = i->Index;
					return COLLISIONRESULT_TOP_PIPE;
				}
			}
            
			// 进行底部碰撞检测
			if ((i->Index % 2 == 0) || ((i->Index % 5 == 0) && (i->Index > 10))
				|| ((i->Index % 3 == 0) && (i->Index > 30)))
			{
				if (Collision::OBBCircleHitTest(bottom, ellipse))
				{
					PipeIndex = i->Index;
					return COLLISIONRESULT_BOTTOM_PIPE;
				}
			}
        }

        // 检查与地面的碰撞
        D2D1_POINT_2F p1 { 0, GAMES_SIZE_H - MS_GROUNDHEIGHT },
            p2 { GAMES_SIZE_W, GAMES_SIZE_H - MS_GROUNDHEIGHT };
        if (Collision::CircleLineHitTest(ellipse, p1, p2))
        {
            return COLLISIONRESULT_GROUND;
        }
        return COLLISIONRESULT_NONE;
    }

    void GameState::CheckScore()
    {
        for (auto i = pipes.begin(); i != pipes.end(); i++)
        {
            if (birdPos.x > i->pos.x && i->Passed == false)
            {
                i->Passed = true;
                PassPipe();
            }
        }
    }

    void GameState::PassPipe()
    {
        score += 5;
		if (score % 25 == 0)
			PlaySound(music_point);
    }

    void GameState::GetBirdState(float & horizontal, float & vertical, float & ground_distance)
    {
        for (auto i = pipes.begin(); i != pipes.end(); ++i)
        {
            if (i->pos.x + (MS_PIPEWIDTH >> 1) - birdPos.x < 0)
                continue;

            horizontal = i->pos.x + (MS_PIPEWIDTH >> 1) - birdPos.x;
            vertical = i->pos.y - birdPos.y;
            ground_distance = GAMES_SIZE_H - MS_GROUNDHEIGHT - birdPos.y;
            break;
        }
    }

}
