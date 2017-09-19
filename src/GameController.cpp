#include <GameController.hpp>

GameController::GameController() {}

void GameController::run() {
    while(true) {
        for (const auto& mapping : mappings) {
            GameComponent& comp_to = *mapping.component;
            for (const IngoingInteraction& ingoing : ingoings) {
                const void* data_from = ingoing.from->output();
                ingoing.process(data_from, comp_to);
            }
            component.update();
        }
    }
}

