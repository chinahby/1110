#pragma once

class COSVersion
{
public:
	enum EOSType
	{
		WIN_UNKNOWN = 0,
		WIN_98 ,
		WIN_ME,
		WIN_2K,
		WIN_XP,
		WIN_2003,
		WIN_VISTA
	};

	static EOSType GetOSVersion();
	static bool IsWin9X();
	static bool IsWin2KLater(){return GetOSVersion()>=WIN_2K;}
	static bool Is64BitsPlatform();
private:
	static bool	IsAMD64();
};