#include <iostream>

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
	// MultiPlayer player(640, 360, NULL);
	SdlMain();

	return 0;
}
