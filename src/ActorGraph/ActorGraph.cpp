/*
 * ActorGraph.cpp
 * Author: <YOUR NAME HERE>
 * Date:   <DATE HERE>
 *
 * This file is meant to exist as a container for starter code that you can use
 * to read the input file format defined in imdb_2019.tsv. Feel free to modify
 * any/all aspects as you wish.
 */

#include "ActorGraph.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#define MOVIE_LEFT_BRACKET '['
#define MOVIE_RIGHT_BRACKET ']'
#define DELIMITER_BETWEEN_M_AND_Y "#@"

using namespace std;

typedef pair<pair<ActorNode*, ActorNode*>, int> nodePair;

/**
 * Constructor of the Actor graph
 */
ActorGraph::ActorGraph(void) {}
ActorGraph::~ActorGraph() {
    for (auto iterator = graph.begin(); iterator != graph.end(); iterator++) {
        delete iterator->second;
    }
}
unordered_map<string, ActorNode*> ActorGraph::getGraph() { return graph; }

bool ActorGraph::checkIfActorExists(string actorName) {
    if (graph.find(actorName) != graph.end()) {
        return true;
    }
    return false;
}

ActorNode* ActorGraph::minimumSpanningKruskal(
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>>& result) {
    priority_queue<nodePair, vector<nodePair>, WeightedPairComparator>
        kruskalQueue;

    for (auto graphIterator = graph.begin(); graphIterator != graph.end();
         graphIterator++) {
        ActorNode* currentNode = graphIterator->second;
        for (auto neighborIterator = currentNode->neighborsWeighted.begin();
             neighborIterator != currentNode->neighborsWeighted.end();
             neighborIterator++) {
            kruskalQueue.push(
                make_pair(make_pair(currentNode, neighborIterator->first),
                          neighborIterator->second));
        }
    }
    unordered_map<ActorNode*, ActorNode*> upTree;
    for (auto element : graph) {
        upTree[element.second] = NULL;
    }

    ActorNode* root = NULL;
    while (!kruskalQueue.empty()) {
        nodePair current = kruskalQueue.top();
        kruskalQueue.pop();
        ActorNode* secondNode = current.first.second;
        ActorNode* firstNode = current.first.first;
        if (upTree[firstNode] == NULL && upTree[secondNode] == NULL) {
            upTree[secondNode] = firstNode;
            upTree[firstNode] = firstNode;
            root = firstNode;
            result[firstNode].push_back(make_pair(secondNode, current.second));
            continue;
        } else {
            int secondNodeCount = 1;
            int firstNodeCount = 1;
            ActorNode* copyOfFirst = current.first.first;
            ActorNode* copyOfSecond = current.first.second;
            vector<ActorNode*> nodesOnPath;
            while (upTree[secondNode] != secondNode) {
                nodesOnPath.push_back(secondNode);
                if (upTree[secondNode] == NULL) break;
                secondNode = upTree[secondNode];
                secondNodeCount++;
            }
            for (auto element : nodesOnPath) {
                upTree[element] = secondNode;
            }
            vector<ActorNode*> nodesOnPath2;
            while (upTree[firstNode] != firstNode) {
                nodesOnPath.push_back(firstNode);
                if (upTree[firstNode] == NULL) break;
                firstNode = upTree[firstNode];
                firstNodeCount++;
            }

            for (auto element : nodesOnPath2) {
                upTree[element] = firstNode;
            }

            if (secondNode != firstNode) {
                if (secondNodeCount > firstNodeCount) {
                    upTree[firstNode] = secondNode;
                    root = secondNode;
                } else {
                    upTree[secondNode] = firstNode;
                    root = firstNode;
                }

                result[copyOfFirst].push_back(
                    make_pair(copyOfSecond, current.second));
            }
        }
    }
}

void ActorGraph::buildGraph(bool usingMovieTraveler) {
    for (auto movieIterator = movies.begin(); movieIterator != movies.end();
         movieIterator++) {
        for (auto yearIterator = movieIterator->second.begin();
             yearIterator != movieIterator->second.end(); yearIterator++) {
            unsigned int left = 0;
            unsigned int right = 1;
            vector<ActorNode*> actors = yearIterator->second;
            if (actors.size() <= 1) {
                continue;
            }
            string movieTitle = movieIterator->first;
            int year = yearIterator->first;
            int movieWeight = edgeWeight(year);

            while (left != actors.size() - 1) {
                if (actors.at(left)->edges[movieTitle][year].at(0) == NULL) {
                    actors.at(left)->edges[movieTitle][year].pop_back();
                }
                actors.at(left)->edges[movieTitle][year].push_back(
                    actors[right]);

                if (actors.at(right)->edges[movieTitle][year].at(0) == NULL) {
                    actors.at(right)->edges[movieTitle][year].pop_back();
                }
                actors.at(right)->edges[movieTitle][year].push_back(
                    actors[left]);

                bool rightNeighborOfLeftExists =
                    actors.at(left)->neighbors.find(actors.at(right)) ==
                            actors.at(left)->neighbors.end()
                        ? false
                        : true;

                bool leftNeighborOfRightExists =
                    actors.at(right)->neighbors.find(actors.at(left)) ==
                            actors.at(right)->neighbors.end()
                        ? false
                        : true;

                if (rightNeighborOfLeftExists) {
                    actors.at(left)->neighbors[actors.at(right)] = 1;
                } else {
                    actors.at(left)->neighbors[actors.at(right)] += 1;
                }
                if (leftNeighborOfRightExists) {
                    actors.at(right)->neighbors[actors.at(left)] = 1;
                } else {
                    actors.at(right)->neighbors[actors.at(left)] += 1;
                }

                if (usingMovieTraveler) {
                    if (!rightNeighborOfLeftExists) {
                        actors.at(left)->neighborsWeighted[actors.at(right)] =
                            movieWeight;
                        actors.at(left)
                            ->movieNeighborsWeighted[actors.at(right)] =
                            formatMovieAndYear(movieTitle, year);
                    } else {
                        if (actors.at(left)
                                ->neighborsWeighted[actors.at(right)] >
                            movieWeight) {
                            actors.at(left)
                                ->neighborsWeighted[actors.at(right)] =
                                movieWeight;
                        }
                    }
                    // if (leftNeighborOfRightExists) {
                    //     actors.at(right)->neighborsWeighted[actors.at(left)]
                    //     =
                    //         movieWeight;
                    // } else {
                    //     if (actors.at(right)
                    //             ->neighborsWeighted[actors.at(left)] >
                    //         movieWeight) {
                    //         actors.at(right)
                    //             ->neighborsWeighted[actors.at(left)] =
                    //             movieWeight;
                    //     }
                    // }
                }

                if (actors.size() - 1 == right) {
                    left++;
                    right = left + 1;
                } else {
                    right++;
                }
            }
        }
    }
}

string ActorGraph::formatMovieAndYear(string movie, int year) {
    return MOVIE_LEFT_BRACKET + movie + DELIMITER_BETWEEN_M_AND_Y +
           to_string(year) + MOVIE_RIGHT_BRACKET;
}

int ActorGraph::edgeWeight(int movieYear) { return 2019 - movieYear + 1; }

vector<string> ActorGraph::shortestWeightedPath(string firstActor,
                                                string secondActor) {
    vector<string> path;
    vector<ActorNode*> nodesTouched;
    // an actor doesn't exist
    if (!checkIfActorExists(firstActor) || !checkIfActorExists(secondActor)) {
        return path;
    }

    if (firstActor == secondActor) {
        return path;
    }

    priority_queue<ActorNode*, vector<ActorNode*>, WeightedComparator> bfsQueue;
    ActorNode* start = graph[firstActor];
    start->dist = 0;
    bfsQueue.push(start);
    while (!bfsQueue.empty()) {
        ActorNode* curr = bfsQueue.top();
        bfsQueue.pop();
        if (curr->done == false) {
            curr->done = true;
            for (auto movieNameIterator = curr->edges.begin();
                 movieNameIterator != curr->edges.end(); movieNameIterator++) {
                for (auto yearIterator = movieNameIterator->second.begin();
                     yearIterator != movieNameIterator->second.end();
                     yearIterator++) {
                    for (unsigned int i = 0; i < yearIterator->second.size();
                         i++) {
                        ActorNode* adj = yearIterator->second.at(i);
                        if (adj == NULL) {
                            break;
                        }
                        int distance =
                            curr->dist + edgeWeight(yearIterator->first);
                        if (distance < adj->dist) {
                            nodesTouched.push_back(adj);
                            adj->dist = distance;
                            adj->prev = curr;
                            adj->nameOfConnector = formatMovieAndYear(
                                movieNameIterator->first, yearIterator->first);

                            if (adj->nameOfActor == secondActor) {
                                while (adj->prev != NULL) {
                                    path.push_back(adj->nameOfActor);
                                    path.push_back(adj->nameOfConnector);
                                    adj = adj->prev;
                                }
                                path.push_back(adj->nameOfActor);
                                for (unsigned int i = 0;
                                     i < nodesTouched.size(); i++) {
                                    nodesTouched.at(i)->dist = INT_MAX;
                                    nodesTouched.at(i)->done = false;
                                    nodesTouched.at(i)->prev = NULL;
                                }
                                return path;
                            }

                            bfsQueue.push(adj);
                        }
                    }
                }
            }
        }
    }
    for (unsigned int i = 0; i < nodesTouched.size(); i++) {
        nodesTouched.at(i)->dist = INT_MAX;
        nodesTouched.at(i)->done = false;
        nodesTouched.at(i)->prev = NULL;
    }
    return path;
}

vector<string> ActorGraph::shortestUnweightedPath(string firstActor,
                                                  string secondActor) {
    vector<string> path;
    vector<ActorNode*> nodesTouched;
    // an actor doesn't exist
    if (!checkIfActorExists(firstActor) || !checkIfActorExists(secondActor)) {
        return path;
    }

    if (firstActor == secondActor) {
        return path;
    }

    queue<ActorNode*> bfsQueue;
    ActorNode* start = graph[firstActor];
    start->dist = 0;
    bfsQueue.push(start);
    while (!bfsQueue.empty()) {
        ActorNode* curr = bfsQueue.front();
        bfsQueue.pop();
        for (auto movieNameIterator = curr->edges.begin();
             movieNameIterator != curr->edges.end(); movieNameIterator++) {
            for (auto yearIterator = movieNameIterator->second.begin();
                 yearIterator != movieNameIterator->second.end();
                 yearIterator++) {
                for (unsigned int i = 0; i < yearIterator->second.size(); i++) {
                    ActorNode* adj = yearIterator->second.at(i);
                    if (adj == NULL) {
                        break;
                    }

                    if (adj->dist == INT_MAX) {
                        nodesTouched.push_back(adj);
                        adj->dist = curr->dist + 1;
                        adj->prev = curr;
                        adj->nameOfConnector = formatMovieAndYear(
                            movieNameIterator->first, yearIterator->first);
                        bfsQueue.push(adj);
                    }
                    if (adj->nameOfActor == secondActor) {
                        for (int i = adj->dist; i >= 1; i--) {
                            path.push_back(adj->nameOfActor);
                            path.push_back(adj->nameOfConnector);
                            adj = adj->prev;
                        }
                        path.push_back(adj->nameOfActor);
                        for (unsigned int i = 0; i < nodesTouched.size(); i++) {
                            nodesTouched.at(i)->dist = INT_MAX;
                        }
                        adj->dist = INT_MAX;
                        start->dist = INT_MAX;
                        return path;
                    }
                }
            }
        }
    }
    for (unsigned int i = 0; i < nodesTouched.size(); i++) {
        nodesTouched.at(i)->dist = INT_MAX;
    }
    start->dist = INT_MAX;
    return path;
}
/** You can modify this method definition as you wish
 *
 * Load the graph from a tab-delimited file of actor->movie
 * relationships.
 *
 * in_filename - input filename
 * use_weighted_edges - if true, compute edge weights as 1 + (2019 -
 * movie_year), otherwise all edge weights will be 1
 *
 * return true if file was loaded sucessfully, false otherwise
 */
bool ActorGraph::loadFromFile(const char* in_filename,
                              bool use_weighted_edges) {
    // Initialize the file stream
    ifstream infile(in_filename);

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

        if (record.size() != 3) {
            // we should have exactly 3 columns
            continue;
        }

        string actor(record[0]);
        string movie_title(record[1]);
        int year = stoi(record[2]);

        ActorNode* actorNode;
        if (graph.find(actor) == graph.end()) {
            graph[actor] = new ActorNode(actor, movie_title, year);

        } else {
            // the NULL represents the to-be connections
            graph[actor]->edges[movie_title][year].push_back(NULL);
        }

        movies[movie_title][year].push_back(graph[actor]);

        // TODO: we have an actor/movie relationship, now what?
    }
    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();

    return true;
}
