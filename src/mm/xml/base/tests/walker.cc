#include <mm/xml/base/xml.hh>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct N
{
  N(std::string&& n, int d)
    : name {std::move (n)},
      depth {d}
  {}
  std::string name;
  int depth;
};

bool
operator== (N const& n1, N const& n2)
{
  return n1.name == n2.name && n1.depth == n2.depth;
}

bool
operator!= (N const& n1, N const& n2)
{
  return !(n1 == n2);
}

class TestWalker : public Mm::Xml::Base::Walker
{
public:
  std::vector<N> nodes;

private:
  virtual bool node (Mm::Xml::Base::Node& node,
                     int depth) override
  {
    nodes.emplace_back (node.name (), depth);
    return true;
  }

  virtual bool postprocess_node (Mm::Xml::Base::Node& node,
                                 int depth) override
  {
    auto pname = "-" + node.name ();
    nodes.emplace_back (std::move (pname), depth);
    return true;
  }
};

int
main ()
{
  std::vector<N> expected {{"a", 0}, {"b", 1}, {"c", 2}, {"-c", 2}, {"d", 2}, {"-d", 2}, {"-b", 1}, {"e", 1}, {"f", 2}, {"-f", 2}, {"g", 2}, {"-g", 2}, {"-e", 1}, {"-a", 0}};

  Mm::Xml::Base::Document doc;

  //    __a__
  //   |     |
  //  _b_   _e_
  // |   | |   |
  // c   d f   g
  auto a_node = doc.add_root ("a");
  a_node.set_text ("this should be ignored");
  auto b_node = a_node.add_child ("b");
  b_node.set_text ("this should be ignored");
  b_node.add_child ("c");
  b_node.add_child ("d");
  auto e_node = a_node.add_child ("e");
  e_node.set_text ("this should be ignored");
  e_node.add_child ("f");
  e_node.add_child ("g");

  TestWalker w;

  w.walk (doc);

  auto code = 0;
  if (w.nodes.size () != expected.size ())
  {
    code = 1;
    std::cerr << "got nodes size: " << w.nodes.size () << ", expected: " << expected.size () << "\n";
  }

  for (auto const&p : boost::make_iterator_range (boost::make_zip_iterator (boost::make_tuple (w.nodes.begin (), expected.begin ())),
                                                  boost::make_zip_iterator (boost::make_tuple (w.nodes.end (), expected.end ()))))
  {
    auto const& got = p.get<0> ();
    auto const& ex = p.get<1> ();

    if (got != ex)
    {
      std::cerr << "got node '" << got.name << "' (depth: " << got.depth << "), expected '" << ex.name << "' (depth: " << ex.depth << ")\n";
      code = 1;
    }
  }

  return code;
}
