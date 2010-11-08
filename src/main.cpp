#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <cassert>
#include <climits>

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

	inline position operator+(position const& p, direction const& d)
	{
		return position(p.x + d.x, p.y + d.y);
	}

	class actor
	{
	public:
		virtual king::position get_position() const = 0;
		virtual void set_position(king::position const& position) = 0;
	};

	class character : public actor
	{
	public:
		character(king::position const& position): position(position) {}

		// actor
		virtual king::position get_position() const
		{
			return this->position;
		}

		virtual void set_position(king::position const& position)
		{
			this->position = position;
		}

		king::position position;
	};

	class event
	{
	public:
		virtual int get_time() const = 0;
		virtual void handle() = 0;
	};

	class clock
	{
	public:
		virtual int get_time() const = 0;
		virtual void set_time(int time) = 0;
	};

	class event_registry
	{
	public:
		virtual void add_event(boost::shared_ptr<event> const& event) = 0;
	};

	class event_manager : public event_registry
	{
	public:
		event_manager(king::clock* clock): clock(clock) {}

		void handle_next_event()
		{
			// Get the first event in the list.
			assert(!this->events.empty());
			event_map::iterator event_pos = this->events.begin();
			int event_time = (*event_pos).first;
			boost::shared_ptr<event> event = (*event_pos).second;
			this->events.erase(event_pos);

			// Advance time to the event time.
			this->clock->set_time(event_time);

			// Handle the event.
			event->handle();
		}

		// event_registry
		virtual void add_event(boost::shared_ptr<event> const& event)
		{
			this->events.insert(std::make_pair(event->get_time(), event));
		}

		typedef std::multimap<int, boost::shared_ptr<event> > event_map;

		king::clock* clock;
		event_map events;
	};

	class simulation : public clock
	{
	public:
		simulation(int time): time(time) {}

		// clock
		virtual int get_time() const
		{
			return this->time;
		}

		virtual void set_time(int time)
		{
			assert(time >= this->time);
			this->time = time;
		}

		int time;
	};

	class move_task : public event
	{
	public:
		move_task(boost::shared_ptr<king::actor> const& actor,
				direction const& direction, int start_time, int duration)
			: actor(actor), direction(direction),
			start_time(start_time), duration(duration) {}

		// event
		virtual int get_time() const
		{
			return this->start_time + this->duration;
		}

		virtual void handle()
		{
			king::position new_position = this->actor->get_position() +
					this->direction;
			this->actor->set_position(new_position);

			printf("Actor moved to <%d,%d>\n", new_position.x, new_position.y);
		}

		boost::shared_ptr<king::actor> actor;
		king::direction direction;
		int start_time;
		int duration;
	};

	class end_simulation_task : public event
	{
	public:
		end_simulation_task(bool& exit_flag, int time)
			: exit_flag(exit_flag), time(time) {}

		// event
		virtual int get_time() const
		{
			return this->time;
		}

		virtual void handle()
		{
			this->exit_flag = true;
		}

		bool& exit_flag;
		int time;
	};
}

int main(int /*argc*/, char* /*argv*/[])
{
	bool should_exit = false;

	king::simulation simulation(0);
	king::event_manager event_manager(&simulation);

	boost::shared_ptr<king::event> exit_event(
			new king::end_simulation_task(should_exit, INT_MAX));
	event_manager.add_event(exit_event);

	boost::shared_ptr<king::character> test_guy(
			new king::character(king::position(2, 2)));

	boost::shared_ptr<king::move_task> move_task(new king::move_task(
			test_guy, king::direction(1, 0), 0, 100));

	event_manager.add_event(move_task);

	while (!should_exit)
		event_manager.handle_next_event();

	return 0;
}

