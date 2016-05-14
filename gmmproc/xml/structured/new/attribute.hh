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

  // TODO: process
};

class String
{
  using Type = std::string;

  // TODO: process?
};

class Int
{
  using Type = int;

  // TODO: process?
};

class OptBool
{
  using Type = std::experimental::optional<bool>;

  // TODO: process?
}

template <bool DefaultValueP>
class Bool
{
  using Type = bool;

  // TODO: process?
};

} // namespace Basic

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_ATTRIBUTE_HH
