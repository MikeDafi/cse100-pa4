/**
 * ActorNode.hpp
 * Author: Michael Askndafi
 * Date: 11/30/2019
 * Email: maskndaf@ucsd.edu
 *
 * Description: Each actor has a node that stores it's neighbors, connections,
 * properities when finding the shortest path and more.
 * Sources:Piazza
 */
#ifndef ACTORNODE_HPP
#define ACTORNODE_HPP

#include <bits/stdc++.h>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * A class, instances of which are nodes in an ActorGraph
 */
class ActorNode {
  public:
    string nameOfActor;  // the name of the actor
    int dist;            // the distance between the previous node and this
    ActorNode* prev;     // the previous node in the sequence
    bool done;           // checks if the node has had their neighbors searched
    string nameOfConnector;  // the movie of that relates prev and this node
    int priority;  // the priority of the node when finding shortest unweighted

    // string is movie name, int is the year, vector is the connections
    unordered_map<string, unordered_map<int, vector<ActorNode*>>> edges;
    unordered_map<ActorNode*, int>
        neighbors;  // int is the number of connections
    unordered_map<ActorNode*, int> neighborsWeighted;  // int is weight
    unordered_map<ActorNode*, string> movieNeighborsWeighted;

    /* Constructor that initialize a ActorNode */
    ActorNode(string actorName, string movieName, int movieYear)
        : nameOfActor(actorName),
          dist(INT_MAX),
          done(false),
          prev(NULL),
          priority(0) {
        edges[movieName][movieYear].push_back(0);
    }
};

// for the priority queue in kruskal's algorithm
struct WeightedPairComparator {
    bool operator()(const pair<pair<ActorNode*, ActorNode*>, int>& lhs,
                    const pair<pair<ActorNode*, ActorNode*>, int>& rhs) const {
        // puts the nodes that are lower count as higher priority
        if (lhs.second != rhs.second) {
            return rhs.second < lhs.second;
        }

        // higher alphabetization goes higher in priority
        if (rhs.first.first->nameOfActor.compare(
                lhs.first.first->nameOfConnector) > 0) {
            return false;
        } else {
            return true;
        }
    }
};

// lower in distance, goes higher in priority, so does lower in string
struct WeightedComparator {
    bool operator()(ActorNode*& lhs, ActorNode*& rhs) const {
        if (lhs->dist != rhs->dist) {
            return rhs->dist < lhs->dist;
        }
        if (rhs->nameOfActor.compare(lhs->nameOfActor) > 0) {
            return false;
        } else {
            return true;
        }
    }
};

// highest priority is nodes with higher priority, self explanatory,
// or higher alphabet
struct ActorNodePtrComp {
    bool operator()(ActorNode*& lhs, ActorNode*& rhs) const {
        if (lhs->priority != rhs->priority) {
            return lhs->priority > rhs->priority;
        }

        if (rhs->nameOfActor.compare(lhs->nameOfActor) > 0) {
            return true;
        } else {
            return false;
        }
    }
};
#endif
