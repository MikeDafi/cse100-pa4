/*
 * ActorGraph.cpp
 * Author: Michael Askndafi
 * Date: 11/30/2019
 *
 * This file is meant to exist as a container for starter code that you can use
 * to read the input file format defined in imdb_2019.tsv. There are also
 * methods to find info about this graph.
 */

#include "ActorGraph.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#define CURRENT_YEAR 2019
#define THIRD_COLUMN 2
#define MOVIE_LEFT_BRACKET '['
#define MOVIE_RIGHT_BRACKET ']'
#define DELIMITER_BETWEEN_M_AND_Y "#@"

using namespace std;

// for the priority queue in minimum spanning tree
typedef pair<pair<ActorNode*, ActorNode*>, int> nodePair;

/**
 * Constructor of the Actor graph
 */
ActorGraph::ActorGraph(void) {}

/*
 * The destructor for deallocating memory for each actor node
 *
 * Parameters: None
 *
 * Return: None
 */
ActorGraph::~ActorGraph() {
    for (auto iterator = graph.begin(); iterator != graph.end(); iterator++) {
        delete iterator->second;
    }
}

/*
 * Returns the graph for debugging purposes
 *
 * Parameters: None
 *
 * Return: the hashmap of the graph
 */
unordered_map<string, ActorNode*> ActorGraph::getGraph() { return graph; }

/*
 * Looks in graph to find if the actor exists
 *
 * Parameters:
 *              actorName - the name of the actor to find existence of
 *
 * Return: true - exists actor, false - doesn't exist actor
 */
bool ActorGraph::checkIfActorExists(string actorName) {
    if (graph.find(actorName) != graph.end()) {
        return true;
    }
    return false;
}

/*
 * Using Kruskal to find all the possible weights of each edge then sorting the
 * edges from lowest weight to highest, then inserting into a graph. With each
 * insertion, the tree uses union and find operations to check for cycles and
 * path compressing the unions and finds for efficiency purposes.
 *
 * Parameters:
 *              result - a reference to all the possible efficient connections
 * of the tree
 *
 * Return: the top of the tree
 */
void ActorGraph::minimumSpanningKruskal(
    unordered_map<ActorNode*, vector<pair<ActorNode*, int>>>& result) {
    // sorts with the least weight at highest priority and in case the same
    // weight use alphabetization
    priority_queue<nodePair, vector<nodePair>, WeightedPairComparator>
        kruskalQueue;

    // pushes all possible connections including the weight of each path
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

    while (!kruskalQueue.empty()) {
        nodePair current = kruskalQueue.top();
        kruskalQueue.pop();
        ActorNode* secondNode = current.first.second;
        ActorNode* firstNode = current.first.first;

        // checks to see if they both don't exist means no possible connections
        // already exist toward them
        if (upTree[firstNode] == NULL && upTree[secondNode] == NULL) {
            upTree[secondNode] = firstNode;
            upTree[firstNode] = firstNode;
            result[firstNode].push_back(make_pair(secondNode, current.second));
            continue;
        } else {
            // keeps track of which subtree is bigger
            int secondNodeCount = 1;
            int firstNodeCount = 1;

            // remembers the original nodes found
            ActorNode* copyOfFirst = current.first.first;
            ActorNode* copyOfSecond = current.first.second;

            vector<ActorNode*> nodesOnPath;  // all the nodes that have been
                                             // touched for path compression

            while (upTree[secondNode] != secondNode) {
                nodesOnPath.push_back(secondNode);
                if (upTree[secondNode] == NULL) break;
                secondNode = upTree[secondNode];
                secondNodeCount++;
            }

            // assign all nodes to the sentinel node to skip going up the tree.
            for (auto element : nodesOnPath) {
                upTree[element] = secondNode;  // path compression
            }
            vector<ActorNode*> nodesOnPath2;

            // makes sure each node isn't pointing to itself which would mean
            // sentinel node
            while (upTree[firstNode] != firstNode) {
                nodesOnPath.push_back(firstNode);
                if (upTree[firstNode] == NULL) break;
                firstNode = upTree[firstNode];
                firstNodeCount++;
            }

            for (auto element : nodesOnPath2) {
                upTree[element] = firstNode;
            }

            // equal mean there is a cycle if this edge were to be inserted
            if (secondNode != firstNode) {
                // which tree is assigned to which other tree depends on the
                // size of each tree
                if (secondNodeCount > firstNodeCount) {
                    upTree[firstNode] = secondNode;
                } else {
                    upTree[secondNode] = firstNode;
                }

                // send the result of each edge of kruskal's algorithm
                result[copyOfFirst].push_back(
                    make_pair(copyOfSecond, current.second));
            }
        }
    }
}

/*
 * With the given movies hash map, we iterate over each element across each
 * movie, knowing that all the nodes in the movie are related to each other by
 * this movie. Using left and right pointers to assign relations to each node's
 * edge member variable.
 *
 * Parameters:
 *              usingMovieTraveler - to check for using movie traveler
 *                                  executable
 *
 * Return: None
 */
void ActorGraph::buildGraph(bool usingMovieTraveler) {
    // searches through each movie
    for (auto movieIterator = movies.begin(); movieIterator != movies.end();
         movieIterator++) {
        for (auto yearIterator = movieIterator->second.begin();
             yearIterator != movieIterator->second.end(); yearIterator++) {
            unsigned int left = 0;
            unsigned int right = 1;
            vector<ActorNode*> actors = yearIterator->second;

            // less than one actor means no neighbors
            if (actors.size() <= 1) {
                continue;
            }
            string movieTitle = movieIterator->first;
            int year = yearIterator->first;

            // makes sure left isn't at end because then where would right be,
            // off the array
            while (left != actors.size() - 1) {
                if (actors.at(left)->edges[movieTitle][year].at(0) == NULL) {
                    actors.at(left)->edges[movieTitle][year].pop_back();
                }
                actors.at(left)->edges[movieTitle][year].push_back(
                    actors[right]);

                // removes the placeholder that exists to put the actual
                // neighbor there
                if (actors.at(right)->edges[movieTitle][year].at(0) == NULL) {
                    actors.at(right)->edges[movieTitle][year].pop_back();
                }
                actors.at(right)->edges[movieTitle][year].push_back(
                    actors[left]);

                // records the number of occurrences between neighbors
                actors.at(left)->neighbors[actors.at(right)] += 1;

                actors.at(right)->neighbors[actors.at(left)] += 1;

                // only necessary when using the movie traveler function
                if (usingMovieTraveler) {
                    int movieWeight = edgeWeight(year);

                    // makes sure the neighbor hasn't existed before
                    bool rightNeighborOfLeftExists =
                        actors.at(left)->neighborsWeighted.find(actors.at(
                            right)) == actors.at(left)->neighborsWeighted.end()
                            ? false
                            : true;

                    if (!rightNeighborOfLeftExists) {
                        // keeps track of the movie weight that took the least
                        // amount of weight between actors
                        actors.at(left)->neighborsWeighted[actors.at(right)] =
                            movieWeight;

                        // keeps track of the movie that has the least weight
                        actors.at(left)
                            ->movieNeighborsWeighted[actors.at(right)] =
                            formatMovieAndYear(movieTitle, year);
                    } else {
                        // updates values in case there is a movie with less
                        // weight
                        if (actors.at(left)
                                ->neighborsWeighted[actors.at(right)] >
                            movieWeight) {
                            actors.at(left)
                                ->neighborsWeighted[actors.at(right)] =
                                movieWeight;
                        }
                    }
                }

                // moves the pointer down and up but never overlapping unless
                // finished
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
string ActorGraph::formatMovieAndYear(string movie, int year) {
    return MOVIE_LEFT_BRACKET + movie + DELIMITER_BETWEEN_M_AND_Y +
           to_string(year) + MOVIE_RIGHT_BRACKET;
}

/*
 * Using the given formula for edge weight, the method finds the weight of each
 * edge.
 *
 * Parameters:
 *              movieYear - the year of the current movie
 *
 * Return: the weight as an int
 */
int ActorGraph::edgeWeight(int movieYear) {
    return CURRENT_YEAR - movieYear + 1;
}

/*
 * Using BFS and Dijkstra, the method searches the graph, having all nodes
 * initially having the distance of INT_MAX and accounting for nodes that have
 * had their neighbors added to queue, the method uses the movie weights to find
 * the shortest distance.
 *
 * Parameters:
 *              firstActor - the string of the first actor
 *              secondActor - the string of the second actor
 *
 * Return: the path from the first actor to the second actor as a vector,
 * alternating between movie and actors to represent the path.
 */
vector<string> ActorGraph::shortestWeightedPath(string firstActor,
                                                string secondActor) {
    vector<string> path;
    vector<ActorNode*> nodesTouched;
    // an actor doesn't exist
    if (!checkIfActorExists(firstActor) || !checkIfActorExists(secondActor)) {
        return path;
    }

    // same actor, no path
    if (firstActor == secondActor) {
        return path;
    }

    priority_queue<ActorNode*, vector<ActorNode*>, WeightedComparator> bfsQueue;

    // initializes all variables for the beginning and end of searching
    ActorNode* start = graph[firstActor];
    ActorNode* end = NULL;
    start->dist = 0;
    start->done = false;
    start->prev = NULL;
    bfsQueue.push(start);

    // makes sure that queue has been checked
    while (!bfsQueue.empty()) {
        ActorNode* curr = bfsQueue.top();
        bfsQueue.pop();
        if (curr->done == false) {
            curr->done = true;

            // at this point there is no reason to search anymore
            if (curr->nameOfActor == secondActor) {
                end = curr;
                break;
            }

            // searches all possible edges
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

                        // finds the distance so far including the current
                        // neighbor's weight
                        int distance =
                            curr->dist + edgeWeight(yearIterator->first);

                        // shorter distance, better path
                        if (distance < adj->dist) {
                            // sets the values to be correlating to the curr's
                            // as the previous variable
                            nodesTouched.push_back(adj);
                            adj->dist = distance;
                            adj->prev = curr;

                            // used when forming the path later
                            adj->nameOfConnector = formatMovieAndYear(
                                movieNameIterator->first, yearIterator->first);
                            bfsQueue.push(adj);
                        }
                    }
                }
            }
        }
    }

    // makes sure that end is the node of the secondActor
    if (end != NULL && end->nameOfActor == secondActor) {
        // forms the path from the firstActor to the secondActor using a vector
        while (end->prev != NULL) {
            path.push_back(end->nameOfActor);
            path.push_back(end->nameOfConnector);
            end = end->prev;
        }
        path.push_back(end->nameOfActor);

        // resets all values when touching each node
        nodesTouched.push_back(start);
        nodesTouched.push_back(end);
        for (unsigned int i = 0; i < nodesTouched.size(); i++) {
            nodesTouched.at(i)->dist = INT_MAX;
            nodesTouched.at(i)->done = false;
            nodesTouched.at(i)->prev = NULL;
        }
        return path;
    }
}

/*
 * Using BFS and Dijkstra, the method searches the graph, having all nodes
 * initially having the distance of INT_MAX to account for each node already
 * touched, then finding the distance from the first actor to the second actor
 * and returning this path
 *
 * Parameters:
 *              firstActor - the string of the first actor
 *              secondActor - the string of the second actor
 *
 * Return: the path from the first actor to the second actor as a vector,
 * alternating between movie and actors to represent the path.
 */
vector<string> ActorGraph::shortestUnweightedPath(string firstActor,
                                                  string secondActor) {
    vector<string> path;
    vector<ActorNode*> nodesTouched;
    // an actor doesn't exist
    if (!checkIfActorExists(firstActor) || !checkIfActorExists(secondActor)) {
        return path;
    }

    // checks for the case of actors being the same
    if (firstActor == secondActor) {
        return path;
    }

    queue<ActorNode*> bfsQueue;
    ActorNode* start = graph[firstActor];

    // pushes the first node as the initial node
    start->dist = 0;
    bfsQueue.push(start);

    // if something in queue, means still nodes to traverse
    while (!bfsQueue.empty()) {
        ActorNode* curr = bfsQueue.front();
        bfsQueue.pop();

        // iterates through each edge of the starting node
        for (auto movieNameIterator = curr->edges.begin();
             movieNameIterator != curr->edges.end(); movieNameIterator++) {
            for (auto yearIterator = movieNameIterator->second.begin();
                 yearIterator != movieNameIterator->second.end();
                 yearIterator++) {
                for (unsigned int i = 0; i < yearIterator->second.size(); i++) {
                    ActorNode* adj = yearIterator->second.at(i);

                    // no node, no push to the queue
                    if (adj == NULL) {
                        break;
                    }

                    // the node hasn't been touched, checking for cycles
                    if (adj->dist == INT_MAX) {
                        nodesTouched.push_back(adj);
                        adj->dist = curr->dist + 1;
                        adj->prev = curr;
                        adj->nameOfConnector = formatMovieAndYear(
                            movieNameIterator->first, yearIterator->first);
                        bfsQueue.push(adj);
                    }

                    // found a match, closest unweighted by bfs
                    if (adj->nameOfActor == secondActor) {
                        // saves the path found by alternating through movies
                        // and actors
                        for (int i = adj->dist; i >= 1; i--) {
                            path.push_back(adj->nameOfActor);
                            path.push_back(adj->nameOfConnector);
                            adj = adj->prev;
                        }
                        path.push_back(adj->nameOfActor);

                        // resets distances of all nodes touched
                        nodesTouched.push_back(adj);
                        nodesTouched.push_back(start);
                        for (unsigned int i = 0; i < nodesTouched.size(); i++) {
                            nodesTouched.at(i)->dist = INT_MAX;
                        }
                        return path;
                    }
                }
            }
        }
    }

    // resets all touched nodes distances
    nodesTouched.push_back(start);
    for (unsigned int i = 0; i < nodesTouched.size(); i++) {
        nodesTouched.at(i)->dist = INT_MAX;
    }
    return path;
}
/** You can modify this method definition as you wish
 *
 * Load the graph from a tab-delimited file of actor->movie
 * relationships. The nadd the actor to a graph and the actor to the movie it's
 * currently at.
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
        int year = stoi(record[THIRD_COLUMN]);

        // if the actor doesn't exist, then add to the graph
        ActorNode* actorNode;
        if (graph.find(actor) == graph.end()) {
            graph[actor] = new ActorNode(actor, movie_title, year);

        } else {
            // the NULL represents the to-be connections, later found
            // when building the graph
            graph[actor]->edges[movie_title][year].push_back(NULL);
        }

        // add the actor to the movie
        movies[movie_title][year].push_back(graph[actor]);
    }

    if (!infile.eof()) {
        cerr << "Failed to read " << in_filename << "!\n";
        return false;
    }
    infile.close();

    return true;
}
