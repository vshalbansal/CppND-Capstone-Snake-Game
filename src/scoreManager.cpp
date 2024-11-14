#include "scoreManager.h"
#include <sstream>
#include<fstream>
#include<ctime>
#include<algorithm>
#include<filesystem>



ScoreManager::ScoreManager(std::string file_name):score_file(file_name) {
    
    
    if(!std::filesystem::exists(file_name)){
        std::ofstream oscore_stream(score_file);
    }
    else {
        std::ifstream iscore_stream;
        iscore_stream.open(score_file,std::fstream::in);
        std::string score_line;
        if(getline(iscore_stream, score_line)) {
            std::stringstream line_str(score_line);
            std::string word;
            getline(line_str,word,',');
            getline(line_str,word,',');
            high_score = std::stoi(word);
        }
        iscore_stream.close();
    }
    
    
}

// read the list of top five scores from the score file
std::vector<ScoreStat> ScoreManager::ListTopScores() {
    
    std::ifstream iscore_stream;
    iscore_stream.open(score_file,std::fstream::in);
    std::string score_line;
    std::vector<ScoreStat> top_scores;


    while(getline(iscore_stream,score_line)) {
        std::stringstream line_str(score_line);
        std::string word;
        ScoreStat temp;
        
        // first element in the row is player name
        if(getline(line_str,word,',')) {
            temp.name = word;
        }
        
        // second element is player score
        if(getline(line_str,word,',')){
            temp.score = std::stoi(word);
        }
        
        // third element is time of scoring
        if(getline(line_str,word,',')) {
            temp.timestamp = word;
            top_scores.push_back(temp);
        }
    }
    iscore_stream.close();
    return top_scores;
}

// write the score into score file if the score is in top five scores
void ScoreManager::PutScore(int score, std::string player) {
    std::vector<ScoreStat> top_scores = ListTopScores();
    ScoreStat temp;
    temp.name = player;
    temp.score = score;
    std::time_t curr_time = std::time(nullptr);
    std::tm *tm = std::localtime(&curr_time);

    // putting time in dd/mm/yyyy format
    std::stringstream time_stream;
    time_stream<<std::put_time(tm, "%d/%m/%y %OH:%M");
    std::string timestring = time_stream.str();;
    temp.timestamp = timestring;
    top_scores.push_back(temp);

    // sort the scores in descending order
    std::sort(top_scores.begin(), top_scores.end(), [](ScoreStat &a, ScoreStat &b) {return a.score>b.score;} );
    if(top_scores.size()>5)
        top_scores.pop_back();

    high_score = top_scores.begin()->score;

    // score_file.clear();
    std::fstream oscore_stream;
    oscore_stream.open(score_file,std::fstream::in | std::fstream::out );

    // write comma seperated score records
    for(auto a:top_scores) {
        oscore_stream<<a.name<<","<<a.score<<","<<a.timestamp<<"\n";
    }
    oscore_stream.close();
    
}