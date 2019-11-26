/**
 * Author: Michael Askndafi
 * Email: maskndaf@ucsd.edu
 * Description: Tests every method in HCTree, tests if the tree has or hasn't
 * been built yet. Tests both ascii and bit method operations all by using
 * google framework.
 * Sources:Piazza
 */

#include <iostream>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include "ActorGraph.hpp"
#include "ActorNode.hpp"

using namespace std;
using namespace testing;

TEST(SimpleTest, TEST_EMPTY_GRAPH) {
    ActorGraph* graph = new ActorGraph();
    ASSERT_FALSE(graph->checkIfActorExists("Kevin Bacon"));
    delete graph;
}
