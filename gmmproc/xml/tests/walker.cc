#include "misc.hh"

#include <gmmproc/xml/xml2.hh>

#include <catch/catch.hpp>

#include <string>

namespace
{

struct Info
{
  Info(std::string&& n, int d)
    : name {std::move (n)},
      depth {d}
  {}
  std::string name;
  int depth;
};

std::ostream& operator<< (std::ostream& os, Info const& info)
{
  os << '{' << info.name << ", " << info.depth << '}';
  return os;
}


bool
operator== (Info const& n1, Info const& n2)
{
  return n1.name == n2.name && n1.depth == n2.depth;
}

bool
operator!= (Info const& n1, Info const& n2)
{
  return !(n1 == n2);
}

class TestWalker : public Gmmproc::Xml::ConstWalker
{
public:
  std::vector<Info> infos;

private:
  bool doc (Gmmproc::Xml::Type::Wrapper<Gmmproc::Xml::Document const>&,
            int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("d"s, depth);
    return true;
  }

  bool node (Gmmproc::Xml::Type::Wrapper<Gmmproc::Xml::Node const>& node,
             int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("n: "s + node->name ().to_string (), depth);
    return true;
  }

  bool text (Gmmproc::Xml::Type::Wrapper<Gmmproc::Xml::Text const>& text,
             int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("t: "s + text->text ().to_string (), depth);
    return true;
  }

  bool postprocess_node (Gmmproc::Xml::Type::Wrapper<Gmmproc::Xml::Node const>& node,
                         int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("-n: "s + node->name ().to_string (), depth);
    return true;
  }

  bool postprocess_doc (Gmmproc::Xml::Type::Wrapper<Gmmproc::Xml::Document const>&,
                        int depth) const override
  {
    using namespace std::string_literals;
    infos.emplace_back ("-d"s, depth);
    return true;
  }
};

Gmmproc::Xml::Bundle
get_test_bundle ()
{
  auto bundle = Gmmproc::Xml::Bundle {};
  auto doc = bundle.document ();

  //  _____a_____
  // |     |     |
  // 1    _b_   _e_
  //     | | | | | |
  //     2 c d 3 f g
  auto a_node = doc->add_root ("a");
  a_node->add_text ()->set_text ("1");
  auto b_node = a_node->add_child ("b");
  b_node->add_text ()->set_text ("2");
  b_node->add_child ("c");
  b_node->add_child ("d");
  auto e_node = a_node->add_child ("e");
  e_node->add_text ()->set_text ("3");
  e_node->add_child ("f");
  e_node->add_child ("g");

  return bundle;
}

} // anonymous namespace

TEST_CASE ("walker", "[walker]") {
  auto bundle = WalkerTest::get_test_bundle ();
  auto doc = bundle.document ();
  auto walker = WalkerTest::TestWalker {};
  auto expected = std::vector<WalkerTest::Info> {{"d", 0}, {"n: a", 1}, {"t: 1", 2}, {"n: b", 2}, {"t: 2", 3}, {"n: c", 3}, {"-n: c", 3}, {"n: d", 3}, {"-n: d", 3}, {"-n: b", 2}, {"n: e", 2}, {"t: 3", 3}, {"n: f", 3}, {"-n: f", 3}, {"n: g", 3}, {"-n: g", 3}, {"-n: e", 2}, {"-n: a", 1}, {"-d", 0}};

  walker.walk (doc);

  CHECK (expected == walker.infos);
}

#endif // GMMPROC_XML_TESTS_WALKER_HH
