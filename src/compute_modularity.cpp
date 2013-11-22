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


std::string parse_options(int argc, char**argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "print this help message")
    ("input,i", po::value<std::string>(), "input file (dot)");

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch(po::error & e)
  {
    std::cerr << "error: " << e.what() << std::endl;
    std::cerr << desc << std::endl;
    exit(1);
  }
  if (!vm.count("input"))
  {
    std::cerr << "please specify an input file [-i]" << std::endl;
    exit(1);
  }
  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    exit(0);
  }
  return vm["input"].as<std::string>();
}


int main(int argc, char **argv)
{
  std::string f = parse_options(argc, argv);
  igraph::graph_t g = igraph::load(f);
  std::vector<std::vector<igraph::vertex_desc_t> > mods;
  float mod1 = mod::modules(g, mods);
  std::cout << "modularity (directed): " << mod1 << std::endl;
  std::cout << "number of modules:" << mods.size() << std::endl;
  std::cout << "modules:" << std::endl;
  for (size_t i = 0; i < mods.size(); ++i)
  {
    for (size_t j = 0; j < mods[i].size(); ++j)
      std::cout << g[mods[i][j]].get_id() << " ";
    std::cout << std::endl;
  }
  return 0;
}
