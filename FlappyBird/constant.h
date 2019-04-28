#pragma once


// ��������
#define GAMES_SIZE_W 800
#define GAMES_SIZE_H 600

#define MS_GROUNDWIDTH      336
#define MS_GROUNDHEIGHT     112
#define MS_GROUNDDRAWCOUNT  (GAMES_SIZE_W / MS_GROUNDWIDTH + 1)
#define MS_PIPEWIDTH        52
#define MS_PIPEHEIGHT       420
#define MS_BIRDIMGSIZE_W    40
#define MS_BIRDIMGSIZE_H    40

#define MS_STAGEMOVESPEED 275   // �ƶ��ٶ�, Ĭ��150
#define MS_PIPECREATETIME 1.2   // ���Ӵ���ʱ����, Ĭ��1.7
#define MS_PIPEGAPHEIGHT  100   // ���Ӽ�϶�߶�, Ĭ��100
#define MS_PIPECREATEYMINRESERVED  10.f
#define MS_PIPECREATEYMAXRESERVED  50.f
#define MS_PIPECREATEYMIN (GAMES_SIZE_H - MS_PIPEHEIGHT - (MS_PIPEGAPHEIGHT / 2.f) + MS_PIPECREATEYMINRESERVED) // ���Ӽ�϶������СYֵ
#define MS_PIPECREATEYMAX (MS_PIPEHEIGHT + (MS_PIPEGAPHEIGHT / 2.f) - MS_PIPECREATEYMAXRESERVED)              // ���Ӽ�϶�������Yֵ

#define MS_BIRDCENTERX  200.f            // ���ˮƽλ��
#define MS_BIRDBOUNDINGCIRCLESIZE 6.f   // ��İ�ΧԲ�뾶, Ĭ��10
#define MS_GRAVITY  980.f                // �������ٶ�
#define MS_BIRDJUMPV 300.f               // ���°���������ٶ�, Ĭ��360
#define MS_BIRDMAXV  600.f               // �������ٶ�
#define MS_BIRDDROPV 400.f               // ��ת����ٶ�
#define MS_BIRD_BEGIN_DROP 250.F         // ��ʼ��ת�ٶ�

#define MS_BIRDMAXROTATION (3.1415926f / 12.f)  // ���ϵ����Ƕ�, Ĭ��/8
#define MS_BIRDMINROTATION (3.1415926f / 12.f)   // ���µ����Ƕ�, Ĭ��/2
#define MS_BIRDROTATIONWEIGHT   1.25f        // ��ת�Ĳ�ֵȨ��

#define MS_PATHLOGTIME          0.06f         // ·����¼ʱ����, Ĭ��0.1
#define MS_PATHFADEOUTTIME      2.5f         // ·������ʧʱ��, Ĭ��2.5
#define MS_PATHSTARTFADEOUTTIME 1.5f         // ·���㿪ʼ��ʧʱ��, Ĭ��1.5
#define MS_PATHCOLOR            0xFFFBE256   // ·������ɫ
#define MS_PATHSIZE             3.f          // ·�����С