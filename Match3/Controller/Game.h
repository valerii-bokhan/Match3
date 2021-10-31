#pragma once

#include <Model/Board.h>
#include <View/Renderer.h>

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
		explicit Game(const Config& config);

	public:
		void Run();

		bool MakeMove(int idx1, int idx2);

		void ProcessMove();
		void UpdateBoard();

	private:
		Config config;

		Board board;
		Renderer renderer;

		Indexes matches;
		Indexes affected;
	};
}
