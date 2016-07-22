#ifndef GMMPROC_XML_STRUCTURED_ATTRIBUTE_HH
#define GMMPROC_XML_STRUCTURED_ATTRIBUTE_HH

#include <gmmproc/xml/structured/detail/getters.hh>

#include <string>
#include <experimental/optional>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

// TODO: Name this namespace differently.
namespace Basic
{

template <typename AttributeTypeP>
class Optional
{
private:
  using UnderlyingType = typename AttributeTypeP::Type;
  using Type = std::experimental::optional<UnderlyingType>;

public:
  class Generator
  {
  public:
    static Type
    generate (std::experimental::optional<Xml::Attribute> const& attr)
    {
      if (!attr)
        return Type {};

      return Type {UnderlyingType::generate (attr->value ());
    }
  };
};

template <typename AttributeTypeP>
class Single
{
  using Type = typename AttributeTypeP::Type;
  class Generator
  {
  public:
    static Type
    generate (std::experimental::optional<Xml::Attribute> const& attr)
    {
      if (!attr)
        throw something{};

      return Type::generate (attr->value ());
    }
  };
};

class String
{
  using Type = std::string;
  class Generator
  {
  public:
    static Type
    generate (std::string const& value)
    {
      return value;
    }
  };
};

class Int
{
  using Type = int;
  class Generator
  {
  public:
    static Type
    generate (std::string& const value)
    {
      std::size_t idx {};
      Type num = std::stoi (value, &idx);

      if (idx < value.size ())
      {
        throw something {};
      }
      return num;
    }
  };
};

class Bool
{
  using Type = bool;
  class Generator
  {
  public:
    static Type
    generate (std::string const& value)
    {
      if (value == "0" || value == "f" || value == "false")
        return false;
      if (value == "1" || value == "t" || value == "true")
        return true;
      throw something {};
    }
  };
}

template <bool DefaultValueP>
class DefBool
{
  using Type = bool;
  class Generator
  {
  public:
    static Type
    generate (std::string const& value)
    {
      if (value.empty ())
        return DefaultValueP;
      return Bool::Generator::generate (value);
    }
  };
};

template <typename AttributeTypeP, typename AttributeKeyP>
class Attribute
{
public:
  class GetContainedType
  {
  public:
    class Apply
    {
    public:
      using Type = typename AttributeTypeP::Type;
    };
  };

  class GetGetter
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Detail::SingleGetterWithDefaultPolicy<StorageP>;
    };
  };

  class GetAccessKey
  {
    class Apply
    {
    public:
      using Type = AttributeKeyP;
    };
  };

  class GetGenerator
  {
  public:
    class Apply
    {
    private:
      class Generator
      {
      public:
        static auto
        generate (Xml::Node& node)
        {
          using TypeGenerator = typename AttributeTypeP::Generator;
          using Name = Registry::AttrInfo<AttributeKeyP>::Name;

          return TypeGenerator::generate (node.attribute (Name::raw));
        }
      };

    public:
      using Type = Generator;
    }
  };
};

} // namespace Basic

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_ATTRIBUTE_HH
