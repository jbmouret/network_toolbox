// code (c) J.-B. Mouret - mouret@isir.upmc.fr
// GPL licence
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

#include "modularity.hpp"
#include "igraph.hpp"



struct Params {
  Params() :
    layered(false),
    normalize(false),
    random(false),
    simplify(false),
    directed(false){ }
  std::string input;
  bool layered;
  bool normalize;
  bool random;
  bool simplify;
  bool directed;
};

Params parse_options(int argc, char**argv) {
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "print this help message")
  ("layered,l", "Directed, layered, feed forward network (change the null model)")
  ("directed,d", "Directed network")
  ("undirected,u", "Undirected network")
  ("normalize,n", "normalize by a random network")
  ("random,r", "generate a random network")
  ("simplify,s", "simplify the network before computing modularity (remove nodes that are not connected to IO)")
  ("input,i", po::value<std::string>(), "input file (dot)");

  Params p;
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (po::error & e) {
    std::cerr << "error: " << e.what() << std::endl;
    std::cerr << desc << std::endl;
    exit(1);
  }
  if (!vm.count("input")) {
    std::cerr << "please specify an input file [-i]" << std::endl;
    exit(1);
  }
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  if (vm.count("layered")) {
    p.layered = true;
    p.simplify = true;
  }
  if (vm.count("random"))
    p.random = true;
  if (vm.count("normalize"))
    p.normalize = true;
  if (vm.count("simplify"))
    p.simplify = true;
  if (vm.count("directed"))
    p.directed = true;
  if (vm.count("undirected"))
    p.directed = false;

  p.input = vm["input"].as<std::string>();
  return p;
}


int main(int argc, char **argv) {
  Params p = parse_options(argc, argv);
  igraph::graph_t g = igraph::load(p.input);
  if (p.simplify)
    igraph::simplify(g);
  std::vector<std::vector<igraph::vertex_desc_t> > mods;
  double mod1 = -1;
  if (p.layered) {
    std::map<igraph::vertex_desc_t, int> layers = igraph::compute_layers(g);
    mod1 = mod::modules(g, mods, mod::null_model::Layered<igraph::graph_t>(g, layers));
  } else if (p.directed)
    mod1 = mod::modules(g, mods, mod::null_model::Directed());
  else
    mod1 = mod::modules(g, mods, mod::null_model::Undirected());

  std::cout << "modularity: " << mod1 << std::endl;
  std::cout << "number of modules:"  << mods.size() << std::endl;
  std::cout << "modules:" << std::endl;
  for (size_t i = 0; i < mods.size(); ++i) {
    for (size_t j = 0; j < mods[i].size(); ++j)
      std::cout << g[mods[i][j]].get_id() << " ";
    std::cout << std::endl;
  }
  return 0;
}
