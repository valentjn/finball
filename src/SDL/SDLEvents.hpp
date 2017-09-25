#ifndef SDL_EVENTS_HPP_
#define SDL_EVENTS_HPP_

#include <functional>
#include <unordered_map>

#include <SDL2/SDL.h>

class SDLEvents {
public:
    typedef std::function<void(SDL_Event&)> listener;

private:
    std::unordered_map<int, listener> listeners;

public:
    void setListener(int event, listener listener) {
        listeners[event] = listener;
    }

    void listen(const bool &running) {
        SDL_Event event;

        while (running) {
            SDL_WaitEvent(&event);

            auto listener = listeners.find(event.type);

            if (listener != listeners.end()) {
                listener->second(event);
            }
        }
    }
};

#endif
