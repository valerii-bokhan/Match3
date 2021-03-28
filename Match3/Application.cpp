#include "Application.h"

#include <iostream>

using namespace std;
using namespace Match3;

namespace
{
	constexpr int width = 7;
	constexpr int height = 10;

	inline constexpr Config get_config()
	{
		return { width, height };
	}

	inline constexpr int make_index(int x, int y)
	{
		return y * width + x;
	}
}

Application::Application()
{
	// TODO: move to the View
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
}

void Application::Run()
{
	Game game(get_config());

	game.PrintBoard("Board");
	game.PrintHints();

	while (true)
	{
		int x1, x2, y1, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		cout << "\n";

		const int idx1 = make_index(x1, y1);
		const int idx2 = make_index(x2, y2);

		if (!game.MakeMove(idx1, idx2))
			continue;

		game.ProcessMove();
		game.PrintHints();
	}
}
