#include <iostream>
#ifdef _WIN32
#include <vld.h>
#endif

#ifdef _WIN32
#define EXPORT_HEAD __declspec(dllexport) __stdcall
#else
#define EXPORT_HEAD
#endif

extern "C" EXPORT_HEAD void SdlMain();

int main()
{
	SdlMain();

	return 0;
}
