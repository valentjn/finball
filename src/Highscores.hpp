#ifndef HIGHSCORES_HPP_
#define HIGHSCORES_HPP_

#include <algorithm>
#include <iostream>

class Highscores {
private:
    static void sortHighscores(std::vector<float> &highscores) {
        std::sort(highscores.begin(), highscores.end(), [=](float a, float b) -> bool { return a > b; });
    }

public:
    static void loadHighscores(std::vector<float> &highscores) {
        fstream file;
        file.open("build/haiscores.txt", fstream::in);
        if (!file.is_open()) {
            cerr << "Failed to load haiscores" << endl;
            return;
        }

        float highscore;
        while (file >> highscore) {
            highscores.push_back(highscore);
        }
        sortHighscores(highscores);
    }
};

#endif /* HIGHSCORES_HPP_ */
