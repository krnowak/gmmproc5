#include <mm/xml/xml.hh>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

int
main ()
{
  std::vector<std::string> expected {"b", "c", "d"};
  std::vector<std::string> got {};

  Mm::Xml::Document doc;

  //    __a__
  //   |  |  |
  //   b  c  d
  auto a_node = doc.add_root ("a");
  a_node.set_text ("this should be ignored");
  a_node.add_child ("b");
  a_node.set_text ("this should be ignored");
  a_node.add_child ("c");
  a_node.set_text ("this should be ignored");
  a_node.add_child ("d");
  a_node.set_text ("this should be ignored");

  for (auto c : a_node.children ())
  {
    got.push_back (c.name ());
  }

  auto code = 0;
  if (got.size () != expected.size ())
  {
    code = 1;
    std::cerr << "got nodes size: " << got.size () << ", expected: " << expected.size () << "\n";
  }

  for (auto const&p : boost::make_iterator_range (boost::make_zip_iterator (boost::make_tuple (got.begin (), expected.begin ())),
                                                  boost::make_zip_iterator (boost::make_tuple (got.end (), expected.end ()))))
  {
    auto const& g = p.get<0> ();
    auto const& ex = p.get<1> ();

    if (g != ex)
    {
      std::cerr << "got node '" << g << "', expected '" << ex << "'\n";
      code = 1;
    }
  }

  if (a_node.parent ())
  {
    std::cerr << "toplevel node should not have any parent\n";
    code = 1;
  }
  return code;
}
