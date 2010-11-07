#include <stdio.h>
#include <boost/shared_ptr.hpp>

namespace king
{
	class position
	{
	public:
		position(int x, int y): x(x), y(y) {}
		int x, y;
	};

	class direction
	{
	public:
		direction(int x, int y): x(x), y(y) {}
		int x, y;
	};

	class actor
	{
	public:
		int id;
		king::position position;
	};

	class character : public actor
	{
	public:
	};

	class character_manager
	{
	public:
		std::vector<boost::shared_ptr<character> > characters;
	};

	class move_task
	{
	public:
		king::actor* actor;
		king::direction direction;
		int start_time;
	};

	class move_task_manager
	{
	public:
		std::vector<boost::shared_ptr<move_task> > tasks;
	};
}

int main(int /*argc*/, char* /*argv*/[])
{
	return 0;
}

