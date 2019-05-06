#include "main.h"
#include "TimeCounter.h"
#include "LanguageExtend.h"

namespace games
{
    inline LONGLONG getCounter(LARGE_INTEGER &litmp)
    {
        QueryPerformanceCounter(&litmp);
        return litmp.QuadPart;
    }

    TimeCounter & TimeCounter::instance()
    {
        static TimeCounter timeCounter;
        return timeCounter;
    }

    TimeCounter::TimeCounter()
    {
        // TODO: ʱ��Ƶ�ʲ��Ծ�
        QueryPerformanceFrequency(&litmp);
        dfFreq = (double)litmp.QuadPart;// ��ü�������ʱ��Ƶ��
        use = 0;
        begin = getCounter(litmp);
    }

    TimeCounter::~TimeCounter()
    {
    }

    void TimeCounter::Update()
    {
        use = Query();
        Dispatch(use);
        RemoveReleaseTimeCounter();
    }

    double TimeCounter::getUse() const
    {
        return use;
    }

    double TimeCounter::getTick() const
    {
        return (end + 0.0) / dfFreq * 1000;
    }

    int TimeCounter::AssignNewID()
    {
        static int id = 0;

        return ++id;
    }

    void TimeCounter::RemoveReleaseTimeCounter()
    {
        while (!removeIDs.empty()) {
            int id = removeIDs.front();
            removeIDs.pop();
            selectors.remove_if([id](const Counter &c) -> auto { return c.id == id; });
        }
    }

    void TimeCounter::Dispatch(double use)
    {
        for (auto sel = selectors.begin(); sel != selectors.end(); ++sel)
        {
            sel->use += use;
            if (sel->need <= sel->use)
            {
                int level = sel->use / sel->need;
                sel->use = sel->use - level * sel->need;
                sel->selector();
            }
        }
    }

    double TimeCounter::Query()
    {
        begin = end;
        end = getCounter(litmp);
        return (double)(end - begin) / dfFreq * 1000;   // translate s to ms
    }

    int TimeCounter::Schedule(int priority, double need, std::function<void(void)> selector)
    {
        int id = AssignNewID();
        for (auto sel = selectors.begin(); sel != selectors.end(); ++sel)
        {
            if (sel->priority >= priority)
            {
                selectors.insert(sel, { id, priority, 0.f, need, selector });
                return id;
            }
        }
        selectors.push_back({ id, priority, 0.f, need, selector });
        return id;
    }

    // ���ｫ�������ݼ�����У�Ȼ��ȵ����е� counter dispatch ��ɺ��� remove,
    // ��֤������ list iterator not incrementable
    void TimeCounter::Release(int id)
    {
        removeIDs.push(id);
    }

}

