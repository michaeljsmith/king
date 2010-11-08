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

	class actor
	{
	public:
		virtual int get_id() const = 0;

		virtual king::position get_position() const = 0;
		virtual void set_position(king::position const& position) = 0;
	};

	class character : public actor
	{
	public:
		virtual int get_id() const = 0;

		virtual king::position get_position() const = 0;
		virtual void set_position(king::position const& position) = 0;

		int id;
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

	class end_simulation_task : public event
	{
	public:
		end_simulation_task(bool& exit_flag, int time)
			: exit_flag(exit_flag) {}

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
}

int main(int /*argc*/, char* /*argv*/[])
{
	bool should_exit = false;

	king::simulation simulation(0);
	king::event_manager event_manager(&simulation);

	boost::shared_ptr<king::event> exit_event(new king::end_simulation_task(should_exit, INT_MAX));
	event_manager.add_event(exit_event);

	while (!should_exit)
		event_manager.handle_next_event();

	return 0;
}

