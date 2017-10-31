#include <vector>

// ====================================================================================
// A Point is just a 2D coordinate.

class Point {
public:
  // constructor
  Point(int x_, int y_);
  // accessors
  int get_x() const { return x; }
  int get_y() const { return y; }
private:
  // representation
  int x;
  int y;
};

// helper function for printing Points
std::ostream& operator<<(std::ostream &ostr, const Point &p);

// ====================================================================================
// An Edge is a vertical or horizontal line segment connecting 2 points.

class Edge {
public:
  // constructor
  Edge(const Point &a_, const Point &b_);
  // accessors
  const Point& get_a() const { return a; }
  const Point& get_b() const { return b; }
  bool operator==(const Edge & right);
private:
  // representation
  Point a;
  Point b;
};

// helper function for printing Edges
std::ostream& operator<<(std::ostream &ostr, const Edge &e);

// ====================================================================================
// A Node is a Point with a count of the number of Edges that should touch it.

class Node {
public:
  Node(const Point &p, int n);
  // accessors
  const Point& get_pt() const { return pt; }
  int get_num_paths() const { return num_paths; }
  int get_paths() const { return paths; }

  void de_path() { num_paths --; }
  void in_path() { num_paths ++; }

  bool isDeleted() { return deleted; }
  void deleteNode() { deleted = true; }
  void revert() { deleted = false; }
  void visit() { visited = true; }
  bool isVisited() { return visited; }

  bool operator==(const Node& right);
private:
  Point pt;
  int num_paths;
  int paths;
  bool deleted;
  bool visited;
};

// ====================================================================================
// A Graph is a collection of Nodes and Edges that can be printed.

class Graph {
public:
  // constructor
  Graph();
  Graph(const Graph& g);
  // modifiers
  void addNode(const Point &p, int n);
  void addEdge(const Node &a, const Node &b);
  void occupy(const Node &a, const Node &b);
  // print helper functions
  void printSolution() const;
  void printGraph() const;

  std::vector<Node> nodes;
  std::vector<Edge> edges;
  std::vector<Edge> emptyEdges;

  bool isNeighbour(const Node & left, const Node & right);
  int size() { return size_; }
  void shrink() { size_ --; }
  void initialize();
  void update();

  bool operator==(const Graph & right);
  bool valid;

  bool isConnected();
  void visit();
private:
  // representation
  int max_x;
  int max_y;
  int size_;
  int oldsize_;

  std::vector<std::vector<bool> > occupied;
};

void visitNode(Graph* g, Node& n);

// ====================================================================================
