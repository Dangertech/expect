#include "input.hpp"

bool ipt::InputContainer::incli = false;
void ipt::InputContainer::print(std::ostream& os)
{
	std::string act_name;
	switch (act)
	{
		case NONE: act_name = "NONE"; break;
		case TURN: act_name = "TURN"; break;
		case PICKUP: act_name = "PICKUP"; break;
		case ZOOM_IN: act_name = "ZOOM_IN"; break;
		case ZOOM_OUT: act_name = "ZOOM_OUT:"; break;
		case ENTER_CLI: act_name = "ENTER_CLI:"; break;
		case EXIT_CLI: act_name = "EXIT_CLI:"; break;
		case ELSE: act_name = "ELSE"; break;
	}
	os << "Action: " << act_name 
		<< ", Incli: " << incli << ", TXT: " << char(txt)
		<< ", Direction: x=" << dir.x << " y=" << dir.y << std::endl;
}
 
std::vector<ipt::InputContainer> ipt::process_input(std::vector<sf::Event>& ev)
{
	std::vector<ipt::InputContainer> ret;
	/* If set to true, putting text into the txt variable can be avoided;
	 * Used to not handle the key that activates the CLI as CLI input already
	 */
	bool skiptxt = false;
	for (int i= 0; i<ev.size(); i++)
	{
		/* Local variables that get changed in this
		 * iteration
		 */
		ipt::action act = ipt::NONE;
		Vec2 dir = {0, 0};
		wchar_t txt = 0;
		bool inc = ipt::InputContainer::incli;
		if (ev[i].type == sf::Event::TextEntered && inc && !skiptxt)
		{
			txt = ev[i].text.unicode;
		}
		if (ev[i].type == sf::Event::KeyPressed)
		{
			auto cd = ev[i].key.code;
			if (cd == sf::Keyboard::Key::Space)
			{
				if (!inc)
				{
					act = ipt::ENTER_CLI; 
					inc = true;
					skiptxt = true;
				}
			}
			else if (cd == sf::Keyboard::Key::Escape)
			{
				if (inc)
				{
					act = ipt::EXIT_CLI;
					inc = false;
				}
			}
			else if (cd == sf::Keyboard::Key::L)
				dir.x = 1;
			else if (cd == sf::Keyboard::Key::H)
				dir.x = -1;
			else if (cd == sf::Keyboard::Key::J)
				dir.y = 1;
			else if (cd == sf::Keyboard::Key::K)
				dir.y = -1;
			else if (cd == sf::Keyboard::Key::Y)
			{
				dir.y = -1;
				dir.x = -1;
			}
			else if (cd == sf::Keyboard::Key::U)
			{
				dir.y = -1;
				dir.x = 1;
			}
			else if (cd == sf::Keyboard::Key::N)
			{
				dir.y = 1;
				dir.x = 1;
			}
			else if (cd == sf::Keyboard::Key::B)
			{
				dir.y = 1;
				dir.x = -1;
			}
			else if (cd == sf::Keyboard::Key::Period)
			{
				act = ipt::TURN;
			}
			else if (cd == sf::Keyboard::Key::Comma)
				act = ipt::PICKUP;
			else if (cd == sf::Keyboard::Key::Add)
				act = ipt::ZOOM_IN;
			else if (cd == sf::Keyboard::Key::Subtract)
				act = ipt::ZOOM_OUT;
		}
		 
		/* Push changed locals to actual containers */
		if (dir.x != 0 || dir.y != 0)
			act = ipt::TURN;
		ipt::InputContainer ipt = {act, dir, txt};
		ipt::InputContainer::incli = inc;
		ipt.print(std::cout);
		ret.push_back(ipt);
	}
	return ret;
}
