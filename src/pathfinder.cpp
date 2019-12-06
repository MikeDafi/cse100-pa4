/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description: For a graph with either weighted or unweighted edges, the file
 * will find the shortest path between two nodes. Using streams as input and
 * output.
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
#define TAB_CHAR '\t'

using namespace std;

/*
 * Reads each line for the pair of actors to find, checking if the there are
 * exactly two columns for each pair and going through the entire stream.
 *
 * Parameters:
 *              in_filename - the name of the file to read from
 *
 * Return: the vector of actors to eventually find colloborations for.
 */
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
            if (!getline(ss, str, TAB_CHAR)) break;
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
 * Description: Find the shortest paths, first checking if the must have
 * weighted or unweighted then relying on each pair to go through Dikjstra's
 * algorithm to find the shortest path.
 *
 * Parameters: outFileName - the name of the file to print the connections to
 *             test_pairs - the pairs of actors to find connections for
 *             graph - the pre-buildt graph that has all the nodes to search
 *                      through
 *             useWeighted - for telling if should care about weights of edges
 *
 * Return: None
 */
void findShortestPaths(const char* outFileName, vector<string> test_pairs,
                       ActorGraph* graph, bool useWeighted) {
    // prints out the header
    string header = "(actor)--[movie#@year]-->(actor)--...";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    // going through each pairing
    for (unsigned int i = 0; i < test_pairs.size(); i += INCREMENTOR) {
        vector<string> returnedPath;
        bool traversor = false;  // if false then print actor, else print movie
        if (useWeighted) {
            returnedPath = graph->shortestWeightedPath(test_pairs.at(i),
                                                       test_pairs.at(i + 1));
        } else {
            returnedPath = graph->shortestUnweightedPath(test_pairs.at(i),
                                                         test_pairs.at(i + 1));
        }

        // print the path by going through the vector of strings
        for (int j = returnedPath.size() - 1; j >= 1; j--) {
            if (traversor) {
                outFile << returnedPath.at(j) << MOVIE_TO_ACTOR_TRANSITION;
            } else {
                outFile << formatActor(returnedPath.at(j))
                        << ACTOR_TO_MOVIE_TRANSITION;
            }
            traversor = !traversor;  // alternate through movie and actor
        }

        // prints out actor at the end
        if (returnedPath.size() > 1) {
            outFile << formatActor(returnedPath.at(0));
        }
        outFile << endl;
    }
    outFile.close();
}

/*
 * Description: Will use the parameters provided to build the graph,
 * load the pairs, find connections between pairings and print to the
 *  given outstream file.
 * Parameters: argc - the number of arguments there are
 *             argv - the strings given from the user
 * Return: -1 if unsuccessful, 0 if successful
 */
int main(int argc, char** argv) {
    const int NUM_ARG = 5;
    if (argc != NUM_ARG) {
        return -1;
    }

    // loads the movie file to create graph
    ActorGraph graph;
    graph.loadFromFile(argv[1], false);

    bool isWeighted;

    // checks for types of edges there are
    if (*argv[2] == UNWEIGHTED_CHAR) {
        isWeighted = false;
    } else if (*argv[2] == WEIGHTED_CHAR) {
        isWeighted = true;
    }
    graph.buildGraph(false);

    // loads the pairings of the actors
    vector<string> test_pairs;
    test_pairs = getTestPairs(argv[3]);

    findShortestPaths(argv[4], test_pairs, &graph, isWeighted);
    return 0;  // successful
}
