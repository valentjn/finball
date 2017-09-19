#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

class GameComponentBase
{
public:
    virtual const ComponentData& output() = 0;
    virtual void update() = 0;
    virtual ~GameComponentBase() = 0;
};

inline GameComponentBase::~GameComponentBase() {}

template<class T>
class GameComponent : public GameComponentBase
{
    T wrapped;
    std::unique_ptr<T::OutputData> current_output;
    std::unique_ptr<T::OutputData> next_output;

public:
    const ComponentData& output() override
    {
        return *current_output;
    }

    void update() override
    {
        *next_output = typename T::OutputData{};
        wrapped.update(static_cast<typename T::OutputData&>(*next_output));
        current_output.swap(next_output);
    }
};

class SpecGameInteraction
{
public:
    using from_comp_t = SpecGameComponent;
    using to_comp_t = SpecGameComponent;

    void process(const typename from_comp_t::OutputData& data_from, to_comp_t& comp_to);
};

class GameInteractionBase {
public:
    virtual void process(const void* data_from, GameComponentBase& comp_to) = 0;
    virtual ~GameInteractionBase() = 0;
};

inline GameInteractionBase::~GameInteractionBase() {}

template<class T>
class GameInteraction : public GameInteractionBase {
public:
    void process(const void* data_from, GameComponentBase& comp_to) override
    {
        T::process(
            *static_cast<const typename T::from_comp_t::OutputData*>(data_from),
            static_cast<typename T::to_comp_t&>(comp_to));
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
    void run();

    template<class ComponentT>
    int addComponent() {
        mappings.push_back(ComponentMapping{});
        mappings.back().component = std::make_unique<GameComponent<ComponentT>>();
        return components.size() - 1;
    }

    template<class InteractionT>
    void addInteraction(int from, int to)
    {
        const GameComponent& comp_from = *mappings[from].component;
        mappings[to].ingoings.emplace_back({ &from, std::make_unique<GameInteraction<InteractionT>>() });
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
