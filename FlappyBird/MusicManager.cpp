#include "main.h"
#include "MusicManager.h"

/* ��ʼ�����ֹ���ָ̬�� */
MusicManager *MusicManager::Mmanager = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////

MusicManager::MusicManager()
{
    this->Mmanager = this;
    this->RunFlag = false;
}

MusicManager::~MusicManager()
{
    this->Release(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//�������֣�ֻ���������߳�δ��ʼ�������ʹ��
bool MusicManager::AddMusic(Music* music)
{
    if (this->RunFlag == true)
        return false;
    if (music != NULL)
    {
        this->vMusic.push_back(music);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ͷ���Դ
void MusicManager::Release(bool Del)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            (*i)->Close();
            if (Del)
                delete *i;
        }
        this->vMusic.clear();
        this->Compress();
    }
}

//ɾ��ָ��������
void MusicManager::DeleteMusic(Music* music, bool Del)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            if ((*i)->GetID() == music->GetID())
            {
                music->Close();
                if (Del)
                    delete music;
                this->vMusic.erase(i);
                break;
            }
        }
        this->Compress();
    }
}

//ѹ������
void MusicManager::Compress()
{
    std::vector<Music*> temp(this->vMusic);
    this->vMusic.swap(temp);
    temp.~vector();
}

//����ID���������Ƿ���ڣ����ھͷ���ָ�룬���򷵻�NULL
Music* MusicManager::FindMusic(int ID)
{
    if (!this->vMusic.empty())
    {
        std::vector<Music*>::iterator i;
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
        {
            if ((*i)->GetID() == ID)
                return *i;
        }
    }
    return NULL;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//���ص��߳�ѭ���еķ���
void MusicManager::Run(MusicParam MP, bool Change)
{
    std::vector<Music*>::iterator i;
    if (Change == true)
        for (i = this->vMusic.begin();i != this->vMusic.end();i++)
            if (MP.ID == (*i)->GetID())
            {
                if (MP.Delete == true)
                    this->DeleteMusic(*i, true);
                else
                    this->Change(*i, MP);
            }
    for (i = this->vMusic.begin();i != this->vMusic.end();i++)
    {
        //ʧȥ����ʱ��ͣ����
        if (MP.KillFocus == true)
        {
            (*i)->KillFocusFlag = true;
            //(*i)->SetVolume(0);	//Ҳ���Լ򵥴������������Ϊ0���������midi��Ч
        }
        //��ý���ʱ�ָ�����
        if (MP.SetFocus == true)
        {
            (*i)->KillFocusFlag = false;
            (*i)->SetFocusFlag = true;
            //(*i)->SetVolume((*i)->Volume);	//��������
        }
        if (MP.ID == (*i)->GetID())
            this->Play(*i, MP.Play, MP.ReStart, MP.Stop);
        else
            this->Play(*i, false, false, false);
    }
}

//��ʼ������
void MusicManager::Initialize()
{
    std::vector<Music*>::iterator i;
    for (i = this->vMusic.begin();i != this->vMusic.end();i++)
    {
        if ((*i)->filepath == L"")
            continue;
        (*i)->Open((*i)->filepath);
        (*i)->SetVolume((*i)->Volume);
    }
}

//����
void MusicManager::Play(Music* music, bool Play, bool ReStart, bool Stop)
{
    if (music->KillFocusFlag == true)
    {
        //ʧȥ����ʱֹͣ����
        if (music->GetStopState() == false)
        {
            music->Stop();
            music->NowPosition = music->GetNowPosition();	//��¼λ��
        }
        return;	//���޸�״̬�������ֳ�
    }
    else if (music->SetFocusFlag == true)
    {
        //��ȡ����ʱ������֮ǰ������״̬�ָ�����
        music->SetFocusFlag = false;
        if (music->Opened == true && music->Playing == true)
            music->Play(false, music->NowPosition, music->EndPosition);
    }
    else if (Stop == true)
    {
        //ֹͣ����
        music->Stop();
        music->NowPosition = music->GetNowPosition();	//��¼λ��
        music->StopPlayFlag = true;	//����ֹͣ���ű�־
    }
    else if (ReStart == true)
    {
        //�ط�
        music->Play(true, music->StartPosition, music->EndPosition);
        music->StopPlayFlag = false;
    }
    else if (music->RepeatFlag == true && music->StopPlayFlag == false)
    {
        //ѭ�����ţ�������Ӧ�طŵ�Ҫ��
        if (music->GetStopState() == true)
            music->Play(false, music->StartPosition, music->EndPosition);
    }
    else
    {
        //��������
        if (Play == true && music->StopPlayFlag == true)
        {
            music->Play(false, music->NowPosition, music->EndPosition);
            music->StopPlayFlag = false;
        }
        //����һ�Σ�ֹͣ����ʱ����Ч
        else if (Play == true && music->GetStopState() == true)
            music->Play(false, music->StartPosition, music->EndPosition);
    }
    //����״̬
    this->SetStatus(music);
}

//�޸Ĳ���
void MusicManager::Change(Music* music, MusicParam MP)
{
    if (MP.filepath != L"")
        if (music->filepath != MP.filepath)
        {
            //�����ļ�������������¶��������ļ�
            music->Open(MP.filepath);
        }
    if (MP.Volume >= 0)
        if (music->Volume != MP.Volume)
        {
            //�����ı�
            music->Volume = MP.Volume;
            music->SetVolume(MP.Volume);
        }
    if (MP.StartPosition >= 0)
        if (MP.StartPosition != music->StartPosition)
            music->StartPosition = MP.StartPosition;	//���ֿ�ʼλ�øı�
    if (MP.EndPosition >= 0)
        if (MP.EndPosition != music->EndPosition)
            music->EndPosition = MP.EndPosition;	//���ֽ���λ�øı�
    if (MP.EnableRepeat == true)
        music->RepeatFlag = MP.Repeat;	//�ı�ѭ�����ű�־
}

//����״̬
void MusicManager::SetStatus(Music* music)
{
    music->Opened = music->GetOpenState();
    music->Playing = music->GetPlayState();
    music->Stopped = music->GetStopState();
}