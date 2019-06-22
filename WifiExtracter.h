#pragma once
#include "stdafx.h"

#ifndef WifiExtracter_H
#define WifiExtracter_H

class WifiExtracter
{
public:
	WifiExtracter();
	~WifiExtracter();
	bool EnumAllProfiles();
private:
	bool InitializeWLANeSucces = false;
	HANDLE hClient = NULL;
};
#endif // !WifiExtracter_H

WifiExtracter::WifiExtracter()
{
	// Initialize WLAN handler
	DWORD dwMaxClient = 2, dwCurVersion = 0;
	DWORD dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS)
	{
		cout << "\n[-] Failed to initialize WLAN!! \nWlanOpenHandle():Error code: " << to_string(dwResult) << endl << endl;
		return;
	}
	else
	{
		InitializeWLANeSucces = true;
		cout << "[+] WLAN handler succesfully initialized." << endl;
	}
}
WifiExtracter::~WifiExtracter()
{
	// Uninitialize WLAN handler
	WlanCloseHandle(hClient, NULL);
}
bool WifiExtracter::EnumAllProfiles()
{
	// Enumarate all profiles
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
	PWLAN_PROFILE_INFO pProfile = NULL;

	DWORD dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS)
	{
		cout << "\nFailed to enum all wifi profiles! \nWlanEnumInterfaces():Error code: " << to_string(dwResult) << endl << endl;
		return false;
	}

	cout << "\nAll Wifi interfaces:\n" << endl;

	// Enumerating wifi profiles
	for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++)
	{
		pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
		cout << "  Interface [" << to_string(i) << "]:" << endl;

		// Retrieve string from GUID
		WCHAR GuidString[39] = { 0 };
		int iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
			sizeof(GuidString) / sizeof(*GuidString));
		if (iRet != 0)
			wcout << "   Interface GUID: " << GuidString << endl;
		else
			wcout << "   Interface GUID: " << "error" << endl;

		// Interface description
		wcout << "   Interface Description: " << pIfInfo->strInterfaceDescription << endl;

		// Interface state
		cout << "   Interface State: ";
		switch (pIfInfo->isState)
		{
		case wlan_interface_state_not_ready:
			cout << "Not ready\n";
			break;
		case wlan_interface_state_connected:
			cout << "Connected\n";
			break;
		case wlan_interface_state_disconnected:
			cout << "Disconnected\n";
			break;
		case wlan_interface_state_ad_hoc_network_formed:
			cout << "First node in an ad hoc network\n";
			break;
		case wlan_interface_state_disconnecting:
			cout << "Disconnecting\n";
			break;
		case wlan_interface_state_associating:
			cout << "Attempting to associate with a network\n";
			break;
		case wlan_interface_state_authenticating:
			cout << "In process of authenticating\n";
			break;
		case wlan_interface_state_discovering:
			cout << "Auto configuration is discovering settings for the network\n";
			break;
		default:
			cout << "Unknown state\n";
			break;
		}

		// Now get the profile list of the interface
		dwResult = WlanGetProfileList(hClient, &pIfInfo->InterfaceGuid, NULL, &pProfileList);
		if (dwResult != ERROR_SUCCESS)
		{
			cout << "   Profile list: Failed to get profile list! Error code: " << to_string(dwResult) << endl;
		}
		else
		{
			cout << "   Profile list: " << endl;
			for (int j = 0; j < pProfileList->dwNumberOfItems; j++)
			{
				pProfile = (WLAN_PROFILE_INFO *)&pProfileList->ProfileInfo[j];

				// Call for every profile GetProfile() function to receive the needed information
				LPCWSTR pProfileName = pProfile->strProfileName;
				LPWSTR pProfileXML = NULL;
				DWORD dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY, dwGrantedAcces = 0;
				dwResult = WlanGetProfile(hClient, &pIfInfo->InterfaceGuid, pProfileName,
					NULL, &pProfileXML, &dwFlags, &dwGrantedAcces);
				if (dwResult != ERROR_SUCCESS)
				{
					wcout << "\tFailed to get the profile[" << to_wstring(j) << "] info from " << pProfile->strProfileName << endl;
					cout << "\tError code: " << to_string(dwResult) << endl;
					continue;
				}
				else
				{
					// Profile name
					wcout << "\tProfile Name [" << to_wstring(j) << "]: " << pProfile->strProfileName << endl;
					
					// Profile acces
					wcout << "\t  Granted Acces: ";
					if (dwGrantedAcces & WLAN_READ_ACCESS)
						wcout << "| Read acces | ";
					if (dwGrantedAcces & WLAN_EXECUTE_ACCESS)
						wcout << "Execute acces | ";
					if (dwGrantedAcces & WLAN_WRITE_ACCESS)
						wcout << "Write acces |";
					cout << endl;

					// Profile key
					string MyStr = CW2A(pProfileXML);
					if (MyStr.find("<keyMaterial>") == string::npos)
					{
						cout << "\t  KEY: None" << endl;
						continue;
					}
					else
					{
						string MyStr2 = MyStr;
						string Str1 = MyStr2.substr(MyStr.find("<keyMaterial>"));
						string Str2 = Str1.substr(13);
						string PASSWORD = Str2.substr(0, Str2.find("</keyMaterial>"));
						cout << "\t  KEY: " << PASSWORD << endl;
					}
				}
			}
			cout << endl << endl;
		}
	}

}