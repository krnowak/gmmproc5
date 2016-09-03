#include <gmmproc/xml/xml.hh>
#include <gmmproc/xml/extra.hh>

#include <catch/catch.hpp>

#include <string>
#include <vector>

template <typename IteratorP>
using IterCatCheck = std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<IteratorP>::iterator_category>;

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::NodeIterator<Gmmproc::Utils::ViewType::Const>>::value,
               "const node iterator is at least bidirectional");

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::NodeIterator<Gmmproc::Utils::ViewType::Mutable>>::value,
               "mutable node iterator is at least bidirectional");

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::TextIterator<Gmmproc::Utils::ViewType::Const>>::value,
               "const text iterator is at least bidirectional");

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::TextIterator<Gmmproc::Utils::ViewType::Mutable>>::value,
               "mutable text iterator is at least bidirectional");

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::NodeOrTextIterator<Gmmproc::Utils::ViewType::Const>>::value,
               "const node or text iterator is at least bidirectional");

static_assert (IterCatCheck<Gmmproc::Xml::XmlImpl::NodeOrTextIterator<Gmmproc::Utils::ViewType::Mutable>>::value,
               "mutable node or text iterator is at least bidirectional");

TEST_CASE ("insertion", "[base]") {
  auto bundle = Gmmproc::Xml::Bundle {};
  auto doc = bundle.document ();
  auto root = doc->add_root ("root");

  SECTION ("adding children works") {
    auto expected = std::vector<std::string> {"1", "2", "3", "4", "5", "6"};
    auto got = std::vector<std::string> {};

    root->insert_child_at (root->children ().begin (), "3");
    root->insert_child_at (root->children ().begin (), "1");
    root->insert_child_at (root->children ().end (), "4");
    root->insert_child_at (root->children ().end (), "6");
    root->insert_child_at (std::next (root->children ().begin ()), "2");
    root->insert_child_at (std::prev (root->children ().end ()), "5");

    for (auto c : root->children ())
    {
      got.push_back (c->name ().to_string ());
    }

    CHECK (expected == got);
  }

  SECTION ("adding attributes works") {
    auto expected = std::vector<std::string> {"1", "2", "3", "4", "5", "6"};
    auto got = std::vector<std::string> {};

    root->insert_attribute_at (root->attributes ().begin (), "3");
    root->insert_attribute_at (root->attributes ().begin (), "1");
    root->insert_attribute_at (root->attributes ().end (), "4");
    root->insert_attribute_at (root->attributes ().end (), "6");
    root->insert_attribute_at (std::next (root->attributes ().begin ()), "2");
    root->insert_attribute_at (std::prev (root->attributes ().end ()), "5");

    for (auto a : root->attributes ())
    {
      got.push_back (a->name ().to_string ());
    }

    CHECK (expected == got);
  }

  SECTION ("adding texts works") {
    auto expected = std::vector<std::string> {"1", "2", "3", "4", "5", "6"};
    auto types_expected = std::vector<Gmmproc::Xml::TextType> {Gmmproc::Xml::TextType::Parsed, Gmmproc::Xml::TextType::Raw, Gmmproc::Xml::TextType::Parsed, Gmmproc::Xml::TextType::Raw, Gmmproc::Xml::TextType::Parsed, Gmmproc::Xml::TextType::Raw};
    auto got = std::vector<std::string> {};
    auto types_got = std::vector<Gmmproc::Xml::TextType> {};

    root->insert_text_at (root->texts ().begin (), Gmmproc::Xml::TextType::Parsed)->set_text ("3");
    root->insert_text_at (root->texts ().begin (), Gmmproc::Xml::TextType::Parsed)->set_text ("1");
    root->insert_text_at (root->texts ().end (), Gmmproc::Xml::TextType::Raw)->set_text ("4");
    root->insert_text_at (root->texts ().end (), Gmmproc::Xml::TextType::Raw)->set_text ("6");
    root->insert_text_at (std::next (root->texts ().begin ()), Gmmproc::Xml::TextType::Raw)->set_text ("2");
    root->insert_text_at (std::prev (root->texts ().end ()), Gmmproc::Xml::TextType::Parsed)->set_text ("5");

    for (auto t : root->texts ())
    {
      got.push_back (t->text ().to_string ());
      types_got.push_back (t->type ());
    }

    CHECK (expected == got);
    CHECK (types_expected == types_got);
  }
}
