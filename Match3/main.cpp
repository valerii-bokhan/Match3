#include "Game.h"

#include <iostream>

using namespace std;
using namespace Match3;

namespace
{
	inline constexpr int make_index(int x, int y, int width)
	{
		return y * width + x;
	}
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);

	/*Board board(7, 10, {
		2, 3, 4, 3, 2, 5, 5,
		1, 5, 4, 2, 4, 5, 1,
		5, 4, 1, 5, 4, 4, 1,
		1, 4, 5, 1, 5, 1, 4,
		4, 2, 1, 3, 2, 5, 4,
		3, 5, 3, 4, 2, 2, 5,
		5, 4, 2, 4, 3, 3, 5,
		1, 4, 3, 3, 4, 2, 1,
		3, 5, 4, 4, 1, 3, 4,
		5, 2, 5, 3, 4, 4, 3
		});*/

	const int width = 7, height = 10;

	Game game({ width, height });

	game.PrintBoard("Board");
	game.PrintHints();

	while (true)
	{
		int x1, x2, y1, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		cout << "\n";

		const int idx1 = make_index(x1, y1, width);
		const int idx2 = make_index(x2, y2, width);

		if (!game.MakeMove(idx1, idx2))
			continue;

		game.ProcessMove();
		game.PrintHints();
	}

	return 0;
}
