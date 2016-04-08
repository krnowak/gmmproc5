#include <gmmproc/utils/str.hh>
#include <gmmproc/xml/base/xml.hh>
#include <gmmproc/xml/node-base.hh>

#include <ext/kr/kr.hh>

#include <pugixml.hpp>

#include <iostream>
#include <memory>

template <typename R, typename... Args>
class Functor
{
  Functor() = default;
  Functor(R(Args...) f) : opt {f} {}
  std::experimental::optional<R(Args...)> opt;
};

template <typename R, typename... Args>
Functor<R, Args...>
make_functor(R(Args...) f)
{
  return Functor<R, Args> {f};
}

class ArrayTag;
class TypeTag;

// names
using NameName = Gmmproc::Utils::StaticString<'n', 'a', 'm', 'e'>;
using ArrayName = Gmmproc::Utils::StaticString<'a', 'r', 'r', 'a', 'y'>;
using TypeName = Gmmproc::Utils::StaticString<'t', 'y', 'p', 'e'>;

using NameAttr = Gmmproc::Xml::StringAttr<NameName>;

using Array = Gmmproc::Xml::NodeBase<
  // attributes
  Kr::TypeList<
    // name attribute - string
    NameAttr
    >,
  // children
  Kr::TypeList<
    // type child - optional with std::optional
    Gmmproc::Xml::OptionalChild<TypeName, TypeTag, Gmmproc::Xml::UseStdOptional>
    >
  >;

using Type = Gmmproc::Xml::NodeBase<
  // attributes
  Kr::TypeList<
    // name attribute - string
    NameAttr
    >,
  // children
  Kr::TypeList<
    // array child - optional with std::unique_ptr
    Gmmproc::Xml::OptionalChild<ArrayName, ArrayTag, Gmmproc::Xml::UseStdUniquePtr>,
    // type child - vector
    Gmmproc::Xml::VectorChild<TypeName, TypeTag>
    >
  >;

using Root = Gmmproc::Xml::Root<TypeName, TypeTag>;

namespace Gmmproc
{

namespace Utils
{

template <>
class TypeToType<ArrayTag> : public TypeToTypeBase<::Array>
{};

template <>
class TypeToType<TypeTag> : public TypeToTypeBase<::Type>
{};

} // namespace Utils

} // namespace Gmmproc

void
print_names (Root const& node, std::size_t indent = 0);
void
print_names (Type const& node, std::size_t indent = 0);
void
print_names (Array const& node, std::size_t indent = 0);

void
print_names (Root const& node, std::size_t indent)
{
  std::string spaces (indent, ' ');
  std::cout << spaces << "docroot\n";

  auto const& type_child = node.get<TypeName> ();
  print_names (type_child, indent + 2);
}

void
print_names (Type const& node, std::size_t indent)
{
  std::string spaces (indent, ' ');
  std::cout << spaces << "type: " << node.get<NameName> () << "\n";

  auto const& array_child = node.get<ArrayName> ();
  if (array_child)
  {
    print_names (*array_child, indent + 2);
  }
  for (auto const& type_child : node.range<TypeName> ())
  {
    print_names (type_child, indent + 2);
  }
}

void
print_names (Array const& node, std::size_t indent)
{
  std::string spaces (indent, ' ');
  std::cout << spaces << "array: " << node.get<NameName> () << "\n";

  auto const& type_child = node.get<TypeName> ();
  if (type_child)
  {
    print_names (*type_child, indent + 2);
  }
}

/*

// type registrar
// specializations should export public type member as 'Type'
template <typename TagType>
class TagToType;

template <typename TagType>
using TagToTypeT = typename TagToType<TagType>::Type;

// old impl
template <typename Child, typename Str>
class Impl
{
  static_assert (Gmmproc::Utils::IsStaticString<Str>::value, "Str is a StaticString type");

public:
  using Type = Child;
  using Name = Str;
};

// new impl
template <typename ChildTag, typename Str>
class Impl2
{
  static_assert (Gmmproc::Utils::IsStaticString<Str>::value, "Str is a StaticString type");

public:
  using Tag = ChildTag;
  using Name = Str;
};

// old node
template<typename ChildImpl>
class Node
{
public:
  void set(pugi::xml_node& node)
  {
    name = node.attribute ("name").value ();

    auto child = node.first_child (ChildImpl::Name::raw.data ());

    if (child)
    {
      m = std::make_unique<typename ChildImpl::Type>(child);
    }
  }

  std::string name;
  std::unique_ptr<typename ChildImpl::Type> m;
};

std::string
get_name(pugi::xml_node const& node)
{
  switch (node.type ()) {
  case pugi::node_document:
    return "docroot";
  case pugi::node_element:
    return node.attribute ("name").value ();
  default:
    return "INVALID";
  }
}

// new node
template<typename ChildImpl>
class Node2
{
  using Child = TagToTypeT<typename ChildImpl::Tag>;
public:
  void set(pugi::xml_node& node)
  {
    name = get_name (node);

    auto child = node.child (ChildImpl::Name::raw.data ());

    if (child)
    {
      m = std::make_unique<Child> ();
      m->set (child);
    }
  }

  std::string name;
  std::unique_ptr<Child> m;
};

// tags
class TypeTag;
class ArrayTag;

// names
using TypeName = Gmmproc::Utils::StaticString<'t', 'y', 'p', 'e'>;
using ArrayName = Gmmproc::Utils::StaticString<'a', 'r', 'r', 'a', 'y'>;

// old stuff
//using ArrayNode = Node<Impl<TypeNode, TypeName>>;
//using TypeNode = Node<Impl<ArrayNode, ArrayName>>;
//using Root = Node<Impl<TypeNode, TypeName>>;

// new stuff
using ArrayNode = Node2<Impl2<TypeTag, TypeName>>;
using TypeNode = Node2<Impl2<ArrayTag, ArrayName>>;
using Root = Node2<Impl2<TypeTag, TypeName>>;

// register tags
template <>
class TagToType<TypeTag>
{
public:
  using Type = TypeNode;
};

template <>
class TagToType<ArrayTag>
{
public:
  using Type = ArrayNode;
};

template <typename NodeType>
void
print_names (NodeType& node, std::size_t indent = 0)
{
  std::string spaces (indent, ' ');
  std::cout << spaces << node.name << "\n";
  if (node.m)
  {
    print_names (*node.m, indent + 2);
  }
}

*/

int
main (int, char** argv)
{
  if (!argv[1])
  {
    std::cerr << "Expected an xml file\n";
    return 1;
  }

  /*

  pugi::xml_document doc;
  auto result = doc.load_file (argv[1]);

  if (!result)
  {
    std::cerr << "Failed to parse - " << result.description () << "\n";
    return 1;
  }

  Root root;
  root.set (doc);
  print_names (root);

  */

  Gmmproc::Xml::Base::Document doc (argv[1]);
  auto doc_node = doc.as_node ();
  Root root;
  root.process_node (doc_node);
  print_names (root);
}
