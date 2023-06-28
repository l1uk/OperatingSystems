cmake_minimum_required(VERSION 3.25)
project(OS C)

set(CMAKE_C_STANDARD 11)

add_executable(OS main.c ProdCons.c excercises.h SleepingBarber.c WorkerWaiter.c Bridge.c ProdConsS.c philosophers.c ProdConsSpecial.c ProdCons100.c ProdCons100.c readerWriter.c)
