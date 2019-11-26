/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description: TODO
 * Sources: Piazza
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "ActorGraph.cpp"

#define INCREMENTOR 2
#define UNWEIGHTED_CHAR 'u'
#define WEIGHTED_CHAR 'w'
#define ACTOR_TO_MOVIE_TRANSITION "--"
#define MOVIE_TO_ACTOR_TRANSITION "-->"
#define MOVIE_LEFT_BRACKET '['
#define MOVIE_RIGHT_BRACKET ']'
#define ACTOR_LEFT_BRACKET '('
#define ACTOR_RIGHT_BRACKET ')'
#define DELIMITER_BETWEEN_M_AND_Y "#@"
#define TWO_COLUMNS 2
using namespace std;

vector<string> getTestPairs(const char* in_filename) {
    ifstream infile(in_filename);
    vector<string> test_pairs;

    bool have_header = false;

    // keep reading lines until the end of file is reached
    while (infile) {
        string s;

        // get the next line
        if (!getline(infile, s)) break;

        if (!have_header) {
            // skip the header
            have_header = true;
            continue;
        }

        istringstream ss(s);
        vector<string> record;

        while (ss) {
            string str;

            // get the next string before hitting a tab character
            // and put it in 'str'
            if (!getline(ss, str, '\t')) break;
            record.push_back(str);
        }

        if (record.size() != TWO_COLUMNS) {
            // we should have exactly 2 columns
            continue;
        }

        test_pairs.push_back(record.at(0));
        test_pairs.push_back(record.at(1));
    }
    infile.close();
    return test_pairs;
}

string formatActor(string actorName) {
    return ACTOR_LEFT_BRACKET + actorName + ACTOR_RIGHT_BRACKET;
}

void findShortestPaths(const char* outFileName, vector<string> test_pairs,
                       ActorGraph* graph, bool useWeighted) {
    string header = "(actor)--[movie#@year]-->(actor)--...";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    for (unsigned int i = 0; i < test_pairs.size(); i += INCREMENTOR) {
        vector<string> returnedPath;
        bool traversor = false;
        if (useWeighted) {
            returnedPath = graph->shortestWeightedPath(test_pairs.at(i),
                                                       test_pairs.at(i + 1));
        } else {
            returnedPath = graph->shortestUnweightedPath(test_pairs.at(i),
                                                         test_pairs.at(i + 1));
        }

        for (int j = returnedPath.size() - 1; j >= 1; j--) {
            if (traversor) {
                outFile << returnedPath.at(j) << MOVIE_TO_ACTOR_TRANSITION;
            } else {
                outFile << formatActor(returnedPath.at(j))
                        << ACTOR_TO_MOVIE_TRANSITION;
            }
            traversor = !traversor;
        }

        if (returnedPath.size() > 1) {
            outFile << formatActor(returnedPath.at(0));
        }
        outFile << endl;
    }
    outFile.close();
}

/*
 * Description: A terminal interface for the user to use either predict
 * completions or predict underscore.
 * Parameters: argc - the number of arguments there are
 *             argv - the strings given from the user
 * Return: TODO
 */
int main(int argc, char** argv) {
    const int NUM_ARG = 5;
    if (argc != NUM_ARG) {
        return -1;
    }
    ActorGraph* graph = new ActorGraph();
    graph->loadFromFile(argv[1], false);

    bool isWeighted;
    if (*argv[2] == UNWEIGHTED_CHAR) {
        isWeighted = false;
    } else if (*argv[2] == WEIGHTED_CHAR) {
        isWeighted = true;
    }
    graph->buildGraph();

    vector<string> test_pairs;
    test_pairs = getTestPairs(argv[3]);

    findShortestPaths(argv[4], test_pairs, graph, isWeighted);
    delete graph;
    return 0;
}
