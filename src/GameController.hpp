#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <memory>
#include <vector>

class GameComponentBase
{
    friend class GameController;

    virtual const void* output() const = 0;
    virtual bool update() = 0;

public:
    virtual ~GameComponentBase() = 0;
};

inline GameComponentBase::~GameComponentBase() {}

template<class T>
class GameComponent : public GameComponentBase
{
    template<class Other>
    friend class GameInteraction;

    T wrapped;
    std::unique_ptr<typename T::OutputData> current_output = std::make_unique<typename T::OutputData>();
    std::unique_ptr<typename T::OutputData> next_output = std::make_unique<typename T::OutputData>();

    const void* output() const override
    {
        return current_output.get();
    }

    bool update() override
    {
        *next_output = typename T::OutputData{};
        bool res = wrapped.update(*next_output);
        current_output.swap(next_output);
        return res;
    }
};

class GameInteractionBase {
    friend class GameController;

    virtual void process(const void* data_from, GameComponentBase& comp_to) const = 0;

public:
    virtual ~GameInteractionBase() = 0;
};

inline GameInteractionBase::~GameInteractionBase() {}

template<class T>
class GameInteraction : public GameInteractionBase {
    void process(const void* data_from, GameComponentBase& comp_to) const override
    {
        T::process(
            *static_cast<const typename T::from_comp_t::OutputData*>(data_from),
            static_cast<GameComponent<typename T::to_comp_t>&>(comp_to).wrapped);
    }
};

class GameController {
    struct IngoingInteraction {
        const GameComponentBase* from;
        std::unique_ptr<const GameInteractionBase> interaction;
    };

    struct ComponentMapping {
        std::unique_ptr<GameComponentBase> component;
        std::vector<IngoingInteraction> ingoings;
    };

    std::vector<ComponentMapping> mappings;

public:
    GameController();
    void run() const;

    template<class ComponentT>
    int addComponent() {
        mappings.push_back(ComponentMapping{});
        mappings.back().component = std::make_unique<GameComponent<ComponentT>>();
        return mappings.size() - 1;
    }

    template<class InteractionT>
    void addInteraction(int from, int to)
    {
        const GameComponentBase& comp_from = *mappings[from].component;
        mappings[to].ingoings.push_back(IngoingInteraction{ &comp_from, std::make_unique<GameInteraction<InteractionT>>() });
    }


    /*void startGame(Array2D<LevelLoader::CellType> *level)
    {
        GameLogic gameLogic(parameters);
        UserInput userInput(parameters);
        LatticeBoltzmann latticeBoltzmann(parameters);
        RigidBody rigidBody(parameters);
        Renderer renderer(parameters);

        UserInputOutput userInputOutput;
        LatticeBoltzmannOutput latticeBoltzmannOutput;
        RigidBodyOutput rigidBodyOutput;
        GameLogicOutput gameLogicOutput;

        LatticeBoltzmannInput latticeBoltzmannInput;
        RigidBodyInput rigidBodyInput;
        GameLogicInput gameLogicInput;
        RendererInput rendererInput;

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            userInput.getInput(userInputOutput);

            // 2. do calculations (rigid body, LBM)
            latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyOutput);
            latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

            rigidBodyInput = RigidBodyInput(userInputOutput, latticeBoltzmannOutput);
            rigidBody.compute(rigidBodyInput, rigidBodyOutput);

            gameLogicInput = GameLogicInput(userInputOutput, rigidBodyOutput, latticeBoltzmannOutput);
            gameLogic.update(gameLogicInput, gameLogicOutput);

            // 3. draw visualization
            rendererInput = RendererInput(rigidBodyOutput, latticeBoltzmannOutput);
            renderer.render(rendererInput);
        }
    }*/
};

#endif
