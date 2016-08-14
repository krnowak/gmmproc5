#ifndef GMMPROC_XML_TESTS_MISC_HH
#define GMMPROC_XML_TESTS_MISC_HH

#include <gmmproc/xml/xml2.hh>

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
  a_node->add_text ()->set_text ("text1");
  a_node->add_child ("b");
  a_node->add_text ()->set_text ("text2");
  a_node->add_child ("c");
  a_node->add_text ()->set_text ("text3");
  a_node->add_child ("d");
  a_node->add_text ()->set_text ("text4");

  a_node->add_attribute ("attr1")->set_value ("value1");
  a_node->add_attribute ("attr2")->set_value ("value2");
  a_node->add_attribute ("attr3")->set_value ("value3");
  return bundle;
}

} // anonymous namespace

#endif // GMMPROC_XML_TESTS_MISC_HH
