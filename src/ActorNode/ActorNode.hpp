/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description:
 * Sources:Piazza
 */
#ifndef HCNODE_HPP
#define HCNODE_HPP

#include <bits/stdc++.h>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

/** A class, instances of which are nodes in an HCTree.
 */
class ActorNode {
  public:
    // a map to a map of the movie name and year that maps to every actor that
    // it can
    string nameOfActor;
    int dist;
    ActorNode* prev;
    bool done;
    string nameOfConnector;
    int priority;

    // string is movie name, int is the year, vector is the connections
    unordered_map<string, unordered_map<int, vector<ActorNode*>>> edges;
    unordered_map<ActorNode*, int>
        neighbors;  // int is the number of connections
    unordered_map<ActorNode*, int> neighborsWeighted;  // int is weight
    unordered_map<ActorNode*, string> movieNeighborsWeighted;

    /* Constructor that initialize a HCNode */
    ActorNode(string actorName, string movieName, int movieYear)
        : nameOfActor(actorName),
          dist(INT_MAX),
          done(false),
          prev(NULL),
          priority(0) {
        edges[movieName][movieYear].push_back(0);
    }

    bool checkIfMovieExists(string movie_title, int year) {
        if (edges[movie_title][year].at(0) == 0) {
            return false;
        }
        return true;
    }

    /* For printing an HCNode to an ostream. Possibly useful for debugging
     */
    // ostream& operator<<(ostream&, const HCNode&) __attribute__((weak));
    // ostream& operator<<(ostream& stm, const HCNode& n) {
    //     stm << "[" << n.count << "," << (int)(n.symbol) << "]";
    //     return stm;
    // }
};

struct WeightedPairComparator {
    bool operator()(const pair<pair<ActorNode*, ActorNode*>, int>& lhs,
                    const pair<pair<ActorNode*, ActorNode*>, int>& rhs) const {
        if (lhs.second != rhs.second) {
            return rhs.second < lhs.second;
        }

        // if (rhs->nameOfActor.length() == lhs->nameOfActor.length()) {
        if (rhs.first.first->nameOfActor.compare(
                lhs.first.first->nameOfConnector) > 0) {
            return false;
        } else {
            return true;
        }
    }
};

struct WeightedComparator {
    bool operator()(ActorNode*& lhs, ActorNode*& rhs) const {
        if (lhs->dist != rhs->dist) {
            return rhs->dist < lhs->dist;
        }

        // if (rhs->nameOfActor.length() == lhs->nameOfActor.length()) {
        if (rhs->nameOfActor.compare(lhs->nameOfActor) > 0) {
            return false;
        } else {
            return true;
        }
    }
};

struct ActorNodePtrComp {
    bool operator()(ActorNode*& lhs, ActorNode*& rhs) const {
        if (lhs->priority != rhs->priority) {
            return lhs->priority > rhs->priority;
        }

        // if (rhs->nameOfActor.length() == lhs->nameOfActor.length()) {
        if (rhs->nameOfActor.compare(lhs->nameOfActor) > 0) {
            return true;
        } else {
            return false;
        }
        //}
        // else {
        //     return rhs->nameOfActor.length() < lhs->nameOfActor.length();
        // }
    }
};
#endif  // HCNODE_HPP
