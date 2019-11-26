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
            if (!getline(ss, str, '\t')) break;
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

    outFile.close();
}

void predictFromPast(const char* outFileName, ActorGraph* list,
                     vector<string> listOfActors) {
    string header = "Actor1,Actor2,Actor3,Actor4";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    unordered_map<string, ActorNode*> graph = list->getGraph();
    unordered_map<string, ActorNode*> firstLevelNeighbors;
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNodePtrComp> pq;
    for (unsigned int ANodeIndex = 0; ANodeIndex < listOfActors.size();
         ANodeIndex++) {
        int currentLowestPriority = 0;
        ActorNode* curr;
        if (graph.find(listOfActors[ANodeIndex]) == graph.end()) {
            break;
        }
        curr = graph[listOfActors[ANodeIndex]];
        auto ANeighbors = curr->neighbors;
        for (auto ANodeIterator = ANeighbors.begin();
             ANodeIterator != ANeighbors.end(); ANodeIterator++) {
            ActorNode* BNode = ANodeIterator->first;
            auto BNeighbors = BNode->neighbors;
            BNode->priority = 0;
            for (auto CNodeIterator = ANeighbors.begin();
                 CNodeIterator != ANeighbors.end(); CNodeIterator++) {
                ActorNode* CNode = CNodeIterator->first;
                if (CNode == BNode) continue;
                if (BNeighbors.find(CNode) != BNeighbors.end()) {
                    BNode->priority += ANeighbors[CNode] * BNeighbors[CNode];
                }
            }

            if (pq.size() == 0 || currentLowestPriority <= BNode->priority ||
                pq.size() < MAX_PAST_COLLAB_PREDS) {
                if (pq.size() == 0) {
                    currentLowestPriority = BNode->priority;
                }
                if (currentLowestPriority > BNode->priority) {
                    currentLowestPriority = BNode->priority;
                }
                pq.push(BNode);
                if (pq.size() > MAX_PAST_COLLAB_PREDS) {
                    pq.pop();
                }
            }
        }
        vector<string> top4;

        for (int i = pq.size() - 1; i >= 0; i--) {
            top4.push_back(pq.top()->nameOfActor);
            pq.pop();
        }
        for (int i = top4.size() - 1; i >= 0; i--) {
            if (i != 0) {
                outFile << top4.at(i) << '\t';
            } else {
                outFile << top4.at(i);
            }
        }
        if (top4.size() < 4) {
            outFile << '\t';
        }
        outFile << endl;
    }
    outFile.close();
}

void predictNew(const char* outFileName, ActorGraph* list,
                vector<string> listOfActors) {
    string header = "Actor1,Actor2,Actor3,Actor4";
    remove(outFileName);
    ofstream outFile;
    outFile.open(outFileName);
    outFile << header << endl;

    unordered_map<string, ActorNode*> graph = list->getGraph();
    unordered_map<ActorNode*, vector<ActorNode*>> secondLevelNeighbors;
    priority_queue<ActorNode*, vector<ActorNode*>, ActorNodePtrComp> pq;

    for (unsigned int ANodeIndex = 0; ANodeIndex < listOfActors.size();
         ANodeIndex++) {
        secondLevelNeighbors.clear();
        ActorNode* curr;
        if (graph.find(listOfActors[ANodeIndex]) == graph.end()) {
            break;
        }
        curr = graph[listOfActors[ANodeIndex]];
        auto ANeighbors = curr->neighbors;
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
            if (pq.size() == 0 ||
                currentLowestPriority <= secondLevelNode->priority ||
                pq.size() < MAX_PAST_COLLAB_PREDS) {
                if (pq.size() == 0) {
                    currentLowestPriority = secondLevelNode->priority;
                }
                if (currentLowestPriority > secondLevelNode->priority) {
                    currentLowestPriority = secondLevelNode->priority;
                }
                pq.push(secondLevelNode);
                if (pq.size() > MAX_PAST_COLLAB_PREDS) {
                    pq.pop();
                }
            }
        }
        vector<string> top4;

        for (int i = pq.size() - 1; i >= 0; i--) {
            top4.push_back(pq.top()->nameOfActor);
            pq.pop();
        }
        for (int i = top4.size() - 1; i >= 0; i--) {
            if (i != 0) {
                outFile << top4.at(i) << '\t';
            } else {
                outFile << top4.at(i);
            }
        }
        if (top4.size() < 4) {
            outFile << '\t';
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
    graph->buildGraph();

    vector<string> listOfActors;
    listOfActors = getListOfActors(argv[2]);

    predictFromPast(argv[3], graph, listOfActors);

    predictNew(argv[4], graph, listOfActors);

    delete graph;
    return 0;
}
