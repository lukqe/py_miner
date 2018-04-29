#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <string>
#include <thread>

#include "xmrstak/misc/executor.hpp"
#include "xmrstak/backend/miner_work.hpp"
#include "xmrstak/backend/backendConnector.hpp"
#include "xmrstak/jconf.hpp"
#include "xmrstak/params.hpp"
#include "xmrstak/misc/configEditor.hpp"
#include "xmrstak/version.hpp"
#include "xmrstak/misc/utility.hpp"

#ifdef _WIN32
#	define strcasecmp _stricmp
#	include <windows.h>
#	include "xmrstak/misc/uac.hpp"
#endif // _WIN32

#include "monero_cpu_miner.hpp"

//----------------------------------------------------------------------------//
namespace miners
{
//----------------------------------------------------------------------------//
MoneroCPU::MoneroCPU()
    : finish_(false)
    , paused_(false)
{
    printer::inst()->print_msg(L1, "MoneroCPU::MoneroCPU");
}

MoneroCPU::~MoneroCPU() 
{
    printer::inst()->print_msg(L1, "MoneroCPU::~MoneroCPU");
    stop();
}

bool
MoneroCPU::start()
{
    printer::inst()->print_msg(L1, "MoneroCPU::start");

    // re-initialize state
    finish_ = false;
    paused_ = false;
    
    worker_.reset(new std::thread(&MoneroCPU::do_work, this));
    return true;
}

bool
MoneroCPU::stop()
{
    printer::inst()->print_msg(L1, "MoneroCPU::stop");

    // set finish flag and signal the thread to resume in the case pause was triggered
    finish_ = true;
    if (worker_.get())
    {
        resume();
        worker_->join();
        worker_.release();
    }

    return true;
}

bool
MoneroCPU::pause()
{
    printer::inst()->print_msg(L1, "MoneroCPU::pause");
    std::lock_guard<std::mutex> lk(mutex_);
    paused_ = true;
 
    return true;
}

bool
MoneroCPU::resume()
{
    printer::inst()->print_msg(L1, "MoneroCPU::resume");
    std::lock_guard<std::mutex> lk(mutex_);
    paused_ = false;
    pause_cv_.notify_all();
    return true;
}

int configure()
{
    // check existance of pre-generated xmr-stak configuration files
    if (!xmrstak::configEditor::file_exist(xmrstak::params::inst().configFile))
        printer::inst()->print_msg(L4, "Configuration 'config.txt' file does not exist");

    if (!xmrstak::configEditor::file_exist(xmrstak::params::inst().configFilePools))
        printer::inst()->print_msg(L4, "Configuration 'pools.txt' file does not exist");

    if (!jconf::inst()->parse_config(
        xmrstak::params::inst().configFile.c_str(),
        xmrstak::params::inst().configFilePools.c_str()))
    {
        printer::inst()->print_msg(L4, "Not able to parse configuration files");
        win_exit();
        return 1;
    }

    if (strlen(jconf::inst()->GetOutputFile()) != 0)
        printer::inst()->open_logfile(jconf::inst()->GetOutputFile());

    if (!xmrstak::BackendConnector::self_test())
    {
        printer::inst()->print_msg(L4, "Backend self test failed");
        win_exit();
        return 1;
    }
}

void
MoneroCPU::do_work()
{
    printer::inst()->print_msg(L0, "Starting ......");
    
    // read config files
    configure();

    // monero block version
    uint8_t block_verstion = 7;

    // work block
    uint8_t work[112];
    memset(work, 0, 112);
    work[0] = block_verstion;

    std::vector<xmrstak::iBackend*>* pv_threads;
    xmrstak::pool_data data;
    xmrstak::miner_work o_work = xmrstak::miner_work();
    
    pv_threads = xmrstak::BackendConnector::thread_starter(o_work);

    // wait 30 sec until all backends are initialized
    std::this_thread::sleep_for(std::chrono::seconds(30));

    // simulate work
    xmrstak::miner_work thread_work = xmrstak::miner_work("", work, 84, 0, false, 0);
    xmrstak::globalStates::inst().switch_work(thread_work, data);

    while (!finish_)
    {
        std::unique_lock<std::mutex> lk(mutex_);
        if (paused_)
        {
            xmrstak::globalStates::inst().switch_work(o_work, data);
            pause_cv_.wait(lk, [this] {return paused_ == false; });
            
            if (!finish_)
                xmrstak::globalStates::inst().switch_work(thread_work, data);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    //clean-up
    xmrstak::globalStates::inst().switch_work(o_work, data);
    double fTotalHps = 0.0;
    for (xmrstak::iBackend* pv_thread: *pv_threads)
    {
        delete pv_thread;
    }
    
    delete pv_threads;
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
