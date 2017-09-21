#include <GameController.hpp>
#include <iostream>

GameController::GameController(Level& level) : m_level(level) {}

void GameController::run() const
{
    while(true) {
        for (const auto& mapping : mappings) {
            GameComponentBase& component = *mapping.component;
            for (const IngoingInteraction& ingoing : mapping.ingoings) {
                const void* data_from = ingoing.from->output();
                ingoing.interaction->process(data_from, component);
            }
            if (!component.update())
                return;
        }
    }
}

