/* Contains everything related to raw key processing */
#pragma once
#include <unordered_map>
#include "util.hpp"
#include "iostream"
#include "vector"
#include "cli.hpp"
#include "settings.hpp"
#include "infra.hpp"
#include "SFML/Graphics.hpp"

namespace ipt
{
	const std::unordered_map<sf::Keyboard::Key, std::wstring> cmdmap = 
	{
		{sf::Keyboard::Key::K, L"move north"},
		{sf::Keyboard::Key::U, L"move northeast"},
		{sf::Keyboard::Key::L, L"move east"},
		{sf::Keyboard::Key::N, L"move southeast"},
		{sf::Keyboard::Key::J, L"move south"},
		{sf::Keyboard::Key::B, L"move southwest"},
		{sf::Keyboard::Key::H, L"move west"},
		{sf::Keyboard::Key::Y, L"move northwest"},
		{sf::Keyboard::Key::Comma, L"pickup"},
		{sf::Keyboard::Key::Space, L"cli enter"},
		{sf::Keyboard::Key::Escape, L"cli exit"},
		{sf::Keyboard::Key::Add, L"zoom in"},
		{sf::Keyboard::Key::Subtract, L"zoom out"}
	};
	std::vector<cli::LogEntry> process_input
	(
		std::wstring input,
		ecs::Aggregate&,
		cli::CliData&,
		in::GfxManager&,
		SettingContainer&,
		bool& skiptxt
	);
}
