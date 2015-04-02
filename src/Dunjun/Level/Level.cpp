#include <Dunjun/Level/Level.hpp>

namespace Dunjun
{
	Level::Level()
		: m_random()
		, m_rooms()
	{}

	void Level::generate()
	{
		placeRooms(-1);
		placeRooms(0);
		placeRooms(1);
	}

	void Level::placeRooms(int floor)
	{
		if (!material)
		{
			std::cout << "Level::placeRooms material == nullptr" << std::endl;
			return;
		}

		//m_random.setSeed(1);

		const int w = 8;
		const int h = 8;
		const Room::Size size(8, 8, 3);

		bool grid[w][h] = { { false } };

		grid[w / 2][h / 2] = true;

		for (int r = 0; r < 2; r++)
		{

			int x = w / 2;
			int y = h / 2;

			int prevX = w / 2;
			int prevY = h / 2;

			for (int n = 0; n < 6; n++)
			{
				int d = m_random.getInt(0, 3);
				for (int l = 0; l < 4; l++)
				{
					switch (d)
					{
					case 0: // right
						x++;
						break;
					case 1: // up
						y++;
						break;
					case 2: // left
						x--;
						break;
					case 3: // down
						y--;
						break;
					}

					if (x > w - 1)
						x = w - 1;
					if (y > h - 1)
						y = h - 1;
					if (x < 0)
						x = 0;
					if (y < 0)
						y = 0;

					if (grid[x][y])
					{
						x = prevX;
						y = prevY;
						continue;
					}

					grid[x][y] = true;
					prevX = x;
					prevY = y;
				}
			}
		}

		{
			for (int shoot = 0; shoot < 5; shoot++)
			{
				while (true)
				{
					int x = m_random.getInt(0, w - 1);
					int y = m_random.getInt(0, h - 1);

					if (grid[x][y])
					{
						int d = m_random.getInt(0, 3);

						switch (d)
						{
						case 0: // right
							x++;
							break;
						case 1: // up
							y++;
							break;
						case 2: // left
							x--;
							break;
						case 3: // down
							y--;
							break;
						}

						if (x > w - 1)
							x = w - 1;
						if (y > h - 1)
							y = h - 1;
						if (x < 0)
							x = 0;
						if (y < 0)
							y = 0;

						if (grid[x][y])
							continue;

						grid[x][y] = true;
						break;
					}
				}
			}
		}

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (!grid[i][j])
					continue;

				auto room = make_unique<Room>(m_random, size);
				room->transform.position.x = size.x * i;
				room->transform.position.y = size.z * floor;
				room->transform.position.z = size.y * j;

				room->material = this->material;
				room->generate();

				attachChild(std::move(room));
			}
		}
	}

}