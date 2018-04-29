
project(py_miner)
set(SOURCE_DIR "py_miner")
set(SOURCES "${SOURCE_DIR}/monero_cpu_miner.cpp")

include_directories(${SOURCE_DIR})
include_directories("lib/xmr-stak")

add_subdirectory(lib/xmr-stak)
add_subdirectory(lib/pybind11)

add_library(py_miner MODULE ${SOURCES} "${SOURCE_DIR}/py_bindings.cpp")
target_link_libraries(py_miner PRIVATE pybind11::module xmr-stak-backend)
set_target_properties(py_miner PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                          SUFFIX "${PYTHON_MODULE_EXTENSION}")