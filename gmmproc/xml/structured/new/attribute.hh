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

namespace Basic
{

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
    generate (Xml::Node& node)
    {
      return TODO;
    }
  };
};

class OptBool
{
  using Type = std::experimental::optional<bool>;
  class Generator
  {
  public:
    static Type
    generate (Xml::Node& node)
    {
      return TODO;
    }
  };
}

template <bool DefaultValueP>
class Bool
{
  using Type = bool;
  class Generator
  {
  public:
    static Type
    generate (Xml::Node& node)
    {
      return TODO;
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
          // TODO: get attribute name from registry
          using TypeGenerator = typename AttributeTypeP::Generator;
          using Name = Registry::AttrInfo<AttributeKeyP>::Name;

          auto attr = node.attribute (Name::raw);
          if (!attr)
          {
            throw something{};
          }

          return TypeGenerator::generate (attr->value ());
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
