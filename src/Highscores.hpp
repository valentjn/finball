#ifndef HIGHSCORES_HPP_
#define HIGHSCORES_HPP_

#define HIGHSCORE_CUTOFF 10

#include <algorithm>
#include <iostream>

#include "Log.hpp"

using namespace std;

class Highscores {
private:
    static void sortHighscores(std::vector<float> &highscores) {
        sort(highscores.begin(), highscores.end(), [=](float a, float b) -> bool { return a > b; });
    }

    static void saveHighscores(vector<float> &highscores) {
        fstream file;
        file.open("haiscores.txt", fstream::out);
        if (file.is_open()) {
            for (auto const &highscore : highscores) {
                file << highscore << "\n";
            }
        } else {
            Log::error("Failed to save Haiscores");
        }
    }

public:
    static void saveHighscore(float highscore) {
        vector<float> highscores;
        loadHighscores(highscores);
        highscores.push_back(highscore);
        sortHighscores(highscores);
        highscores.resize(HIGHSCORE_CUTOFF);
        saveHighscores(highscores);
        Log::info("Saved haiscore: %f", highscore);
    }

    static void loadHighscores(std::vector<float> &highscores) {
        fstream file;
        file.open("haiscores.txt", fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load haiscores");
            return;
        }

        float highscore;
        while (file >> highscore) {
            highscores.push_back(highscore);
        }
        sortHighscores(highscores);
        highscores.resize(HIGHSCORE_CUTOFF);
    }
};

#endif /* HIGHSCORES_HPP_ */
