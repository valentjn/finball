#ifndef COMPUI_HPP_
#define COMPUI_HPP_

#include <Level.hpp>

class CompUI
{
public:
	// output type of this component
    struct OutputData
	{
		// TODO: add output fields
	};

	CompUI(const Level& level) {}

	// writes the output of this component to output
    bool update(OutputData& output)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				return false;

			// TODO: handle more events
			}
		}
		return true;
	}
};

#endif
