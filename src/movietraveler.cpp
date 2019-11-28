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
#include "ActorNode.hpp"

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
#define MAX_PAST_COLLAB_PREDS 4
using namespace std;

string formatActor(string actorName) {
    return ACTOR_LEFT_BRACKET + actorName + ACTOR_RIGHT_BRACKET;
}

string formatMovieAndYear(string movie, int year) {
    return MOVIE_LEFT_BRACKET + movie + DELIMITER_BETWEEN_M_AND_Y +
           to_string(year) + MOVIE_RIGHT_BRACKET;
}

void printMovieTraveler(
    ofstream& outFile, ActorNode* currentNode,
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>>& result) {
    ActorNode* firstNode;
    ActorNode* secondNode;

    int weight = 0;
    int nodesUsed = 0;
    for (auto element : result) {
        for (auto innerElement : element.second) {
            nodesUsed++;
        }
    }
    cout << nodesUsed << endl;
    for (auto iterator = result.begin(); iterator != result.end(); iterator++) {
        firstNode = iterator->first;
        for (int i = 0; i < iterator->second.size(); i++) {
            secondNode = iterator->second.at(i).first;
            weight += iterator->second.at(i).second;

            outFile << formatActor(firstNode->nameOfActor)
                    << ACTOR_TO_MOVIE_TRANSITION;

            if (firstNode->movieNeighborsWeighted.find(secondNode) !=
                firstNode->movieNeighborsWeighted.end()) {
                outFile << firstNode->movieNeighborsWeighted[secondNode];
            } else if (secondNode->movieNeighborsWeighted.find(firstNode) !=
                       secondNode->movieNeighborsWeighted.end()) {
                outFile << secondNode->movieNeighborsWeighted[firstNode];
            }
            outFile << MOVIE_TO_ACTOR_TRANSITION;
            outFile << formatActor(secondNode->nameOfActor) << endl;
        }
    }
    cout << "NODES WEIGHT " << weight << endl;
    outFile.close();
}

void minimumSpanningTree(const char* outFileName, ActorGraph* graph) {
    unordered_map<ActorNode*, ActorNode*> upTree;
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>> result;
    ActorNode* root = graph->minimumSpanningKruskal(result);

    string header = "(actor)--[movie#@year]-->(actor)--...";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    printMovieTraveler(outFile, root, result);

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
    const int NUM_ARG = 3;
    if (argc != NUM_ARG) {
        return -1;
    }
    ActorGraph* graph = new ActorGraph();
    graph->loadFromFile(argv[1], false);
    graph->buildGraph(true);

    minimumSpanningTree(argv[2], graph);

    delete graph;
    return 0;
}
