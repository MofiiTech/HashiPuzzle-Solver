#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "graph.h"


// ====================================================================================

Point::Point(int x_, int y_) : x(x_),y(y_) {
  // Points must have positive coordinates
  assert (x_ >= 0);
  assert (y_ >= 0);
}

// helper function for printing Points
std::ostream& operator<<(std::ostream &ostr, const Point &p) {
  ostr << "(" << p.get_x() << "," << p.get_y() << ")";
  return ostr;
}

// ====================================================================================

Edge::Edge(const Point &a_, const Point &b_) : a(a_),b(b_) {
  // Edges must be horizontal or vertical
  //assert ((a_.get_x() == b_.get_x() && a_.get_y() != b_.get_y()) ||
  //         (a_.get_x() != b_.get_x() && a_.get_y() == b_.get_y()));
}

bool Edge::operator==(const Edge & right) {
  if (a.get_x() == right.a.get_x()) {
    if (a.get_y() == right.a.get_y()) {
      if (b.get_x() == right.b.get_x()) {
        if (b.get_y() == right.b.get_y()) {
          return true;
        }
      }
    }
  }
  if (a.get_x() == right.b.get_x()) {
    if (a.get_y() == right.b.get_y()) {
      if (b.get_x() == right.a.get_x()) {
        if (b.get_y() == right.a.get_y()) {
          return true;
        }
      }
    }
  }
  return false;
}

// ====================================================================================

Node::Node(const Point &p, int n) : pt(p),num_paths(n) {
  // Nodes/islands must have at least one Edge/bridge.  At most 2
  // Edges/bridges can connect a pair of Nodes/islands.  Edges/bridges
  // may only run horizontal or vertical and cannot cross other
  // Edges/bridges.  This a Node/island can support at most 8 bridges.
  assert (n >= 1 && n <= 8);
  deleted = false;
  visited = false;
  paths = n;
}

// ====================================================================================

// helper function for printing Edges
std::ostream& operator<<(std::ostream &ostr, const Edge &e) {
  ostr << e.get_a() << "," << e.get_b();
  return ostr;
}

bool Node::operator==(const Node& right) {
  if (pt.get_x() == right.get_pt().get_x()) {
    if (pt.get_y() == right.get_pt().get_y()) {
      return true;
    }
  }
  return false;
}

// ====================================================================================

Graph::Graph() {
  // We track the maximum dimensions of the graph.
  max_x = 0;
  max_y = 0;
  size_ = 0;
  oldsize_ = 0;
  valid = true;
}


void Graph::addNode(const Point &p, int n) {
  // Incrementally track the maximum dimensions of the graph.
  max_x = std::max(p.get_x(),max_x);
  max_y = std::max(p.get_y(),max_y);
  nodes.push_back(Node(p,n));
  size_++;
  oldsize_++;
}


void Graph::addEdge(const Node &a, const Node &b) {
  assert (a.get_pt().get_x() <= max_x);
  assert (a.get_pt().get_y() <= max_y);
  assert (b.get_pt().get_x() <= max_x);
  assert (b.get_pt().get_y() <= max_y);
  edges.push_back(Edge(a.get_pt(),b.get_pt()));
  int x1 = a.get_pt().get_x();
  int x2 = b.get_pt().get_x();
  int y1 = a.get_pt().get_y();
  int y2 = b.get_pt().get_y();
  if (x1 == x2) {
    if (y1 > y2) {
      for (int i = y2 + 1; i < y1; i ++) {
        occupied[i][x1] = true;
      }
    } else {
      for (int i = y1 + 1; i < y2; i ++) {
        occupied[i][x1] = true;
      }
    }
  } else {
    if (x1 > x2) {
      for (int i = x2 + 1; i < x1; i ++) {
        occupied[y1][i] = true;
      }
    } else {
      for (int i = x1 + 1; i < x2; i ++) {
        occupied[y1][i] = true;
      }
    }
  }
}

void Graph::occupy(const Node &a, const Node &b) {
  emptyEdges.push_back(Edge(a.get_pt(),b.get_pt()));
  emptyEdges.push_back(Edge(b.get_pt(),a.get_pt()));
}

void Graph::printSolution() const {
  std::cout << "Solution:" << std::endl;
  for (int i = 0; i < edges.size(); i++) {
    std::cout << edges[i] << std::endl;
  }
}


void Graph::printGraph() const {
  // a graph should have at least one node/island
  assert (nodes.size() > 1);
  // the dimensions of the board should be positive
  assert (max_x >= 0);
  assert (max_y >= 0);

  // allocate an empty/blank board, twice as big in each dimension so
  // we can draw lines between the nodes/islands.
  std::vector<std::string> board(2*max_y+1,std::string(2*max_x+1,' '));
  // mark all of the islands with their bridge count
  for (int i = 0; i < nodes.size(); i++) {
    const Point &pt = nodes[i].get_pt(); 
    std::string value = std::to_string(nodes[i].get_paths());
    board[2*pt.get_y()][2*pt.get_x()] = value[0];
  }

  // loop over all of the edges/bridges
  for (int i = 0; i < edges.size(); i++) {
    Point a = edges[i].get_a();
    Point b = edges[i].get_b();
    // determine the edge/edge direction
    int diff_x = b.get_x()-a.get_x();
    int diff_y = b.get_y()-a.get_y();
    if (diff_x > 0) diff_x = 1;
    if (diff_x < 0) diff_x = -1;
    if (diff_y > 0) diff_y = 1;
    if (diff_y < 0) diff_y = -1;

    // walk between the islands to draw the bridge
    int x = a.get_x()*2+diff_x;
    int y = a.get_y()*2+diff_y;
    while (x != b.get_x()*2 || y != b.get_y()*2) {
      if (diff_x == 0) {
        // vertical bridge
        assert (diff_y != 0);
        if (board[y][x] == '|') {
          // if we already have a bridge, draw a double vertical bridge
          board[y][x] = '#';
        } else {
          assert (board[y][x] == ' ');
          // otherwise draw a single vertical bridge
          board[y][x] = '|';        
        }
      } else {
        assert (diff_y == 0);
        // horizontal bridge
        if (board[y][x] == '-') {
          // if we already have a brige, draw a double horizontal bridge
          board[y][x] = '=';        
        } else {
          assert (board[y][x] == ' ');
          // otherwise draw a single horizontal bridge
          board[y][x] = '-';        
        }
      }
      x+=diff_x;
      y+=diff_y;
    }
  }

  // surround the board with a border of spaces
  char border = ' ';
  std::cout << std::string(board[0].size()+2,border) << std::endl;
  for (int i = board.size()-1; i >= 0; i--) {
    std::cout << border << board[i] << border << std::endl;
  }
  std::cout << std::string(board[0].size()+2,border) << std::endl;
}

// ====================================================================================

void Graph::initialize() {
  size_ = oldsize_;
  for (int i = 0; i < nodes.size(); i ++) {
    nodes[i].revert();
  }
  edges.clear();
}

Graph::Graph(const Graph& g) {
  max_x = g.max_x;
  max_y = g.max_y;
  size_ = g.size_;
  oldsize_ = g.oldsize_;
  nodes = std::vector<Node>(g.nodes);
  edges = std::vector<Edge>(g.edges);
  occupied = std::vector<std::vector<bool> >(g.occupied);
  valid = g.valid;
  emptyEdges = g.emptyEdges;
}

void Graph::update() {
  for (int i = 0; i <= max_y; i ++) {
    std::vector<bool> temp;
    for (int j = 0; j <= max_x; j ++) {
      temp.push_back(false);
    }
    occupied.push_back(temp);
  }
  for (int i = 0; i < nodes.size(); i ++) {
    occupied[nodes[i].get_pt().get_y()][nodes[i].get_pt().get_x()] = true;
  }
}

bool Graph::isNeighbour(const Node & left, const Node & right) {
  if (left.get_pt().get_x() != right.get_pt().get_x()) {
    if (left.get_pt().get_y() != right.get_pt().get_y()) {
      return false;
    } else {
      int x1 = left.get_pt().get_x();
      int x2 = right.get_pt().get_x();
      int y1 = left.get_pt().get_y();
      // int y2 = right.get_pt().get_x();
      for (int i = std::min(x1, x2) + 1; i < std::max(x1, x2); i ++) {
        if (occupied[y1][i]){
          // std::cout << "OCCUPIED******\n";
          return false;
        }
      }
      if (std::find(emptyEdges.begin(), emptyEdges.end(), Edge(left.get_pt(), right.get_pt())) != emptyEdges.end()){
        // std::cout << left.get_pt() << right.get_pt() << "empy Edge.\n";
        return false;
      }
      return true;
    }
  } else {
    int x1 = left.get_pt().get_x();
    // int x2 = right.get_pt().get_x();
    int y1 = left.get_pt().get_y();
    int y2 = right.get_pt().get_y();
    for (int i = std::min(y1, y2)+1; i < std::max(y1, y2); i ++) {
      if (occupied[i][x1]){
        // std::cout << "OCCUPIED******\n";
        return false;
      }
    }
    if (std::find(emptyEdges.begin(), emptyEdges.end(), Edge(left.get_pt(), right.get_pt())) != emptyEdges.end()){
      // std::cout << left.get_pt() << right.get_pt() << "empy Edge.\n";
      return false;
    }
    return true;
  }
}

bool Graph::isConnected() {
  this->visit();
  for (int i = 0; i < nodes.size(); i ++) {
    if (!nodes[i].isVisited()){
      // std::cout << "not visited\n";
      return false;
    }
  }
  return true;
}

void Graph::visit() {
  visitNode(this, nodes[0]);
}

void visitNode(Graph* g, Node& n) {
  for (int i = 0; i < g->nodes.size(); i ++) {
    if (n.get_pt().get_x() == g->nodes[i].get_pt().get_x() && n.get_pt().get_y() == g->nodes[i].get_pt().get_y())
      continue;
    if (std::find(g->edges.begin(), g->edges.end(), Edge(n.get_pt(), g->nodes[i].get_pt())) != g->edges.end()) {
      if (!g->nodes[i].isVisited()) {
        g->nodes[i].visit();
        visitNode(g, g->nodes[i]);
      }
    }
  }
}

/*
bool Graph::operator==(const Graph & right) {
  if (size_ != right.size_)
    return false;
  if (edges.size_ != )



  bool result = true;
  result = result && (size_ == right.size_);
  result = result && (edges == right.edges);
  return result;
}
 */



















