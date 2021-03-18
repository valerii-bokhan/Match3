#include "Game.h"

#include <iostream>

using namespace std;
using namespace Match3;

int main()
{
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

	Game game({ 7, 10 });

	game.PrintBoard("Board");
	game.PrintHint();

	while (true)
	{
		int idx1, idx2;
		cin >> idx1 >> idx2;
		cout << "\n";

		if (!game.MakeMove(idx1, idx2))
			continue;

		game.ProcessMove();
		game.PrintHint();
	}

	return 0;
}
