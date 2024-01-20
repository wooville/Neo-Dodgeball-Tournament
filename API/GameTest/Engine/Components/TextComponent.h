#pragma once

#include <vector>
#include <string>

struct TextComponent {
	// vector containing pairs of coordinates and strings to render at those coordinates
	std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;

	TextComponent(std::vector<std::pair<std::pair<float, float>, std::string>> textToRender = std::vector<std::pair<std::pair<float, float>, std::string>>()) {
		this->textToRender = textToRender;
	}
};