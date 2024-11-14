#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include<iostream>
#include<string>
#include<vector>

class ScoreStat {
    public:
    std::string name;
    int score;
    std::string timestamp;
};

class ScoreManager {
    public:
    ScoreManager(std::string file_name);
    int GetTopScore() {return  high_score;}
    std::vector<ScoreStat> ListTopScores();
    void PutScore(int score, std::string player);
    private:
    std::string score_file;
    int high_score = 0;
    
};

#endif