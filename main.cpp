#include <fstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "graph.h"
/*
This is not the recursion part of this program, but with this function, we can spend much less
time to find the solutions.
Bascially, this function look for end points of the graph and update the edges.
 */
void checkEndPoints(std::vector<Graph>& solutions, Graph& g) {
  // std::cout << g.nodes.size() << std::endl;
  bool updated = false;

  // update the graph until no more updates can be made
  do {
    updated = false;
    for (int i = 0; i < g.nodes.size(); i ++) {
      if (g.nodes[i].isDeleted())
        continue;
      int points = 0;
      // find all the neighbours
      std::vector<int> nexts;
      for (int j = 0; j < g.nodes.size(); j ++) {
        if (g.nodes[j].isDeleted()) {
          // std::cout << g.nodes[j].get_pt() << "is deleted\n";
          continue;
        }
        if (i == j){
          // std::cout << "same point\n";
          continue;
        }
        if (!g.isNeighbour(g.nodes[i], g.nodes[j])) {
          // std::cout << g.nodes[i].get_pt() << g.nodes[j].get_pt() << "not neighbour\n";
          continue;
        }
        nexts.push_back(j);
        points += std::min(g.nodes[j].get_num_paths(), 2);
      }
      // std::cout << g.nodes[i].get_pt() << "nexts: " << nexts.size() << " points: " << points << std::endl;

      // if the node has only 1 neighbour
      if (nexts.size() == 1) {
        if (points < g.nodes[i].get_num_paths()) {
          g.valid = false;
          return;
        } else {
          int path = g.nodes[i].get_num_paths();
          for (int t = 0; t < path; t ++) {
            g.addEdge(g.nodes[i], g.nodes[nexts[0]]);
            // std::cout << g.nodes[nexts[0]].get_num_paths() << " ";
            g.nodes[nexts[0]].de_path();
            // std::cout << g.nodes[nexts[0]].get_num_paths() << std::endl;
          }
          if (g.nodes[nexts[0]].get_num_paths() == 0){
            g.nodes[nexts[0]].deleteNode();
            // std::cout << "delete " << g.nodes[nexts[0]].get_pt();
            g.shrink();
          }
          g.nodes[i].deleteNode();
          // std::cout << "delete " << g.nodes[i].get_pt();
          g.shrink();
          updated = true;
        }
        // if the node has more than 1 neighbour
      } else {
        if (points > g.nodes[i].get_num_paths()) {
          continue;
        } else if (points < g.nodes[i].get_num_paths()) {
          g.valid = false;
          return;
        } else {
          for (int j = 0; j < nexts.size(); j ++) {
            int path = g.nodes[nexts[j]].get_num_paths();
            for (int k = 0; k < std::min(path, 2); k ++) {
              g.addEdge(g.nodes[i], g.nodes[nexts[j]]);
              // std::cout << g.nodes[nexts[j]].get_num_paths() << " ";
              g.nodes[nexts[j]].de_path();
              // std::cout << g.nodes[nexts[j]].get_num_paths() << std::endl;
            }
            if (g.nodes[nexts[j]].get_num_paths() == 0){
              g.nodes[nexts[j]].deleteNode();
              // std::cout << "delete " << g.nodes[nexts[j]].get_pt();
              g.shrink();
            }
          }
          g.nodes[i].deleteNode();
          // std::cout << "delete " << g.nodes[i].get_pt();
          g.shrink();
          updated = true;
        }
      }
    }
  } while(updated);
}

void findSolution(std::vector<Graph>& solutions, Graph& g) {

  // std::cout << "start checking end points\n";
  // g.printGraph();
  checkEndPoints(solutions, g);
  // g.printGraph();
  // std::cout << "checking end points ended\n";
  if (!g.valid)
    return;
  if (g.size() == 0) {
    // g.initialize();
    solutions.push_back(g);
    return;
  }

  // find the node in this scenario
  int now = 0;
  while (g.nodes[now].isDeleted()) {
    now ++;
  }

  // std::cout << g.nodes[now].get_pt() << std::endl;

  // find all the neighbours of this node
  std::vector<int> neighbours;
  for (int i = 0; i < g.nodes.size(); i ++) {
    if (now != i) {
      if (!g.nodes[i].isDeleted()) {
        if (g.isNeighbour(g.nodes[now], g.nodes[i]))
          neighbours.push_back(i);
      }
    }
  }

  // check if valid
  int points = 0;
  for (int i = 0; i < neighbours.size(); i ++) {
    points += std::min(g.nodes[neighbours[i]].get_num_paths(), 2);
  }
  if (points < g.nodes[now].get_num_paths()) {
    g.valid = false;
    return;
  }

  // std::cout << g.nodes[neighbours[0]].get_pt() << std::endl;

  // recursion part
  int path = g.nodes[neighbours[0]].get_num_paths();
  for (int i = 0; i <= std::min(path, 2); i ++) {
    Graph newg = g;
    if (i == 0) {
      newg.occupy(newg.nodes[now], newg.nodes[neighbours[0]]);
    } else {
      for (int j = 0; j < i; j ++) {
        newg.addEdge(newg.nodes[now], newg.nodes[neighbours[0]]);
        newg.nodes[neighbours[0]].de_path();
        newg.nodes[now].de_path();
      }
      if (newg.nodes[neighbours[0]].get_num_paths() == 0) {
        newg.nodes[neighbours[0]].deleteNode();
        newg.shrink();
      }
      if (newg.nodes[now].get_num_paths() == 0) {
        newg.nodes[now].deleteNode();
        newg.shrink();
      }
    }
    // Graph* temp = &newg;
    // solutions.push_back(newg);
    findSolution(solutions, newg);
  }
  g.valid = false;
  
  /*
  int rest = g.nodes[now].get_num_paths();
  for (int i = 0; i < neighbours.size(); i ++) {
    for (int j = 0; j <= std::min(rest, 2); j ++) {
      for (int k = 0; k < j; k ++) {
        g.addEdge(g.nodes[now], g.nodes[neighbours[i]]);
        g.nodes[neighbours[i]].de_path();
        rest--;
      }
    }
  }
   */

}

int main(int argc, char* argv[] ) {

  
  // Parse the arguments
  if (argc < 2) {
    std::cout << "ERROR!  Must specify input file" << std::endl;
    exit(1);
  }
  std::ifstream istr(argv[1]);
  if (!istr.good()) {
    std::cout << "ERROR!  Could not open input file '" << argv[1] << "'" << std::endl;
    exit(1);
  }
  bool find_all_solutions = false;
  bool connected = false;
  for (int i = 2; i < argc; i++) {
    if (argv[i] == std::string("--find_all_solutions")) {
      find_all_solutions = true;
    } else if (argv[i] == std::string("--connected")) {
      connected = true;
    } else {
      std::cout << "ERROR!  Unknown argument '" << argv[i] << "'" << std::endl;
      exit(1);
    }
  }


  // Create an empty graph object
  Graph g;
  // Read in the puzzle from the input file
  int x,y,n;
  std::vector<Point> pts;
  while (istr >> x >> y >> n) {
    pts.push_back(Point(x,y));
    g.addNode(Point(x,y),n);
  }
  g.update();

  g.printGraph();
  // std::cout << g.size() << std::endl;
  std::vector<Graph> solutions;
  // Graph* temp = &g;
  // solutions.push_back(g);
  // std::cout << "solutions" << solutions.size() << std::endl;
  findSolution(solutions, g);
  if (solutions.size() == 0){
    std::cout << "no solution" << std::endl;
    exit(0);
  }

  if (find_all_solutions == true) {
    if (connected == true) {
      for (int i = 0; i < solutions.size(); i ++) {
        if (solutions[i].isConnected()) {
          solutions[i].printSolution();
          solutions[i].printGraph();
        }
      }
    } else {
      std::cout << "Found " << solutions.size() << " solution(s)\n";
      for (int i = 0; i < solutions.size(); i ++) {
        solutions[i].printSolution();
        solutions[i].printGraph();
      }
    }
  } else {
    if (connected == true) {
      for (int i = 0; i < solutions.size(); i ++) {
        if (solutions[i].isConnected()) {
          solutions[i].printSolution();
          solutions[i].printGraph();
          exit(0);
        }
      }
      std::cout << "No solutions";
    } else {
      solutions[0].printSolution();
      solutions[0].printGraph();
    }
  }

  /*
  for (int i = 0; i < solutions.size(); i ++) {
    if (solutions[i].valid) {
      solutions[i].printSolution();
      solutions[i].printGraph();
    }
  }
   */
  
  /*
  // The statements below hardcode the solution to puzzle1.txt.
  // You should remove this code and replace it with a general solver.
  if (argv[1] == std::string("puzzle1.txt")) {
    g.addEdge(g.nodes[0],g.nodes[1]);
    g.addEdge(g.nodes[0],g.nodes[1]);
    g.addEdge(g.nodes[1],g.nodes[2]);
    g.addEdge(g.nodes[1],g.nodes[2]);
    g.addEdge(g.nodes[2],g.nodes[3]);

    // print the edges (any order is ok)
    g.printSolution();
  }
   */

  // print a visualization of the (solved or unsolved) puzzle
  // g.printGraph();

  /*
  for (int i = 0; i < g.nodes.size(); i ++) {
    std::cout << g.nodes[i].get_pt() << std::endl;
  }
  std::cout << g.nodes[2].get_pt() << " " << g.nodes[0].get_pt() << ": " << g.isNeighbour(g.nodes[2], g.nodes[0]) << std::endl;
  std::cout << g.nodes[2].get_pt() << " " << g.nodes[0].get_pt() << ": " << g.isNeighbour(g.nodes[0], g.nodes[2]) << std::endl;
  */

}























