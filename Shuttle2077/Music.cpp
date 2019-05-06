#include "main.h"
#include "Music.h"
#include <sstream>
#include <Mmsystem.h>

#pragma comment(lib,"winmm.lib")

int Music::m_nNewID = 1;		//��ʼ����һ�����ñ��

Music::Music()
{
    this->m_ID = m_nNewID;
    this->m_nNewID++;

    this->filepath = L"";
    this->Volume = 500;
    this->StartPosition = 0;
    this->EndPosition = 0;
    this->NowPosition = 0;

    this->RepeatFlag = false;
    this->StopPlayFlag = false;
    this->KillFocusFlag = false;
    this->SetFocusFlag = false;

    this->Playing = false;
    this->Stopped = false;
    this->Opened = false;
}

Music::Music(std::wstring filepath, bool repeat)
{
    this->m_ID = m_nNewID;
    this->m_nNewID++;

    this->filepath = filepath;
    this->Volume = 500;
    this->StartPosition = 0;
    this->EndPosition = 0;
    this->NowPosition = 0;

    this->RepeatFlag = false;
    this->StopPlayFlag = false;
    this->KillFocusFlag = false;
    this->SetFocusFlag = false;

    this->Playing = false;
    this->Stopped = false;
    this->Opened = false;
}

Music::~Music()
{
    this->Close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ���
int Music::GetID()
{
    return this->m_ID;
}

//��ȡ����״̬
bool Music::IsPlaying()
{
    return this->Playing;
}
bool Music::IsStopped()
{
    return this->Stopped;
}
bool Music::IsOpened()
{
    return this->Opened;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ���ֲ���״̬
bool Music::GetPlayState()
{
    std::wstring szStatus;
    wchar_t szCommand[MAX_PATH];

    wsprintf(szCommand, L"status MUSIC%d mode", this->m_ID);	//����MCI�����ַ���
    szStatus = Music::GetFromMCI(szCommand);
    if (szStatus == L"playing")
        return true;
    return false;
}

//��ȡ�����ļ���״̬
bool Music::GetOpenState()
{
    std::wstring szStatus;
    wchar_t szCommand[MAX_PATH];

    wsprintf(szCommand, L"status MUSIC%d mode", this->m_ID);
    szStatus = Music::GetFromMCI(szCommand);
    if (szStatus == L"stopped" || szStatus == L"playing")
        return true;
    return false;
}

//��ȡֹͣ״̬
bool Music::GetStopState()
{
    std::wstring szStatus;
    wchar_t szCommand[MAX_PATH];

    wsprintf(szCommand, L"status MUSIC%d mode", this->m_ID);
    szStatus = Music::GetFromMCI(szCommand);
    if (szStatus == L"stopped")
        return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ���ֵ�ǰλ��
long Music::GetNowPosition()
{
    long position;
    std::wstring szStatus;
    wchar_t szCommand[MAX_PATH];
    wsprintf(szCommand, L"status MUSIC%d position", this->m_ID);
    szStatus = Music::GetFromMCI(szCommand);
    if (szStatus == L"")
        position = 0;
    else
    {
        std::wstringstream ss;
        ss << szStatus;
        ss >> position;
    }

    return position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//�������ļ�
bool Music::Open(std::wstring filepath)
{
    if (filepath == L"")
        return false;

    if (this->GetPlayState() || this->GetStopState())	//������������ļ����򿪣����ȹر�
        this->Close();

    wchar_t szCommand[MAX_PATH];
    wsprintf(szCommand, L"OPEN %s ALIAS MUSIC%d", filepath.c_str(), this->m_ID);
    if (Music::SendToMCI(szCommand))	//�������ļ�
    {
        this->filepath = filepath;
        return true;
    }
    return false;
}

//�ر������ļ�
bool Music::Close()
{
    wchar_t szCommand[MAX_PATH];

    if (this->GetOpenState())
    {
        wsprintf(szCommand, L"CLOSE MUSIC%d", this->m_ID);
        if (Music::SendToMCI(szCommand))
            return true;
    }
    return false;
}

//��������
void Music::SetVolume(int nVolume)
{
    if (nVolume<0)
        nVolume = 0;
    if (nVolume>1000)
        nVolume = 1000;

    wchar_t szCommand[MAX_PATH];
    wsprintf(szCommand, L"Setaudio MUSIC%d volume to %d", this->m_ID, nVolume);	//����MCI�����ַ���
    Music::SendToMCI(szCommand);	//��������
}

//����
bool Music::Play(bool ReStart, long from, long to)
{
    wchar_t Command[MAX_PATH] = { L'\0' };
    if (from<0 || to<0 || (from >= to&&to != 0))
        wsprintf(Command, L"PLAY MUSIC%d", this->m_ID);
    else
    {
        if (to == 0)
            wsprintf(Command, L"PLAY MUSIC%d FROM %d", this->m_ID, from);
        else
            wsprintf(Command, L"PLAY MUSIC%d FROM %d to %d", this->m_ID, from, to);
    }

    if (ReStart)
        return this->Play(Command);
    else
        if (this->GetStopState())
            return this->Play(Command);
    return false;
}

//ֹͣ����
bool Music::Stop()
{
    wchar_t szCommand[MAX_PATH];
    if (this->GetPlayState())
    {
        wsprintf(szCommand, L"STOP MUSIC%d", m_ID);
        if (Music::SendToMCI(szCommand))
            return true;
    }
    return false;
}

//ֻ�����ַ�������Ĳ��ŷ�ʽ
bool Music::Play(std::wstring Command)
{
    if (Command == L"")
    {
        wchar_t szCommand[MAX_PATH];
        wsprintf(szCommand, L"PLAY MUSIC%d FROM 0", this->m_ID);
        return Music::SendToMCI(szCommand);
    }
    else
        return Music::SendToMCI(Command);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//������Ϣ��MCI
bool Music::SendToMCI(std::wstring command)
{
    if (!mciSendString(command.c_str(), NULL, 0, 0))
        return true;
    return false;
}

//��MCI��ȡ��Ϣ
std::wstring Music::GetFromMCI(std::wstring command)
{
    wchar_t message[MAX_PATH];
    mciSendString(command.c_str(), message, MAX_PATH, 0);
    std::wstring str(message);
    return str;
}