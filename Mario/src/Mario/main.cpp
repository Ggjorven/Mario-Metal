#include "mmpch.h"
#include "Mario/Core/Game.hpp"

static int Main()
{
    Mario::Game game;
    game.Run();
    return 0;
}

#if defined(MM_CONFIG_DIST) && defined(MM_PLATFORM_WINDOWS) // Dist on Windows, removes console.
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    return Main();
}
#else
int main()
{
	return Main();
}
#endif