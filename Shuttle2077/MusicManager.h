#pragma once

#include "Music.h"
#include <vector>
#include <queue>

//���ֲ��Ų������������̶߳������̵߳�ͨ��
typedef struct tagMusicParam {
    int ID;		        //Ҫִ����������ֶ���ID

                        //���ȼ����
    bool Delete;	    //Ҫ��ɾ�����ֶ���
    bool Exit;		    //Ҫ�������߳��˳�

                        //���ȼ��ڶ�
    std::wstring filepath;	//Ϊ""��ʾ���ı�·��������ı������ļ�·��
    int Volume;	        //������Ϊ������ʾ���ı�
    long StartPosition;	//���ֿ�ʼλ�ã�Ϊ������ʾ���ı�
    long EndPosition;	//���ֽ���λ�ã�Ϊ������ʾ���ı�
    bool EnableRepeat;	//��Ϊtrueʱ����Repeat�������ֵ�ѭ�����ű�־
    bool Repeat;	    //�������ֵ�ѭ�����ű�־

                        //���ȼ����
    bool ReStart;	    //Ϊtrue��ʾҪ���ط�
    bool Play;		    //Ϊtrue��ʾҪ�󲥷�һ�Σ���������ֹͣʱ��Ч
    bool Stop;		    //Ϊtrue��ʾҪ��ֹͣ

                        //��Ϸ����ʧȥ���㡢���»�ý���ı�־
    bool KillFocus;
    bool SetFocus;

    tagMusicParam()
    {
        this->ID = -1;
        this->Delete = false;
        this->Exit = false;
        this->filepath = L"";
        this->Volume = -1;
        this->StartPosition = -1;
        this->EndPosition = -1;
        this->EnableRepeat = false;
        this->Repeat = false;
        this->ReStart = false;
        this->Play = false;
        this->Stop = false;
        this->KillFocus = false;
        this->SetFocus = false;
    }

}MusicParam;

//�������߳̽���ͨ�ŵ���Ϣ����
static std::queue<MusicParam> g_MusicMessageQueue;

//�����̵߳ľ��
static HANDLE g_hThreadForMusic;

////////////////////////////////////////////////////////////////////////////////////////////////////////
class MusicManager
{
protected:
    std::vector<Music*> vMusic;	//��������
    static MusicManager* Mmanager;	//ָ���Լ���ָ��
    bool RunFlag;	//�ж������߳��Ƿ�����������Լ���������ַ���

public:
    MusicManager();
    ~MusicManager();

    ////////////////////////////////////////////////////////////////////////////////////////////
    //�������֣�ֻ���������߳�δ��ʼ�������ʹ��
    bool AddMusic(Music* music);

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////
    //�̲߳ſɽ��еĲ���
    //�ͷ���Դ
    void Release(bool Del = false);

    //ɾ��ָ��ID������
    void DeleteMusic(Music* music, bool Del);

    //ѹ������
    void Compress();

    //����ID���������Ƿ���ڣ����ھͷ���ָ�룬���򷵻�NULL
    Music* FindMusic(int ID);

    //��ȡָ��ö����ָ��
    static MusicManager* GetPointer() {
        return Mmanager;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //���ص��߳�ѭ���еķ���
    void Run(MusicParam MP, bool Change = false);

    //��ʼ������
    void Initialize();

    //����
    void Play(Music* music, bool Play, bool ReStart, bool Stop);

    //�޸Ĳ���
    void Change(Music* music, MusicParam MP);

    //����״̬
    void SetStatus(Music* music);

public:
    ////////////////////////////////////////////////////////////////////////////////////////////
    //�����̵߳ķ���
    static unsigned __stdcall ThreadForPlayMusic(LPVOID lpParameter)
    {
        MusicManager* g_pMmanager = MusicManager::GetPointer();
        g_pMmanager->RunFlag = true;
        g_pMmanager->Initialize();
        while (true)
        {
            Sleep(1);
            MusicParam MP;
            if (g_MusicMessageQueue.empty() != true)
            {
                MP = g_MusicMessageQueue.front();
                g_MusicMessageQueue.pop();
                if (MP.Exit == true)
                    break;
                g_pMmanager->Run(MP, true);
            }
            else
                g_pMmanager->Run(MP);
        }
        delete g_pMmanager;		//�˳�ʱ�̻߳�����������ص�������Դ
        return 0;
    }
};
