#include <gmmproc/xml/xml.hh>
#include <gmmproc/xml/extra.hh>

#include <catch/catch.hpp>

#include <iterator>

TEST_CASE ("basic parent", "[parent]")
{
  auto bundle = Gmmproc::Xml::Bundle {};
  auto doc = bundle.document ();
  auto root = doc->add_root ("root");
  auto child = root->insert_child_at (root->children ().end (), "child");

  root->insert_child_at (root->children ().end (), "nevermind");

  SECTION ("basic document parent")
  {
    auto basic_parent = root->basic_parent ();

    CHECK (std::distance (basic_parent->children ().begin (), basic_parent->children ().end ()) == 1);
    CHECK (*(basic_parent->children ().begin ()) == root);
    CHECK (basic_parent == doc);
  }

  SECTION ("basic node parent")
  {
    auto basic_parent = child->basic_parent ();

    CHECK (std::distance (basic_parent->children ().begin (), basic_parent->children ().end ()) == 2);
    CHECK (*(basic_parent->children ().begin ()) == child);
    CHECK (basic_parent == root);
  }
}
