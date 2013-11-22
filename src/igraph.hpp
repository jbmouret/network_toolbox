#ifndef IGRAPH_HPP_
#define IGRAPH_HPP_


#include <boost/program_options.hpp>
#include <boost/graph/reverse_graph.hpp>


namespace igraph
{

  struct Node
  {
    Node() {
    }
    const std::string& get_id() const { return _id; }
    int get_in() const
    {
      if (_id[0] != 'i')
        return -1;
      return get_io(_id);
    }
    int get_out() const
    {
      if (_label[0] != 'o')
        return -1;
      return get_io(_id);
    }
    const std::string& get_type() const { return _id; }
    const std::string& get_label() const { return _label; }

    std::string _id;
    std::string _label;
    std::string _type;

    int _color; //for algos
    protected:
      int get_io(const std::string& s) const
      {
        std::string k = _label.substr(1, _label.size() - 1);
        return boost::lexical_cast<int>(k);
      }
  };

  struct Connection {
    float _weight;
    float get_weight() const { return _weight; }
  };

  typedef std::vector< std::pair<int, int> > gen_t;
  typedef boost::adjacency_list<boost::listS, boost::listS,
                                boost::bidirectionalS,
                                Node, Connection> graph_t;
  typedef boost::graph_traits<graph_t>::vertex_iterator vertex_it_t;
  typedef boost::graph_traits<graph_t>::edge_iterator edge_it_t;
  typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_it_t;
  typedef boost::graph_traits<graph_t>::in_edge_iterator in_edge_it_t;
  typedef boost::graph_traits<graph_t>::edge_descriptor edge_desc_t;
  typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_desc_t;
  typedef boost::graph_traits<graph_t>::adjacency_iterator adj_it_t;
  typedef std::vector<vertex_desc_t> vertex_list_t;
  typedef unsigned char byte_t;

  static graph_t load(const std::string& fname)
  {
    std::ifstream ifs(fname.c_str());
    assert(ifs.good());

    graph_t g;
    boost::dynamic_properties dp;
    dp.property("node_id", get(&Node::_id, g));
    dp.property("label", get(&Node::_label, g));
    dp.property("type", get(&Node::_type, g));
    dp.property("label", get(&Connection::_weight, g));

    boost::read_graphviz(ifs, g, dp, "node_id");
    return g;
  }

// a useful boost functor
  template<typename V>
  class bfs_pred_visitor : public boost::default_bfs_visitor
  {
    public:
      bfs_pred_visitor(std::set<V>&pred) : _pred(pred) {
      }
      template<typename Vertex, typename Graph >
      void discover_vertex(Vertex u, const Graph& g)
      {
        _pred.insert(u);
      }
    protected:
      std::set<V>& _pred;
  };

// remove neurons that are not connected to both one input an
// one output (this is NOT callled automatically in NN
//
// WARNING: if simplify_in is true, this can change the behavior
// of neurons since neurons not connected to inputs but connected
// to outputs can output a constant value
//
// principle : keep the neurons that are successors of inputs
// and predecessors of outputs
  static void simplify(graph_t& g, bool simplify_in = false)
  {
    using namespace boost;
     // we need sets and not lists withouh io
    std::set<vertex_desc_t> all_neurons;
    std::vector<vertex_desc_t> inputs, outputs;
    BGL_FORALL_VERTICES(v, g, graph_t)
    if (g[v].get_out() != -1)
      outputs.push_back(v);
    else
    if (g[v].get_in() != -1)
      inputs.push_back(v);
    else
      all_neurons.insert(v);
    std::set<vertex_desc_t> out_preds, in_succs;

     // out
    BOOST_FOREACH(vertex_desc_t v, outputs)
    {
      std::set<vertex_desc_t> preds;
      bfs_pred_visitor<vertex_desc_t> vis(preds);
      breadth_first_search(boost::make_reverse_graph(g), v,
                           color_map(get(&Node::_color, g)).visitor(vis));
      out_preds.insert(preds.begin(), preds.end());
    }
     // in
    if (simplify_in)
      BOOST_FOREACH(vertex_desc_t v, inputs)
      {
        std::set<vertex_desc_t> succs;
        bfs_pred_visitor<vertex_desc_t> vis(succs);
        breadth_first_search(g, v, color_map(get(&Node::_color, g)).visitor(vis));
        in_succs.insert(succs.begin(), succs.end());
      }
      else
        in_succs = all_neurons;
     // make the intersection of in_succ and out_preds
    std::set<vertex_desc_t> valid_neurons;
    std::set_intersection(in_succs.begin(), in_succs.end(),
                          out_preds.begin(), out_preds.end(),
                          std::insert_iterator<std::set<vertex_desc_t> >(valid_neurons,
                                                                         valid_neurons.begin()));
     // get the list of neurons that are NOT in valid_neurons
    std::set<vertex_desc_t> to_remove;
    std::set_difference(all_neurons.begin(), all_neurons.end(),
                        valid_neurons.begin(), valid_neurons.end(),
                        std::insert_iterator<std::set<vertex_desc_t> >(to_remove,
                                                                       to_remove.begin()));

     // remove these neurons
    BOOST_FOREACH(vertex_desc_t v, to_remove)
    {
      clear_vertex(v, g);
      remove_vertex(v, g);
    }
    to_remove.clear();

     // also remove disconnected i/o
    BGL_FORALL_VERTICES(v, g, graph_t)
    if (in_degree(v, g) == 0 && out_degree(v, g) == 0)
      to_remove.insert(v);
    BOOST_FOREACH(vertex_desc_t v, to_remove)
    {
      clear_vertex(v, g);
      remove_vertex(v, g);
    }
  }
}


#endif
