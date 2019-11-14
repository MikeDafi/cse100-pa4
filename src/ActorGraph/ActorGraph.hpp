/*
 * ActorGraph.hpp
 * Author: <YOUR NAME HERE>
 * Date:   <DATE HERE>
 *
 * This file is meant to exist as a container for starter code that you can use to read the input file format
 * defined imdb_2019.tsv. Feel free to modify any/all aspects as you wish.
 */

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>

// Maybe include some data structures here

using namespace std;

/**
 * TODO: add class header
 */
class ActorGraph {
  protected:
    // Maybe add class data structure(s) here

  public:
    /**
     * Constuctor of the Actor graph
     */
    ActorGraph(void);

    // Maybe add some more methods here
  
    /** You can modify this method definition as you wish
     *
     * Load the graph from a tab-delimited file of actor->movie relationships.
     *
     * in_filename - input filename
     * use_weighted_edges - if true, compute edge weights as 1 + (2019 - movie_year), 
     *                      otherwise all edge weights will be 1
     *
     * return true if file was loaded sucessfully, false otherwise
     */
    bool loadFromFile(const char* in_filename, bool use_weighted_edges);
    
};

#endif  // ACTORGRAPH_HPP
