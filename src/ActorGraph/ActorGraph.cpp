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

void ActorGraph::buildUnweightedGraph() {
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

                if (actors.at(left)->neighbors.find(actors.at(right)) ==
                    actors.at(left)->neighbors.end()) {
                    actors.at(left)->neighbors[actors.at(right)] = 1;
                } else {
                    actors.at(left)->neighbors[actors.at(right)] += 1;
                }
                if (actors.at(right)->neighbors.find(actors.at(left)) ==
                    actors.at(right)->neighbors.end()) {
                    actors.at(right)->neighbors[actors.at(left)] = 1;
                } else {
                    actors.at(right)->neighbors[actors.at(left)] += 1;
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

                        if (adj->nameOfActor == secondActor) {
                            for (int i = adj->dist; i >= 1; i--) {
                                path.push_back(adj->nameOfActor);
                                path.push_back(adj->nameOfConnector);
                                adj = adj->prev;
                            }
                            path.push_back(adj->nameOfActor);
                            for (unsigned int i = 0; i < nodesTouched.size();
                                 i++) {
                                nodesTouched.at(i)->dist = INT_MAX;
                            }
                            return path;
                        }

                        bfsQueue.push(adj);
                    }
                }
            }
        }
    }
    for (unsigned int i = 0; i < nodesTouched.size(); i++) {
        nodesTouched.at(i)->dist = INT_MAX;
    }
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
