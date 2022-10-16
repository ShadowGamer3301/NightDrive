#pragma once
//Check if platform is either 32-bit or 64-bit windows
#if defined _WIN32 || defined _WIN64
	//Check if output is set as DLL file
	#ifdef _DLL
		#define NAPI __declspec(dllexport) //If output is DLL create additional LIB file for linking
	#else
		#define NAPI __declspec(dllimport) //If output is not DLL read contents of LIB and DLL
	#endif
#endif

