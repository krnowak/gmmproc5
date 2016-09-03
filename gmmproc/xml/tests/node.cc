#include <gmmproc/xml/xml.hh>
#include <gmmproc/xml/extra.hh>

#include <catch/catch.hpp>

#include <string>
#include <vector>

namespace
{

Gmmproc::Xml::Bundle
get_bundle ()
{
  Gmmproc::Xml::Bundle bundle;
  auto doc = bundle.document ();
  auto a_node = doc->add_root ("a");

  //    __a__
  //   |  |  |
  //   b  c  d
  Gmmproc::Xml::Extra::add_text (a_node, "text1");
  Gmmproc::Xml::Extra::add_child (a_node, "b");
  Gmmproc::Xml::Extra::add_text (a_node, "text2");
  Gmmproc::Xml::Extra::add_child (a_node, "c");
  Gmmproc::Xml::Extra::add_text (a_node, "text3");
  Gmmproc::Xml::Extra::add_child (a_node, "d");
  Gmmproc::Xml::Extra::add_text (a_node, "text4");

  Gmmproc::Xml::Extra::add_attribute (a_node, "attr1", "value1");
  Gmmproc::Xml::Extra::add_attribute (a_node, "attr2", "value2");
  Gmmproc::Xml::Extra::add_attribute (a_node, "attr3", "value3");

  return bundle;
}

} // anonymous namespace

TEST_CASE ("empty bundle", "[base]") {
  auto bundle = Gmmproc::Xml::Bundle {};
  auto doc = bundle.document ();
  auto basic_doc = doc->as_basic_node ();

  CHECK (!doc->root_tag ());

  auto children = basic_doc->children ();

  CHECK (std::distance (children.begin (), children.end ()) == 0);
}

TEST_CASE ("basic functions work", "[base]") {
  auto bundle = get_bundle ();
  auto doc = bundle.document ();
  auto root_opt = doc->root_tag ();

  REQUIRE (root_opt);

  auto root = *root_opt;

  SECTION ("all children have the same parent") {
    for (auto c : root->children ())
    {
      CHECK (c->basic_parent() == root);
    }
  }

  SECTION ("all texts have the same parent") {
    for (auto t : root->texts ())
    {
      CHECK (t->parent() == root);
    }
  }

  SECTION ("root node has expected children") {
    std::vector<std::string> expected {"b", "c", "d"};
    std::vector<std::string> got {};

    for (auto c : root->children ())
    {
      got.push_back (c->name ().to_string ());
    }

    CHECK (expected == got);
  }

  SECTION ("root node has expected texts") {
    std::vector<std::string> expected {"text1", "text2", "text3", "text4"};
    std::vector<std::string> got {};

    for (auto t : root->texts ())
    {
      got.push_back (t->text ().to_string ());
    }

    CHECK (expected == got);
  }

  SECTION ("root node has expected children and texts") {
    std::vector<std::string> expected {"text: text1", "node: b", "text: text2", "node: c", "text: text3", "node: d", "text: text4"};
    std::vector<std::string> got {};
    class NodeVisitor : public boost::static_visitor<std::string>
    {
    public:
      std::string
      operator() (Gmmproc::Xml::TextView& text) const
      {
        using namespace std::string_literals;
        return "text: "s + text->text ().to_string ();
      }

      std::string
      operator() (Gmmproc::Xml::NodeView& node) const
      {
        using namespace std::string_literals;
        return "node: "s + node->name ().to_string ();
      }
    };

    for (auto a : root->all ())
    {
      got.push_back (boost::apply_visitor (NodeVisitor {}, a));
    }

    CHECK (expected == got);
  }
}

TEST_CASE ("swap", "[base]") {
  auto empty_bundle = Gmmproc::Xml::Bundle {};
  auto filled_bundle = get_bundle ();

  CHECK (!empty_bundle.document ()->root_tag ());
  CHECK (filled_bundle.document ()->root_tag ());

  SECTION ("bundle std") {
    using std::swap;

    swap (empty_bundle, filled_bundle);
    CHECK (empty_bundle.document ()->root_tag ());
    CHECK (!filled_bundle.document ()->root_tag ());
  }

  SECTION ("bundle") {
    empty_bundle. swap (filled_bundle);
    CHECK (empty_bundle.document ()->root_tag ());
    CHECK (!filled_bundle.document ()->root_tag ());
  }

  SECTION ("document std") {
    auto empty_document = empty_bundle.document ();
    auto filled_document = filled_bundle.document ();

    using std::swap;

    swap (empty_document, filled_document);
    // changes the view, but...
    CHECK (empty_document->root_tag ());
    CHECK (!filled_document->root_tag ());
    // does not change the bundle
    CHECK (!empty_bundle.document ()->root_tag ());
    CHECK (filled_bundle.document ()->root_tag ());
  }

  SECTION ("document") {
    auto empty_document = empty_bundle.document ();
    auto filled_document = filled_bundle.document ();

    using std::swap;

    empty_document.swap (filled_document);
    // changes the view, but...
    CHECK (empty_document->root_tag ());
    CHECK (!filled_document->root_tag ());
    // does not change the bundle
    CHECK (!empty_bundle.document ()->root_tag ());
    CHECK (filled_bundle.document ()->root_tag ());
  }
}

TEST_CASE ("move assignment", "[base]") {
  auto empty_bundle = Gmmproc::Xml::Bundle {};

  CHECK (!empty_bundle.document ()->root_tag ());

  SECTION ("bundle") {
    empty_bundle = get_bundle ();

    CHECK (empty_bundle.document ()->root_tag ());
  }

  SECTION ("document") {
    auto bundle = get_bundle ();
    auto empty_document = empty_bundle.document ();

    using std::swap;

    empty_document = bundle.document ();
    // changes the view, but...
    CHECK (empty_document->root_tag ());
    // does not change the bundle
    CHECK (!empty_bundle.document ()->root_tag ());
    CHECK (bundle.document ()->root_tag ());
  }
}

TEST_CASE ("equality operator", "[base]") {
  auto bundle1 = get_bundle ();
  auto bundle2 = get_bundle ();

  SECTION ("document") {
    auto doc1 = bundle1.document ();
    auto doc2 = bundle2.document ();
    auto doc1_again = bundle1.document ();

    CHECK (doc1 != doc2);
    CHECK (doc1 == doc1_again);

    CHECK (doc1->as_basic_node () != doc2);
    CHECK (doc1 != doc2->as_basic_node ());
    CHECK (doc1->as_basic_node () == doc1_again);
    CHECK (doc1 == doc1_again->as_basic_node ());
  }

  SECTION ("node") {
    auto node1 = *bundle1.document ()->root_tag ();
    auto node2 = *bundle2.document ()->root_tag ();
    auto node1_again = *bundle1.document ()->root_tag ();

    CHECK (node1 != node2);
    CHECK (node1 == node1_again);

    CHECK (node1->as_basic_node () != node2);
    CHECK (node1 != node2->as_basic_node ());
    CHECK (node1->as_basic_node () == node1_again);
    CHECK (node1 == node1_again->as_basic_node ());
  }

  SECTION ("text") {
    auto text1 = *((*bundle1.document ()->root_tag ())->texts().begin ());
    auto text2 = *((*bundle2.document ()->root_tag ())->texts().begin ());
    auto text1_again = *((*bundle1.document ()->root_tag ())->texts().begin ());

    CHECK (text1 != text2);
    CHECK (text1 == text1_again);
  }

  SECTION ("attribute") {
    auto attribute1 = *((*bundle1.document ()->root_tag ())->attributes().begin ());
    auto attribute2 = *((*bundle2.document ()->root_tag ())->attributes().begin ());
    auto attribute1_again = *((*bundle1.document ()->root_tag ())->attributes().begin ());

    CHECK (attribute1 != attribute2);
    CHECK (attribute1 == attribute1_again);
  }

  SECTION ("basic node") {
    auto basic_node1 = bundle1.document ()->as_basic_node ();
    auto basic_node2 = bundle2.document ()->as_basic_node ();
    auto basic_node1_again = bundle1.document ()->as_basic_node ();

    CHECK (basic_node1 != basic_node2);
    CHECK (basic_node1 == basic_node1_again);
  }
}
