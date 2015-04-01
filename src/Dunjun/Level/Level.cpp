#include <Dunjun/Level/Level.hpp>

namespace Dunjun
{
	Level::Level()
		: m_random()
		, m_rooms()
	{}

	void Level::generate()
	{
		placeRooms();	
	}

	void Level::placeRooms()
	{
		if (!material)
		{
			std::cout << "Level::placeRooms material == nullptr" << std::endl;
			throw std::runtime_error("Level::placeRooms material == nullptr");
			return;
		}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				auto room = make_unique<Room>(m_random, Room::Size(8, 8, 3));
				room->transform.position.x = 8 * i;
				room->transform.position.z = 8 * j;
				room->material = this->material;
				room->generate();

				attachChild(std::move(room));
			}
		}
	}

}