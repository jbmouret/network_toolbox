// code (c) J.-B. Mouret - mouret@isir.upmc.fr
// GPL licence
#include <fstream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>

#include "modularity.hpp"
#include "igraph.hpp"



struct Params {
  std::string input;
  bool layered = false;
  bool normalize = false;
  bool random = false;
  bool simplify = false;
};

Params parse_options(int argc, char**argv) {
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "print this help message")
  ("layered,l", "Layered, feed forward network (change the null model)")
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
  if (vm.count("layered"))
    p.layered = true;
  if (vm.count("random"))
    p.random = true;
  if (vm.count("normalize"))
    p.normalize = true;
  if (vm.count("simplify"))
    p.simplify = true;
  p.input = vm["input"].as<std::string>();
  return p;
}


int main(int argc, char **argv) {
  Params p = parse_options(argc, argv);
  igraph::graph_t g = igraph::load(p.input);
  if (p.simplify)
    igraph::simplify(g);
  std::vector<std::vector<igraph::vertex_desc_t> > mods;
  float mod1 = mod::modules(g, mods);
  std::cout << "modularity (directed): " << mod1 << std::endl;
  std::cout << "number of modules:" << mods.size() << std::endl;
  std::cout << "modules:" << std::endl;
  for (size_t i = 0; i < mods.size(); ++i) {
    for (size_t j = 0; j < mods[i].size(); ++j)
      std::cout << g[mods[i][j]].get_id() << " ";
    std::cout << std::endl;
  }
  return 0;
}
