#include <gmmproc/xml/structured/structured.hh>

#include <boost/hana/string.hpp>
#include <boost/hana/type.hpp>

#include <sstream>

namespace Xml = Gmmproc::Xml;

namespace Foo
{

namespace Xmls = Xml::Structured;

class A {};
class B {};

// access keys
constexpr A a;
constexpr B b;

namespace Names
{

// names
constexpr auto a = BOOST_HANA_STRING ("a");
constexpr auto b = BOOST_HANA_STRING ("b");

} // namespace Names

namespace Tags
{

// node tags
class A {};
class B {};

} // namespace Tags

using BNode = Xmls::Node<>;
using ANode = Xmls::Node<Xmls::Basic::Child<B, Xmls::Basic::Single, Tags::B>>;
using Doc = Xmls::Document<A, Tags::A>;

namespace Tags
{

constexpr auto
get_node_info (A)
{
  return Xmls::Registry::Registered {Names::a, boost::hana::type_c<ANode>};
}

constexpr auto
get_node_info (B)
{
  return Xmls::Registry::Registered {Names::b, boost::hana::type_c<BNode>};
}

} // namespace Tags

} // namespace Foo

int
main ()
{
  std::stringstream ss;

  ss << "<a><b/></a>";

  Xml::Bundle bundle {ss};
  auto doc = bundle.document ();
  Foo::Doc cxx_doc {doc};
  decltype(auto) a_node = cxx_doc.get (Foo::a);
  decltype(auto) b_node = a_node.get (Foo::b);
  (void)b_node;
}
