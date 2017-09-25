#ifndef HIGHSCORES_HPP_
#define HIGHSCORES_HPP_

#define HIGHSCORE_CUTOFF 5

#include <algorithm>
#include <iostream>
#include <string>

#include "Log.hpp"

using namespace std;

class Highscores {
public:
    struct Highscore {
        float score;
        string name;

        Highscore(float score, string name) : score(score), name(name) {}
    };

private:
    string filename;
    vector<Highscore> highscores;

public:
    Highscores(string filename) : filename(filename) {
        loadHighscores();
    }

    void saveHighscore(float score, string name) {
        highscores.push_back(Highscore(score, name));
        sortHighscores(highscores);
        saveHighscores(highscores);
        Log::info("Saved haiscore: %f %s", score, name);
    }

    const vector<Highscore> &getHighscores() const {
        return highscores;
    }

private:
    void loadHighscores() {
        fstream file;
        file.open(filename, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load haiscores");
            return;
        }

        float score;
        string name;
        while (file >> score >> name) {
            highscores.push_back(Highscore(score, name));
        }
        sortHighscores(highscores);
    }

    void sortHighscores(vector<Highscore> &highscores) {
        sort(highscores.begin(), highscores.end(),
                [=](Highscore &a, Highscore &b) -> bool { return a.score > b.score; });

        if (highscores.size() > HIGHSCORE_CUTOFF) {
            highscores.erase(highscores.begin() + HIGHSCORE_CUTOFF);
        }
    }

    void saveHighscores(vector<Highscore> &highscores) {
        fstream file;
        file.open(filename, fstream::out);
        if (file.is_open()) {
            for (auto const &highscore : highscores) {
                file << highscore.score << " " << highscore.name << "\n";
            }
        } else {
            Log::error("Failed to save Haiscores");
        }
    }
};

#endif
