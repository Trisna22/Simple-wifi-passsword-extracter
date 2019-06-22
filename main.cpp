#include "stdafx.h"
#include "WifiExtracter.h"

int main(int argc, char* argv[])
{
	cout << "WifiExtracter..." << endl;
	WifiExtracter WifiExtracter;
	WifiExtracter.EnumAllProfiles();
	return 0;
}