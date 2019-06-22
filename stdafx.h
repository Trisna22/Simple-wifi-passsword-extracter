#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <wlanapi.h>
#include <atlstr.h>

using namespace std;

#pragma comment(lib, "Wlanapi.lib")
#pragma comment(lib, "Ole32.lib")

#ifndef UNICODE
#define UNICODE
#elif not defined WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !UNICODE & !WIN32_LEAN_AND_MEAN
