#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iterator>
#include <functional>

#include <experimental/optional>

#include <pugixml.hpp>

#include "kr.hh"
#include "str.hh"

using Str = std::string;

class InvalidVersion : public std::runtime_error
{
public:
  InvalidVersion (std::string const& str)
    : std::runtime_error (get_error (str))
  {}

private:
  static std::string
  get_error (std::string const& str)
  {
    std::ostringstream oss;

    oss << "Expected a version in form of two integers separated with a dot, got '" << str << "'";
    return oss.str ();
  }
};

class Version
{
public:
  static Version
  from_string (std::string const& str)
  {
    std::smatch results;

    if (!std::regex_match (str, results, re))
    {
      throw InvalidVersion (str);
    }
    major = std::stoi (results[1]);
    minor = std::stoi (results[2]);
  }

private:
  static const std::regex re = std::regex {r"(^(\d+)\.(\d+)$)"};
  unsigned major;
  unsigned minor;
};

class AttributesOnly {};
class ChildrenOnly {};
constexpr AttributesOnly Attrs = AttributesOnly{};
constexpr ChildrenOnly Children = ChildrenOnly{};

class InvalidParserAssumption : public std::logic_error
{
public:
  InvalidParserAssumption (AttributesOnly, std::string const& node_name, std::string const& attribute)
    : std::logic_error {get_error (node_name, "attribute", attribute)}
  {}

  InvalidParserAssumption (ChildrenOnly, std::string const& node_name, std::string const& child)
    : std::logic_error {get_error (node_name, "child", child)}
  {}

private:
  static std::string
  get_error (std::string const& node_name, std::string const& type, std::string const& name)
  {
    std::ostringstream oss;

    oss << "Unexpected " << type << " named '" << name << "' of node named '" << node_name << "' found";
    return oss.str ();
  }
};

class Checker
{
public:
  Checker(AttributesOnly, SVector const& attrs)
    : m_attrs{attrs},
      m_children{}
  {}

  Checker(AttributesOnly, SVector&& attrs)
    : m_attrs{std::move (attrs)},
      m_children{}
  {}

  Checker(ChildrenOnly, SVector const& children)
    : m_attrs{},
      m_children{children}
  {}

  Checker(ChildrenOnly, SVector&& children)
    : m_attrs{},
      m_children{std::move (children)}
  {}

  Checker(SVector const& attrs, SVector const& children)
    : m_attrs{attrs},
      m_children{children}
  {}

  Checker(SVector&& attrs, SVector&& children)
    : m_attrs{std::move (attrs)},
      m_children{std::move (children)}
  {}

  void
  process_node (pugi::xml_node const& node) const
  {
    for (auto const& attr : m_attrs)
    {
      if (node.attribute (attr.c_str ()))
      {
        throw InvalidParserAssumption {AttributesOnly, node.name (), attr};
      }
    }
    for (auto const& child : m_children)
    {
      if (node.child (child.c_str()))
      {
        throw InvalidParserAssumption {ChildrenOnly, node.name(), child};
      }
    }
  }

private:
  SVector m_attrs;
  SVector m_children;
};

template <typename AttrTr>
class AttrVecWrapper
{
public:
  static void call(SVector& v)
  {
    v.push_back (GenericAttr::Name::raw.data ());
  };
};

template <typename TraitsList, template <typename Tr> class TraitsHandler>
class Vectorize
{
public:
  static SVector get ()
  {
    SVector v;

    v.reserve (Kr::Len<TraitsList>::Value);
    Kr::MultiCall<TraitsHandler, TraitsList>::call (this, v);
    return v;
  };
};

// TODO: type and array should be a part of some union or boost::variant

// class some node base wrapper for process_node
template<>
class NodeBaseWrapper

template <typename Attrs, typename Children>
class NodeBase
{
public:
  class AbsenceChecker : public Checker
  {
  public:
    AbsenceChecker ()
      : Checker(Vectorize<Attrs>::vector, Vectorize<TypeList<Children...>>::vector)
    {}
  };

  void process_node (pugi::xml_node& node)
  {

  }
};

class Docs : public NodeBase<TypeList<StringAttr<DocName>, StringAttr<DocDeprecatedName>>, TypeList<>>
{};

class Docs
{
public:
  class AbsenceChecker : public Checker
  {
  public:
    AbsenceChecker ()
      : Checker (Children, SVector {"doc", "doc-deprecated"})
    {}
  };

  void
  process_node (pugi::xml_node const& node)
  {
    doc = node.child_value ("doc");
    doc_deprecated = node.child_value ("doc-deprecated");
  }

private:
  Str doc;
  Str doc_deprecated;
};

class Versioned
{
public:
  class AbsenceChecker : public Checker
  {
  public:
    AbsenceChecker ()
      : Checker (Attrs, SVector {"version"})
    {}
  };

  void process_node (pugi::xml_node const& node)
  {
    version = Version::from_string (node.attribute ("version").value ())
  }

private:
  Version version;
};

enum class HasDeprecations
{
  No,
  Yes
};

class Deprecations
{
  bool deprecated;
  Version deprecated_version;
};

enum class HasCType
{
  No,
  Yes
};

class CType
{
  Str c_type;
};

enum class HasName
{
  No,
  Yes
};

class Name
{
  Str name;
};

enum class HasIntrospectable
{
  No,
  Yes
};

class Introspectable
{
  bool introspectable;
};

template <HasDocs docable, HasVersion versioned, HasDeprecations deprecable, HasCType typed, HasName named>
class Common
{
};

enum class Throwing
{
  Yes,
  No
};

enum class Shadowable
{
  Yes,
  No
};

enum class Identifiable
{
  Yes,
  No
};

template <typename ParamsType, Throwing throwing, Shadowable shadowable, Identifiable identifiable, HasCType typed>
class BasicCallable : public Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::Yes, typed, HasName::Yes>
{
  bool introspectable;

  ParamsType parameters;
  ReturnValue return_value;
};





class Alias : public Common<HasDocs::Yes, HasVersion::No, HasDeprecations::No, HasCType::Yes, HasName::Yes>
{
  Type type;
};

class Array : public Common<HasDocs::No, HasVersion::No, HasDeprecations::No, HasCType::Yes, HasName::Yes>
{
  unsigned fixed_size;
  unsigned length;
  bool zero_terminated;

  union {
    Array array;
    Type type;
  };
};

class Bitfield : Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::Yes, HasCType::Yes, HasName::Yes>
{
  Str glib_type_name;
  Str glib_get_type;

  std::vector<Member> members;
};

class Callback : public BasicCallable<Parameters, Throwing::No, Shadowable::No, Identifiable::No, HasCType::Yes>
{
};

class Class : public Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::Yes, HasCType::Yes, HasName::Yes>
{
  bool abstract;
  Str c_symbol_prefix;
  bool glib_fundamental;
  Str glib_get_type;
  Str glib_type_name;
  Str glib_type_struct;
  Str parent;

  Map<Constructor> constructors;
  Map<Field> fields;
  Map<Function> functions;
  Map<GLibSignal> signals;
  SVector implemented_interfaces;
  Map<Method> methods;
  Map<Property> properties;
  Map<VMethod> virtual_methods;
};

class Constant : public Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::Yes, HasCType::Yes, HasName::Yes>
{
  bool introspectable;
  Str value;

  Type type;
};

class Constructor : public BasicCallable<Parameters, Throwing::Yes, Shadowable::Yes, Identifiable::Yes, HasCType::No>
{
};

class Enumeration : public Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::Yes, HasCType::Yes, HasName::Yes>
{
  Str glib_error_domain;
  Str glib_get_type;
  Str glib_type_name;

  Map<Function> functions;
  std::vector<Member> members;
};

class Field : public Common<HasDocs::Yes, HasVersion::No, HasDeprecations::No, HasCType::No, HasName::Yes>
{
  unsigned bits;
  bool introspectable;
  bool internal; // private
  bool readable; // default true
  bool writable; // default false

  union
  {
    Array array;
    Field field;
    Callback callback;
  }
};

class Function : public BasicCallable<Parameters, Throwing::Yes, Shadowable::Yes, Identifiable::Yes, HasCType::No>
{
  Str moved_to;
};

class GLibBoxed
{
  Str c_symbol_prefix;
  Str glib_get_type;
  Str glib_name;
  Str glib_type_name;
};

enum class When
{
  First,
  Last,
  Cleanup
};

class GLibSignal : public BasicCallable<Parameters, Throwing::No, Shadowable::No, Identifiable::No, HasCType::No>
{
  Str action;
  bool detailed;
  bool no_hooks;
  bool no_recurse;
  When when;
};

class Interface : public Common<HasDocs::Yes, HasVersion::Yes, HasDeprecations::No, HasCType::Yes, HasName::Yes>
{
  Str c_symbol_prefix;
  Str glib_get_type;
  Str glib_type_name;
  Str glib_type_struct;

  Map<Function> functions;
  Map<GLibSignal> signals;
  Map<Method> methods;
  Str prerequisite;
  Map<Property> properties;
  Map<VMethod> virtual_methods;
};

class Member : Common<HasDocs::Yes, HasVersion::No, HasDeprecations::No, HasCType::No, HasName::Yes>
{
  Str value;
  Str c_identifier;
  Str glib_nick;
};

class Method : public BasicCallable<ClassParameters, Throwing::Yes, Shadowable::Yes, Identifiable::Yes, HasCType::No>
{
  Str moved_to;

  Str doc_version // ignore it
};

class Namespace : Common<HasDocs::No, HasVersion::Yes, HasDeprecations::No, HasCType::No, HasName::Yes>
{
  Version version;
  Str shared_lib;
  SVector c_id_prefixes;
  SVector c_symbol_prefixes;

  Map<Alias> aliases;
  Map<Bitfield> bitfields;
  Map<Callback> callbacks;
  Map<Class> classes;
  Map<Constant> constants;
  Map<Enum> enumerations;
  Map<Function> functions;
  Map<GlibBoxed> boxeds;
  Map<Interface> interfaces;
  Map<Record> records;
  Map<Union> unions;
};

enum class Direction
{
  In,
  Out,
  InOut
};

class Parameter : Common<HasDocs::Yes, HasVersion::No, HasDeprecations::No, HasCType::No, HasName::Yes>
{
  bool allow_none; // default false
  bool caller_allocates; // default true
  unsigned closure; // what is this for?
  unsigned destroy; // 0-based index of destroy notify - not taking instance-parameter into account
  Direction direction; // default in
};

class Parameters
{
  std::vector<Parameter> parameters;
};

class ClassParameters : public Parameters
{
  Parameter instance_parameter;
};

class Repository
{
  SVector packages;
  SVector c_includes;

  Namespace ns;

  Str ns_name; // extra
};

class VMethod : public BasicCallable<ClassParameters, Throwing::Yes, Shadowable::No, Identifiable::No, HasCType::No>
{
  Str invoker;
};
