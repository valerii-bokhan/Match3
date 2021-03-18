#pragma once

#include "Board.h"

namespace Match3
{
	struct Config
	{
		int width;
		int height;
	};

	class Game final
	{
	public:
		Game(const Config& config);

	public:
		bool MakeMove(int idx1, int idx2);

		void ProcessMove();
		void UpdateBoard();

		// "Renderers"

		void PrintHint();
		void PrintBoard(const char* title);

	private:
		Board board;
		Move hint;

		Indexes matches;
		Indexes affected;
	};
}
