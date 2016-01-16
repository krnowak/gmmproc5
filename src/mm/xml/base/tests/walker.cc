#include <mm/xml/base/xml.hh>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

class TestWalker : public Mm::Xml::Base::Walker
{
public:
  std::vector<std::string> visited_nodes;

private:
  virtual bool node (Mm::Xml::Base::Node& node, int) override
  {
    visited_nodes.push_back (node.name ());
    return true;
  }

  virtual bool postprocess_node (Mm::Xml::Base::Node& node, int) override
  {
    auto pname = "-" + node.name ();
    visited_nodes.push_back (std::move (pname));
    return true;
  }
};

int
main ()
{
  std::vector<std::string> expected {"a", "b", "c", "-c", "d", "-d", "-b", "e", "f", "-f", "g", "-g", "-e", "-a"};

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
  if (w.visited_nodes.size () != expected.size ())
  {
    code = 1;
    std::cerr << "got nodes size: " << w.visited_nodes.size () << ", expected: " << expected.size () << "\n";
  }

  for (auto const&p : boost::make_iterator_range (boost::make_zip_iterator (boost::make_tuple (w.visited_nodes.begin (), expected.begin ())),
                                                  boost::make_zip_iterator (boost::make_tuple (w.visited_nodes.end (), expected.end ()))))
  {
    auto const& got = p.get<0> ();
    auto const& ex = p.get<1> ();

    if (got != ex)
    {
      std::cerr << "got node '" << got << "', expected '" << ex << "'\n";
      code = 1;
    }
  }

  return code;
}
