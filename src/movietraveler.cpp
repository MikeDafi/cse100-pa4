/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description: Using Kruskal algorithm and a union/find up-tree, the file
 * constructs an efficient way of navigating to each actor across time.
 * The file then prints out the routes to each actor.
 * Sources: Piazza
 */
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "ActorGraph.cpp"
#include "ActorNode.hpp"

#define ACTOR_TO_MOVIE_TRANSITION "--"
#define MOVIE_TO_ACTOR_TRANSITION "-->"
#define MOVIE_LEFT_BRACKET '['
#define MOVIE_RIGHT_BRACKET ']'
#define ACTOR_LEFT_BRACKET '('
#define ACTOR_RIGHT_BRACKET ')'
#define DELIMITER_BETWEEN_M_AND_Y "#@"
#define OUTPUT_FILE_ARG 2

using namespace std;

/*
 * Formally write the way an actor should be written for the output file
 *
 * Parameters: actorName - the name of the actor to format
 *
 * Return: the newly formatted string
 */
string formatActor(string actorName) {
    return ACTOR_LEFT_BRACKET + actorName + ACTOR_RIGHT_BRACKET;
}

/*
 * For the proper format of movies and year, the method formally returns the
 * correct semantics of a movie and year
 *
 * Parameters:
 *              movie - the movie to be altered
 *              year - the year to be concatenated to the movie
 *
 * Return: the formal version of the movie and year
 */
string formatMovieAndYear(string movie, int year) {
    return MOVIE_LEFT_BRACKET + movie + DELIMITER_BETWEEN_M_AND_Y +
           to_string(year) + MOVIE_RIGHT_BRACKET;
}

/*
 * Printing in the format actor--movie-->actor to the outFile that are the most
 * efficient.
 *
 * Parameters:
 *              outFile - the stream to print out the connections
 *              result - each connection from an actor
 *
 * Return: None
 */
void printMovieTraveler(
    ofstream& outFile,
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>>& result) {
    // the two actors in each connection
    ActorNode* firstNode;
    ActorNode* secondNode;
 
    // goes through each actor's all possible connections
    for (auto iterator = result.begin(); iterator != result.end(); iterator++) {
        firstNode = iterator->first;
        for (int i = 0; i < iterator->second.size(); i++) {
            secondNode = iterator->second.at(i).first;

            outFile << formatActor(firstNode->nameOfActor)
                    << ACTOR_TO_MOVIE_TRANSITION;

            // finds the connector movie across each node cause the connection
            // isn't in both
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
}

/*
 * Finds the spanning tree relying on the graph's member method and then prints
 * out the header and rest of connections
 *
 * Parameters:
 *              outFileName - string to stream to print out the connections
 *              graph - the graph of the nodes to find the MST
 *
 * Return: None
 */
void minimumSpanningTree(const char* outFileName, ActorGraph*& graph) {
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>> result;
    graph->minimumSpanningKruskal(result);

    // header for the file
    string header = "(actor)--[movie#@year]-->(actor)--...";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    printMovieTraveler(outFile, result);

    outFile.close();
}

/*
 * Description: A terminal executable that prints all the possible connections
 * for the minimum spanning tree in one of the given args.
 *  Parameters:
 *              argc - the number of arguments there are
 *              argv - the strings given from the user
 * Return: -1 if unsuccessful, otherwise successful
 */
int main(int argc, char** argv) {
    const int NUM_ARG = 3;
    if (argc != NUM_ARG) {
        return -1;
    }
    ActorGraph* graph = new ActorGraph();
    graph->loadFromFile(argv[1], false);
    graph->buildGraph(true);

    minimumSpanningTree(argv[OUTPUT_FILE_ARG], graph);

    delete graph;
    return 0;
}
