#include <iostream>
#include <vld.h>

/*
#include "multi-player.h"

using namespace std;
using namespace cxm::av;

#ifdef main
#undef main
#endif
*/
extern "C" __declspec(dllexport) void __stdcall SdlMain();

int main()
{
	SdlMain();

	return 0;
}
