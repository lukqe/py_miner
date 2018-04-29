#pragma once 

//----------------------------------------------------------------------------//
namespace miners
{
//----------------------------------------------------------------------------//

class MoneroCPU
{
public:
    MoneroCPU();
    virtual ~MoneroCPU();

    // operations
    bool start();
    bool stop();
    bool pause();
    bool resume();
     
private:
    void do_work();

private:
    std::atomic<bool> finish_;
    std::atomic<bool> paused_;
    std::unique_ptr<std::thread> worker_;
    std::condition_variable pause_cv_;
    mutable std::mutex mutex_;
};

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
