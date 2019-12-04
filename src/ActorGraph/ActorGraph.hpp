/*
 * ActorGraph.hpp
 * Author: Michael Askndafi
 * Date:   11/15/2019
 *
 * This file is meant to exist as a container for starter code that you can use
 * to read the input file format defined imdb_2019.tsv. There are also methods
 * to find info about the graph.
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>
#include <unordered_map>
#include "ActorNode.hpp"

using namespace std;

/**
 * The ActorGraph that stores the connections and methods to build the graph and
 * find info about the graph.
 */
class ActorGraph {
  protected:
    // the graph relating the actor's names to their nodes
    unordered_map<string, ActorNode*> graph;

    // the hashmap of the movies and the actors having relations with this movie
    unordered_map<string, unordered_map<int, vector<ActorNode*>>> movies;

  public:
    /**
     * Constuctor of the Actor graph
     */
    ActorGraph(void);

    /*
     * The destructor for deallocating memory for each actor node
     *
     * Parameters: None
     *
     * Return: None
     */
    ~ActorGraph();

    /*
     * Returns the graph for debugging purposes
     *
     * Parameters: None
     *
     * Return: the hashmap of the graph
     */
    unordered_map<string, ActorNode*> getGraph();

    /*
     * Using the given formula for edge weight, the method finds the weight of
     * each edge.
     *
     * Parameters:
     *              movieYear - the year of the current movie
     *
     * Return: the weight as an int
     */
    int edgeWeight(int movieYear);

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
    string formatMovieAndYear(string movie, int year);

    /*
     * Looks in graph to find if the actor exists
     *
     * Parameters:
     *              actorName - the name of the actor to find existence of
     *
     * Return: true - exists actor, false - doesn't exist actor
     */
    bool checkIfActorExists(string actorName);

    /*
     * With the given movies hash map, we iterate over each element across each
     * movie, knowing that all the nodes in the movie are related to each other
     * by this movie. Using left and right pointers to assign relations to each
     * node's edge member variable.
     *
     * Parameters:
     *              usingMovieTraveler - to check for using movie traveler
     *                                  executable
     *
     * Return: None
     */
    void buildGraph(bool usingMovieTraveler);

    /*
     * Using BFS and Dijkstra, the method searches the graph, having all nodes
     * initially having the distance of INT_MAX to account for each node already
     * touched, then finding the distance from the first actor to the second
     * actor and returning this path
     *
     * Parameters:
     *              firstActor - the string of the first actor
     *              secondActor - the string of the second actor
     *
     * Return: the path from the first actor to the second actor as a vector,
     * alternating between movie and actors to represent the path.
     */
    vector<string> shortestUnweightedPath(string firstActor,
                                          string secondActor);

    /*
     * Using BFS and Dijkstra, the method searches the graph, having all nodes
     * initially having the distance of INT_MAX and accounting for nodes that
     * have had their neighbors added to queue, the method uses the movie
     * weights to find the shortest distance.
     *
     * Parameters:
     *              firstActor - the string of the first actor
     *              secondActor - the string of the second actor
     *
     * Return: the path from the first actor to the second actor as a vector,
     * alternating between movie and actors to represent the path.
     */
    vector<string> shortestWeightedPath(string firstActor, string secondActor);

    /*
     * Using Kruskal to find all the possible weights of each edge then sorting
     * the edges from lowest weight to highest, then inserting into a graph.
     * With each insertion, the tree uses union and find operations to check for
     * cycles and path compressing the unions and finds for efficiency purposes.
     *
     * Parameters:
     *              result - a reference to all the possible efficient
     * connections of the tree
     *
     * Return: the top of the tree
     */
    void minimumSpanningKruskal(
        unordered_map<ActorNode*, vector<pair<ActorNode*, int>>>& result);
    // Maybe add some more methods here

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
    bool loadFromFile(const char* in_filename, bool use_weighted_edges);
};

#endif  // ACTORGRAPH_HPP
