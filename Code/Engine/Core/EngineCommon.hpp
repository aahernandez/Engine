#pragma once

#include <atomic>
#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <string>

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)

const float ms = 1000.f;
const float us = 1000000.f;

const float KiB = 1024.f;
const float MiB = KiB * 1024.f;
const float GiB = MiB * 1024.f;

typedef unsigned int uint;
typedef unsigned __int8 byte_t;

extern std::atomic<int> gA;
extern int gB;

#define KB * 1024;
#define kB * 1000;
#define MB * 1024 * KB;
#define mB * 1000 * kB;

void pause();
std::string GetPrettierTime(float timeInSeconds);
std::string GetPrettierSize(unsigned int sizeInBytes);

class DeveloperConsole;
extern DeveloperConsole *g_theDevConsole;