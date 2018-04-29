import os
import pytest
import psutil
import sys
import time

import py_miner

# process memory info
#https://psutil.readthedocs.io/en/latest/#psutil.Process.memory_info
pid = os.getpid()
process = psutil.Process(pid)

def test_miner_workflow():

    print('Starting, memory: {}'.format(process.memory_info()))
    
    moneru_cpu = py_miner.miners.MoneroCPU()
    started = moneru_cpu.start()
    assert started == True
    print('Started, memory: {}'.format(process.memory_info()))
    time.sleep(60)

    paused = moneru_cpu.pause()
    assert paused == True
    print('Paused, memory: {}'.format(process.memory_info()))
    time.sleep(10)

    resumed = moneru_cpu.resume()
    assert resumed == True
    print('Resumed, memory: {}'.format(process.memory_info()))
    time.sleep(10)

    stoped = moneru_cpu.stop()
    assert stoped == True
    print('Stopped, memory: {}'.format(process.memory_info()))
    time.sleep(10)

    started = moneru_cpu.start()
    assert started == True
    print('Statred, memory: {}'.format(process.memory_info()))
    time.sleep(60)
