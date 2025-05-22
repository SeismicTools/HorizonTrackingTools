#pragma once 

#include <cstdint>
#include <utility>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>

typedef std::pair<int32_t, int32_t> iPair;

// To represent Disjoint Sets
class DisjointSets
{
private:
    int32_t n;
    std::vector<int32_t> parent, rnk;
public:
    // Constructor. 
    DisjointSets(int32_t _n) :n(_n), parent(n + 1), rnk(n + 1)
    {
        // Initially, all vertices are in 
        // different sets and have rank 0. 
        for (int32_t i = 0; i <= n; i++)
        {
            rnk[i] = 0;
            //every element is parent of itself 
            parent[i] = i;
        }
    }

    // Find the parent of a node 'u' 
    // Path Compression 
    int32_t find(int32_t u)
    {
        /* Make the parent of the nodes in the path
        from u--> parent[u] point to parent[u] */
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }

    // Union by rank 
    void merge(int32_t x, int32_t y)
    {
        x = find(x), y = find(y);

        /* Make tree with smaller height
        a subtree of the other tree */
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else // If rnk[x] <= rnk[y] 
            parent[x] = y;

        if (rnk[x] == rnk[y])
            rnk[y]++;
    }
};

template<typename TCost> class EdgeCost;

class Edge {
public:
    int32_t idx_start, idx_end;
public:
    inline Edge():idx_start(0),idx_end(0){}
    inline Edge(const int32_t _idx_start, const int32_t _idx_end)
        :idx_start(_idx_start), idx_end(_idx_end){
    }
    inline int32_t idxStart()const {
        return idx_start;
    }
    inline int32_t idxEnd()const {
        return idx_end;
    }
    template<typename TCost> void operator=(const EdgeCost<TCost>& edge) {
        idx_start = edge.idxStart();
        idx_end = edge.idxEnd();
    }
};

template<typename TCost> class EdgeCost: public Edge{
public:
    TCost _cost;
public:
    inline EdgeCost(const int32_t _idx_start, const int32_t _idx_finish, const TCost& __cost)
        :Edge(_idx_start, _idx_finish),_cost(__cost){
    }

    inline bool operator >(const EdgeCost& ec)const {
        return (double)(*this) >(double) ec;
    }

    inline operator double() {
        return _cost;
    }

    TCost& cost() {
        return _cost;
    }
};

template<typename TEdge> class Graph
{
private:
    std::vector<TEdge> edges;
    std::vector<std::list<std::tuple<int32_t, int32_t>>> link;
private:
    void initLinks() {
        link = std::vector<std::list<std::tuple<int32_t, int32_t>>>(countVertext());
        for (int32_t i = 0; i < edges.size(); i++) {
            link[edges[i].idxStart()].push_back(std::make_tuple(edges[i].idxEnd(),i));
            link[edges[i].idxEnd()].push_back(std::make_tuple(edges[i].idxStart(),i));
        }
        /*for (auto it = edges.cbegin(); it != edges.cend(); it++) {
            link[it->idxStart()].push_back(it->idxEnd());
            link[it->idxEnd()].push_back(it->idxStart());
        }*/
    }
public:
    Graph():edges(0),link(0){
    }
    // Constructor 
    Graph(const int32_t _V,
        const std::vector<TEdge>& _edges = std::vector<TEdge>())
        :edges(_edges), link(_V)
    {
        initLinks();
    }

    template<typename TEdgeOther> Graph(const Graph<TEdgeOther>& graph)
        :link(graph.countVertext()),edges(graph.edgeSize()){
        for (int32_t i = 0; i < link.size(); i++) {
            link[i] = graph.near(i);
        }
        for (int32_t i = 0; i < edges.size();i++) {
            edges[i] = graph.getEdge(i);
        }
    }

    //Соседние вершины к заданной
    std::list< std::tuple<int32_t, int32_t>> near(const int32_t u)const {
        return link[u];
    }
    //Количесво вершин
    int32_t countVertext()const {
        return link.size();
    }

    TEdge& getEdge(const int32_t i){
        return edges[i];
    }

    TEdge getEdge(const int32_t i)const {
        return edges[i];
    }

    int32_t edgeSize()const {
        return edges.size();
    }

    // Utility function to add an edge 
    void addEdge(const TEdge& edge)
    {
        edges.push_back(edge);
        initLinks();
    }

    // Function to find MST using Kruskal's 
  // MST algorithm 
    static  void kruskalMST(Graph<TEdge>& g) {
        //std::shared_ptr<Foo> sptr = std::make_shared<Foo>(100);

        // Sort edges in increasing order on basis of cost 
        std::sort(g.edges.begin(), g.edges.end());
        // Create disjoint sets 
        DisjointSets ds(g.countVertext());
        // Iterate through all sorted edges 
        //std::vector<std::pair<TCost, iPair>>::const_iterator it;
        std::list<TEdge> min_edges_list;
        for (auto it = g.edges.cbegin(); it != g.edges.cend(); it++)
        {
            int32_t u = it->idxStart();
            int32_t v = it->idxEnd();

            int32_t set_u = ds.find(u);
            int32_t set_v = ds.find(v);

            // Check if the selected edge is creating 
            // a cycle or not (Cycle is created if u 
            // and v belong to same set) 
            if (set_u != set_v)
            {
                // Update MST weight 
                // Merge two sets 
                ds.merge(set_u, set_v);

                min_edges_list.push_back(*it);

            }
        }
        std::vector<TEdge> min_edges(min_edges_list.cbegin(), min_edges_list.cend());
        g.edges = min_edges;
        g.initLinks();
    }
};

