#include <thread>
#include <atomic>
#include <condition_variable>

#include <pybind11/pybind11.h>
#include "monero_cpu_miner.hpp"

namespace py = pybind11;

PYBIND11_MODULE(py_miner, m_main) {
    m_main.doc() = "pybind11 py_miner CPU monero xmr-stak wrapper";

    auto m_miners = m_main.def_submodule("miners");
    py::class_<miners::MoneroCPU>(m_miners, "MoneroCPU")
        .def(py::init<>())
        .def("start", &miners::MoneroCPU::start)
        .def("stop", &miners::MoneroCPU::stop)
        .def("pause", &miners::MoneroCPU::pause)
        .def("resume", &miners::MoneroCPU::resume);
}