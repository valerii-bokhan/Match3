#include "Application.h"

#include "Controller/Game.h"

using namespace Match3;

namespace
{
	constexpr int kBoardWidth = 7;
	constexpr int kBoardHeight = 10;
}

void Application::Run()
{
	Game game({ kBoardWidth, kBoardHeight });

	game.Run();
}
