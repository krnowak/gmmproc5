#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>
#include <gmmproc/xml/structured/generators.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Basic
{

// Node& get<Node>()
class Single
{
public:
  class GetContainedType
  {
  public:
    template <typename NodeTagP>
    class Apply
    {
    public:
      using Type = typename Registry::template NodeInfo<NodeTagP>::Type;
    };
  };

  class GetGetters
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Detail::SingleGetterWithDefaultPolicy<StorageP>;
    };
  };

  template <typename NodeTagP>
  class GetGenerator
  {
  public:
    class Apply
    {
    public:
      using Type = Generators::SingleGenerator<typename Registry::template NodeInfo<NodeTagP>::Type>;
    };
  };
};

template <typename BasicP, typename NodeTagP> // BasicP - single, optional, vector, mapvector
class Child
{
public:
  class GetContainedType
  {
  public:
    class Apply
    {
    public:
      using Type = Detail::Mpl::ApplyT<typename BasicP::GetContainedType, NodeTagP>;
    };
  };

  class GetAccessInfo
  {
  public:
    template <typename StorageP>
    class Apply
    {
    private:
      using AccessKey = typename Registry::NodeInfo<NodeTagP>::Type;
      using GettersMap = Detail::Mpl::ApplyT<typename BasicP::GetGetters, StorageP>;

    public:
      using Type = Detail::Mpl::Map<Detail::Mpl::Pair<AccessKey, GettersMap>>;
    };
  };

  class GetGenerator
  {
  public:
    class Apply
    {
    public:
      using Type = Detail::Mpl::ApplyT<typename BasicP::GetGenerator, NodeTagP>;
    };
  };
};

} // namespace Basic

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
