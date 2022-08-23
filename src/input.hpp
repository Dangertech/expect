/* Contains everything related to raw key processing */
#pragma once
#include "util.hpp"
#include "iostream"
#include "vector"
#include "SFML/Graphics.hpp"

namespace ipt
{
	enum action { NONE, TURN, PICKUP, ZOOM_IN, ZOOM_OUT, ENTER_CLI, EXIT_CLI, ELSE };
	struct InputContainer
	{
		action act;
		Vec2 dir;
		/* Text Entered in cli mode */
		wchar_t txt;
		static bool incli;
		void print(std::ostream& os);
	};
	/* This takes a bundle of events and spits out InputContainers that can
	 * be logically processed further
	 */
	std::vector<ipt::InputContainer> process_input(std::vector<sf::Event>& ev);
}
