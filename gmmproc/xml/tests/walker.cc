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

class TestWalker : public Gmmproc::Xml::ConstWalker
{
public:
  std::vector<Info> infos;

private:
  bool doc (Gmmproc::Xml::DocumentConstView&,
            int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("d"s, depth);
    return true;
  }

  bool node (Gmmproc::Xml::NodeConstView& node,
             int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("n: "s + node->name ().to_string (), depth);
    return true;
  }

  bool text (Gmmproc::Xml::TextConstView& text,
             int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("t: "s + text->text ().to_string (), depth);
    return true;
  }

  bool postprocess_node (Gmmproc::Xml::NodeConstView& node,
                         int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("-n: "s + node->name ().to_string (), depth);
    return true;
  }

  bool postprocess_doc (Gmmproc::Xml::DocumentConstView&,
                        int depth) override
  {
    using namespace std::string_literals;
    infos.emplace_back ("-d"s, depth);
    return true;
  }
};

Gmmproc::Xml::TextView
add_text (Gmmproc::Xml::NodeView& view,
          Gmmproc::Xml::Type::StringView text)
{
  auto text_view = view->insert_text_at (view->all ().end ());

  text_view->set_text (text);

  return text_view;
}

Gmmproc::Xml::NodeView
add_child (Gmmproc::Xml::NodeView& view,
           Gmmproc::Xml::Type::StringView name)
{
  return view->insert_child_at (view->all ().end (), name);
}

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
  add_text (a_node, "1");
  auto b_node = add_child (a_node, "b");
  add_text (b_node, "2");
  add_child (b_node, "c");
  add_child (b_node, "d");
  auto e_node = add_child (a_node, "e");
  add_text (e_node, "3");
  add_child (e_node, "f");
  add_child (e_node, "g");

  return bundle;
}

} // anonymous namespace

TEST_CASE ("walker", "[walker]") {
  auto bundle = get_test_bundle ();
  auto doc = bundle.document ();
  auto walker = TestWalker {};
  auto expected = std::vector<Info> {{"d", 0}, {"n: a", 1}, {"t: 1", 2}, {"n: b", 2}, {"t: 2", 3}, {"n: c", 3}, {"-n: c", 3}, {"n: d", 3}, {"-n: d", 3}, {"-n: b", 2}, {"n: e", 2}, {"t: 3", 3}, {"n: f", 3}, {"-n: f", 3}, {"n: g", 3}, {"-n: g", 3}, {"-n: e", 2}, {"-n: a", 1}, {"-d", 0}};

  walker.walk (doc);

  CHECK (expected == walker.infos);
}
