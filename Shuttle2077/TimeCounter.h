#pragma once

namespace games 
{
    struct Counter {
        int id;
        int priority;
        double use;
        double need;
        std::function<void(void)> selector;
    };

    class TimeCounter
    {
    public:
        static TimeCounter &instance();

        int Schedule(int priority, double need, std::function<void(void)> selector);
        void Release(int id);

        void Update();
        double getUse() const;
        double getTick() const;
    private:
        TimeCounter();
        ~TimeCounter();

        void RemoveReleaseTimeCounter();
        void Dispatch(double use);
        double Query();

        int AssignNewID();
    private:
        LARGE_INTEGER litmp;
        LONGLONG begin, end;

        double dfFreq;
        double use;

        std::list<Counter> selectors;
        std::queue<int> removeIDs;
    };

}