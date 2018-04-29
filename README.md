# Installation

```
$ git clone https://github.com/lukqe/py_miner.git
$ git submodule init
$ git submodule update
$ pip install -r requirements.txt
$ pip install -e .
```

# Configuration

Please use xmr-stak configuration files:
 - config.txt
 - cpu.txt
 - pools.txt


# Usage

```
$ pytest -s tests/
```


# Memory usage
https://psutil.readthedocs.io/en/latest/#psutil.Process.memory_info