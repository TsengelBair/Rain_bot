#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <thread>
#include <atomic>

class RainBot;

class Scheduler
{
public:
    explicit Scheduler(RainBot &bot);
    ~Scheduler();

    void start();
    void stop();

private:
    void runInOtherThread();
    bool isRainExpected();
    void logRainRequestResult(const std::pair<std::time_t, std::time_t> &boundaries, bool isRain);

private:
    RainBot& m_bot;
    std::thread m_thread;
    std::atomic<bool> m_isRunning;
};

#endif // SCHEDULER_H
