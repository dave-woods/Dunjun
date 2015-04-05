#include <Dunjun/Level/Level.hpp>

namespace Dunjun
{
	Level::Level()
		: m_random(1337)
		, rooms()
	{}

	void Level::generate()
	{
		//placeRooms(-1);
		placeRooms(0);
		//placeRooms(1);
	}

	void Level::placeRooms(int floor)
	{
		if (!material)
		{
			std::cout << "Level::placeRooms material == nullptr" << std::endl;
			return;
		}

		const int gridWidth = 100;
		const int gridHeight = 100;
		const Room::Size size(9, 9, 3);

		bool grid[gridWidth][gridHeight] = { { false } };

		grid[gridWidth / 2][gridHeight / 2] = true;

		for (int randomWalks = 0; randomWalks < 4; randomWalks++)
		{

			int x = gridWidth / 2;
			int y = gridHeight / 2;

			int prevX = gridWidth / 2;
			int prevY = gridHeight / 2;

			for (int n = 0; n < 10; n++)
			{
				int direction = m_random.getInt(0, 3);
				for (int l = 0; l < 3; l++)
				{
					switch (direction)
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

					x = Math::clamp(x, 0, gridWidth - 1);
					y = Math::clamp(y, 0, gridHeight - 1);

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
			for (int shoot = 0; shoot < 10; shoot++)
			{
				while (true)
				{
					int x = m_random.getInt(0, gridWidth - 1);
					int y = m_random.getInt(0, gridHeight - 1);

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

						x = Math::clamp(x, 0, gridWidth - 1);
						y = Math::clamp(y, 0, gridHeight - 1);

						if (grid[x][y])
							continue;

						grid[x][y] = true;
						break;
					}
				}
			}
		}

		for (int i = 0; i < gridWidth; i++)
		{
			for (int j = 0; j < gridHeight; j++)
			{
				if (!grid[i][j])
					continue;

				auto room = make_unique<Room>(m_random, size);
				room->transform.position.x = size.x * (i - gridWidth / 2.0f);
				room->transform.position.y = size.z * (f32)floor;
				room->transform.position.z = size.y * (j - gridHeight / 2.0f);

				room->material = this->material;
				
				bool northDoor = false;
				bool eastDoor = false;
				bool southDoor = false;
				bool westDoor = false;


				if (i != 0)
				{
					if (grid[i - 1][j])
						westDoor = true;
				}
				if (i != gridWidth - 1)
				{
					if (grid[i + 1][j])
						eastDoor = true;
				}
				if (j != 0)
				{
					if (grid[i][j - 1])
						northDoor = true;
				}
				if (j != gridHeight - 1)
				{
					if (grid[i][j + 1])
						southDoor = true;
				}

				room->generate(northDoor, eastDoor, southDoor, westDoor);
				
				rooms.push_back(room.get());

				attachChild(std::move(room));
			}
		}
	}

}