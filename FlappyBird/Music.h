#pragma once

#include <process.h>
#include <string>

class Music
{
protected:
    friend class MusicManager;

    int	m_ID;				    //��ţ������ж��Ƿ���ͬһ������
    static int m_nNewID;		//��һ�����ñ��

    std::wstring filepath;	    //Ϊ""��ʾ���ı�·��������ı������ļ�·��
    int Volume;	                //������Ϊ������ʾ���ı�
    long StartPosition;	        //���ֿ�ʼλ�ã�Ϊ������ʾ���ı�
    long EndPosition;	        //���ֽ���λ�ã�Ϊ������ʾ���ı�
    int NowPosition;		    //���ڼ�¼��ͣʱ������λ��

    bool StopPlayFlag;	        //��ͣ���ŵı�־
    bool RepeatFlag;	        //ѭ�����ŵı�־
    bool KillFocusFlag;		    //ʧȥ�����־
    bool SetFocusFlag;		    //��ȡ�����־

                                //��־λ�����ڻ�ȡ����״̬
    bool Playing;	            //�Ƿ����ڲ���
    bool Stopped;	            //�Ƿ�ֹͣ
    bool Opened;	            //�Ƿ���������ļ�

public:
    Music();
    Music(std::wstring filepath, bool repeat = false);
    ~Music();

    ////////////////////////////////////////////////////////////////////////////////////////////
    /*�����߳�ʹ�õķ���*/
    //��ȡ���
    int GetID();

    //��ȡ����״̬
    bool IsPlaying();
    bool IsStopped();
    bool IsOpened();

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////
    /*�������߳�ʹ�õķ���*/
    bool GetPlayState();    //��ȡ���ֲ���״̬
    bool GetOpenState();    //��ȡ�����ļ���״̬
    bool GetStopState();    //��ȡ����ֹͣ״̬
    long GetNowPosition();  //��ȡ���ֵ�ǰλ��

    bool Open(std::wstring filepath);
    bool Close();
    void SetVolume(int nVolume);
    bool Stop();
    bool Play(bool ReStart, long from, long to);

    //ֻ�����ַ�������Ĳ��ŷ�ʽ
    bool Play(std::wstring Command);

    static bool SendToMCI(std::wstring command);        //������Ϣ��MCI
    static std::wstring GetFromMCI(std::wstring command);//��MCI��ȡ��Ϣ
};

