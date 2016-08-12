#include <gmmproc/xml/xml2.hh>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

Gmmproc::Xml::Bundle
get_bundle ()
{
  Gmmproc::Xml::Bundle bundle;
  auto doc = bundle.document ();
  auto a_node = doc.add_root ("a");

  //    __a__
  //   |  |  |
  //   b  c  d
  a_node.add_text ().set_text ("text1");
  a_node.add_child ("b");
  a_node.add_text ().set_text ("text2");
  a_node.add_child ("c");
  a_node.add_text ().set_text ("text3");
  a_node.add_child ("d");
  a_node.add_text ().set_text ("text4");

  return bundle;
}

Gmmproc::Xml::Node
must_get_doc_root (Gmmproc::Xml::Document const& doc)
{
  auto root = doc.root_tag ();

  if (!root)
  {
    throw std::runtime_error ("no root tag");
  }

  return *root;
}

template <typename RangeP>
bool
compare_ranges (RangeP const& got_range,
                RangeP const& expected_range)
{
  auto fail = false;
  auto const gb = std::cbegin (got_range);
  auto const ge = std::cend (got_range);
  auto const eb = std::cbegin (expected_range);
  auto const ee = std::cend (expected_range);
  auto const gs = std::distance (gb, ge);
  auto const es = std::distance (eb, ee);

  if (gs != es)
  {
    fail = true;
    std::cerr << "got size: " << gs << ", expected: " << es << "\n";
  }

  for (auto const& p : boost::make_iterator_range (boost::make_zip_iterator (boost::make_tuple (got_range.begin (), expected_range.begin ())),
                                                   boost::make_zip_iterator (boost::make_tuple (got_range.end (), expected_range.end ()))))
  {
    auto const& g = p.get<0> ();
    auto const& ex = p.get<1> ();

    if (g != ex)
    {
      std::cerr << "got '" << g << "', expected '" << ex << "'\n";
      fail = true;
    }
  }

  return fail;
}

bool
test_node_parent ()
{
  auto doc = get_doc ();
  auto fail = false;
  auto root = must_get_doc_root (doc);

  if (root.parent ())
  {
    std::cerr << "toplevel node should not have any parent\n";
    fail = true;
  }

  for (auto c : root.children ())
  {
    if (c.parent() != root)
    {
      std::cerr << "wrong parent of the toplevel node's children\n";
      fail = true;
    }
  }

  for (auto t : root.texts ())
  {
    if (t.parent() != root)
    {
      std::cerr << "wrong parent of the toplevel node's texts\n";
      fail = true;
    }
  }

  return fail;
}

bool
test_children ()
{
  std::vector<std::string> expected {"b", "c", "d"};
  std::vector<std::string> got {};
  auto doc = get_doc ();
  auto root = must_get_doc_root (doc);

  for (auto c : root.children ())
  {
    got.push_back (c.name ());
  }

  return compare_ranges (got, expected);
}

bool
test_texts ()
{
  std::vector<std::string> expected {"text1", "text2", "text2", "text3"};
  std::vector<std::string> got {};
  auto doc = get_doc ();
  auto root = must_get_doc_root (doc);

  for (auto c : root.texts ())
  {
    got.push_back (c.text ());
  }

  return compare_ranges (got, expected);
}

class NodeVisitor
  : public boost::static_visitor<std::string>
{
public:
  std::string
  operator()(Gmmproc::Xml::Node const& node) const
  {
    return std::string {"node: "}.append (node.name ());
  }

  std::string
  operator()(Gmmproc::Xml::Text const& text) const
  {
    return std::string {"text: "}.append (text.text ());
  }
};

bool
test_all ()
{
  std::vector<std::string> expected {"text: text1", "node: b", "text: text2", "node: c", "text: text3", "node: d", "text: text4"};
  std::vector<std::string> got {};
  auto doc = get_doc ();
  auto root = must_get_doc_root (doc);

  for (auto a : root.all ())
  {
    got.push_back (boost::apply_visitor (NodeVisitor {}, a));
  }

  return compare_ranges (got, expected);
}

} // anonymous namespace

int
main ()
{
  std::vector<std::string> expected {"b", "c", "d"};
  std::vector<std::string> got {};
  auto doc = get_doc ();

  for (auto c : a_node.children ())
  {
    got.push_back (c.name ());
  }

  auto code = 0;
  if (got.size () != expected.size ())
  {
    fail = true;
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
      fail = true;
    }
  }

  if (a_node.parent ())
  {
    std::cerr << "toplevel node should not have any parent\n";
    fail = true;
  }
  return code;
}
