/*
 * GetFontFile.cpp (Win32)
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "GetFontFile.h"

#include <Windows.h>
#include <algorithm>


namespace Tg
{


/*
This source code was derived from a public domain software by Hans Dietrich (2001)
\see http://www.codeproject.com/Articles/1235/Finding-a-Font-file-from-a-Font-name
*/

static bool OpenRegistryKey(HKEY key, const std::string& subKeyName, HKEY& subKeyHandle)
{
	return (RegOpenKeyEx(key, subKeyName.c_str(), 0, KEY_READ, (&subKeyHandle)) == ERROR_SUCCESS);
}

static void CloseRegistryKey(HKEY subKeyHandle)
{
    if (subKeyHandle)
        RegCloseKey(subKeyHandle);
}

static bool GetNextFontName(HKEY key, std::string& name, std::string& value, DWORD index)
{
	char valueName[MAX_PATH];
	DWORD valueNameSize = (sizeof(valueName) - 1);
	
    BYTE valueData[MAX_PATH];
	DWORD valueDataSize = (sizeof(valueData) - 1);

	DWORD type = 0;

	LONG retval = RegEnumValue(
        key, index, valueName, &valueNameSize, NULL, 
		&type, valueData, &valueDataSize
    );

	if (retval == ERROR_SUCCESS) 
	{
        name = std::string(reinterpret_cast<const char*>(valueName));
        value = std::string(reinterpret_cast<const char*>(valueData));
        return true;
	}
	
    return false;
}

bool GetFontFile(std::string fontName, std::string& fontFilename)
{
    /* Clear output filename and check for valid input */
	fontFilename.clear();

    if (fontName.empty())
        return false;

    /* Convert input font name to lower case */
    std::transform(fontName.begin(), fontName.end(), fontName.begin(), ::tolower);
    
	const std::string regKeyName = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
	std::string name, value;

	bool result = false;

    /* Open registry key for fonts */
    HKEY regKey = NULL;
    if (!OpenRegistryKey(HKEY_LOCAL_MACHINE, regKeyName, regKey))
        throw std::runtime_error(std::string("failed to open registry key: ") + regKeyName);

    /* Iterate over all fonts in the Windows registry */
    DWORD index = 0;

	while (GetNextFontName(regKey, name, value, index++))
	{
        /* Find input font name within current name in lower case */
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        if (name.find(fontName) != std::string::npos)
		{
			fontFilename = value;
			result = true;
			break;
		}
	}

    /* Clean up */
    CloseRegistryKey(regKey);

	return result;
}


} // /namespace Tg



// ================================================================================
