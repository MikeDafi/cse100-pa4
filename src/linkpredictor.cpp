/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description: Finds the future collobrations of a node based on previous
 * colloborations with people and future colloborations based on the neighbors
 * of the neighbors of the actor. Using an actor graph.
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
#define MAX_COLLAB_PREDS 4
#define OUT_FILE_PAST_PREDICTIONS 3
#define OUT_FILE_FUTURE_PREDICTIONS 4
#define TEST_ACTORS_FILE 2
#define TAB_CHARACTER '\t'

using namespace std;

/*
 * Reads each line for the actor of which to find the collaborations for.
 * Returning the vector of each actor to find a list of collobrations for.
 *
 * Parameters:
 *              in_filename - the name of the file to read from
 *
 * Return: the vector of actors to eventually find colloborations for.
 */
vector<string> getListOfActors(const char* in_filename) {
    ifstream infile(in_filename);
    vector<string> listOfActors;

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
            if (!getline(ss, str, TAB_CHARACTER)) break;
            record.push_back(str);
        }

        if (record.size() != 1) {
            // we should have exactly 2 columns
            continue;
        }

        listOfActors.push_back(record.at(0));
    }
    infile.close();
    return listOfActors;
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
 * Using the neighbors variable of each node, the method tries to find
 * triangularization for finding the priority of the each node then sorting the
 * list as you go along.
 *
 * Parameters:
 *              outFileName - the name of the file to print out to
 *              list - the graph to read from info
 *              listOfActors - the number of actors to find relations for
 *
 * Return: None
 */
void predictFromPast(const char* outFileName, ActorGraph* list,
                     vector<string> listOfActors) {
    // header for the file
    string header = "Actor1,Actor2,Actor3,Actor4";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    // keeps track of the graph
    unordered_map<string, ActorNode*> graph = list->getGraph();

    // all firstlevel neighbors of the node from hte listOfActors
    unordered_map<string, ActorNode*> firstLevelNeighbors;

    // the priority queue for searching the collaborations
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNodePtrComp> pq;

    for (unsigned int ANodeIndex = 0; ANodeIndex < listOfActors.size();
         ANodeIndex++) {
        int currentLowestPriority = 0;
        ActorNode* curr;
        if (graph.find(listOfActors[ANodeIndex]) == graph.end()) {
            break;
        }
        curr =
            graph[listOfActors[ANodeIndex]];  // current root from listOfActors

        auto ANeighbors = curr->neighbors;

        // searches every firstLevelNeighbor for connections with a neighbor
        for (auto ANodeIterator = ANeighbors.begin();
             ANodeIterator != ANeighbors.end(); ANodeIterator++) {
            ActorNode* BNode =
                ANodeIterator->first;  // the node to find priority for
            auto BNeighbors = BNode->neighbors;
            BNode->priority = 0;
            for (auto CNodeIterator = ANeighbors.begin();
                 CNodeIterator != ANeighbors.end(); CNodeIterator++) {
                ActorNode* CNode =
                    CNodeIterator->first;  // node to find connection between
                                           // ANode and BNode

                if (CNode == BNode) continue;  // no pointing at itself
                if (BNeighbors.find(CNode) != BNeighbors.end()) {
                    BNode->priority += ANeighbors[CNode] * BNeighbors[CNode];
                }
            }

            // adds the pq for BNode
            if (pq.size() == 0 || currentLowestPriority <= BNode->priority ||
                pq.size() < MAX_COLLAB_PREDS) {
                if (pq.size() == 0) {
                    currentLowestPriority = BNode->priority;
                }
                if (currentLowestPriority > BNode->priority) {
                    currentLowestPriority = BNode->priority;
                }
                pq.push(BNode);
                if (pq.size() > MAX_COLLAB_PREDS) {
                    pq.pop();
                }
            }
        }
        vector<string> top4;

        // gets values of pq in reverse order and prints out that way
        for (int i = pq.size() - 1; i >= 0; i--) {
            top4.push_back(pq.top()->nameOfActor);
            pq.pop();
        }
        for (int i = top4.size() - 1; i >= 0; i--) {
            if (i != 0) {
                outFile << top4.at(i) << TAB_CHARACTER;
            } else {
                outFile << top4.at(i);
            }
        }

        // not enough collabs then print a tab to indicate nothingness
        if (top4.size() < MAX_COLLAB_PREDS) {
            outFile << TAB_CHARACTER;
        }
        outFile << endl;
    }
    outFile.close();
}

/*
 * Using the neighbors of the neighbors of the node currently searching for,
 * find the priority of the neighbor of the neighbor which would indicate how
 * likely they are to work with the original node. Printing to file and using
 * priority queue for assistance of sorting.
 *
 * Parameters:
 *              outFileName - the name of the file to print out to
 *              list - the graph to read from info
 *              listOfActors - the number of actors to find relations for
 *
 * Return: None
 */
void predictNew(const char* outFileName, ActorGraph* list,
                vector<string> listOfActors) {
    // prints out header
    string header = "Actor1,Actor2,Actor3,Actor4";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    // same as predictPast
    unordered_map<string, ActorNode*> graph = list->getGraph();
    unordered_map<ActorNode*, vector<ActorNode*>> secondLevelNeighbors;
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNodePtrComp> pq;

    // goes through list of actors
    for (unsigned int ANodeIndex = 0; ANodeIndex < listOfActors.size();
         ANodeIndex++) {
        secondLevelNeighbors.clear();
        ActorNode* curr;
        if (graph.find(listOfActors[ANodeIndex]) == graph.end()) {
            break;
        }
        curr = graph[listOfActors[ANodeIndex]];
        auto ANeighbors = curr->neighbors;

        // find the second-level neighbors
        for (auto ANodeIterator = ANeighbors.begin();
             ANodeIterator != ANeighbors.end(); ANodeIterator++) {
            auto BNode = ANodeIterator->first;
            auto BNeighbors = BNode->neighbors;
            for (auto BNodeIterator = BNeighbors.begin();
                 BNodeIterator != BNeighbors.end(); BNodeIterator++) {
                ActorNode* CNeighbor = BNodeIterator->first;
                if (curr == CNeighbor) continue;
                if (ANeighbors.find(CNeighbor) == ANeighbors.end()) {
                    secondLevelNeighbors[CNeighbor].push_back(BNode);
                }
            }
        }

        // iterates over each second level neighbor to find the priorities of
        // each
        for (auto secondLevelIterator = secondLevelNeighbors.begin();
             secondLevelIterator != secondLevelNeighbors.end();
             secondLevelIterator++) {
            int currentLowestPriority = 0;
            auto secondLevelNode = secondLevelIterator->first;
            secondLevelNode->priority = 0;
            auto vectorsL2 = secondLevelIterator->second;
            for (auto element : vectorsL2) {
                secondLevelNode->priority +=
                    ANeighbors[element] * secondLevelNode->neighbors[element];
            }

            // puts in queue if priority too low or not enough in queue
            if (pq.size() == 0 ||
                currentLowestPriority <= secondLevelNode->priority ||
                pq.size() < MAX_COLLAB_PREDS) {
                if (pq.size() == 0) {
                    currentLowestPriority = secondLevelNode->priority;
                }
                if (currentLowestPriority > secondLevelNode->priority) {
                    currentLowestPriority = secondLevelNode->priority;
                }
                pq.push(secondLevelNode);

                // restricts size to 4
                if (pq.size() > MAX_COLLAB_PREDS) {
                    pq.pop();
                }
            }
        }
        vector<string> top4;

        // prints out top 4 in reverse order
        for (int i = pq.size() - 1; i >= 0; i--) {
            top4.push_back(pq.top()->nameOfActor);
            pq.pop();
        }
        for (int i = top4.size() - 1; i >= 0; i--) {
            if (i != 0) {
                outFile << top4.at(i) << TAB_CHARACTER;
            } else {
                outFile << top4.at(i);
            }
        }
        if (top4.size() < MAX_COLLAB_PREDS) {
            outFile << TAB_CHARACTER;
        }
        outFile << endl;
    }

    outFile.close();
}

/*
 * Description: A terminal executable for the user to use either find a
 * prediction of collobrations based on past collabs and future neighbors.
 * Parameters: argc - the number of arguments there are
 *             argv - the strings given from the user
 * Return: -1 failed, other is success
 */
int main(int argc, char** argv) {
    const int NUM_ARG = 5;
    if (argc != NUM_ARG) {
        return -1;
    }
    ActorGraph graph;
    graph.loadFromFile(argv[1], false);
    graph.buildGraph(false);

    vector<string> listOfActors;
    listOfActors = getListOfActors(argv[TEST_ACTORS_FILE]);

    predictFromPast(argv[OUT_FILE_PAST_PREDICTIONS], &graph, listOfActors);

    predictNew(argv[OUT_FILE_FUTURE_PREDICTIONS], &graph, listOfActors);

    return 0;
}
