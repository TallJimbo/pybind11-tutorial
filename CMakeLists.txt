cmake_minimum_required(VERSION 2.8.12)
project(spanops)

add_subdirectory(pybind11)
pybind11_add_module(spanops src/spanops.cc src/main.cpp)
