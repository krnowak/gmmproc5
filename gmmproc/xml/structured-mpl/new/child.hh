#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>

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
      using Type = typename Registry::NodeInfo<NodeTagP>::Type;
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
      using Type = TODO;
    }
  };
};

// TODO: rename to OptionalKind?
enum class OptionalType : int
{
  Pointer,
  Value
};

// Optional<Node>& get<Node>()
template <typename OptionalTypeP> // ptr or val
class Optional
{
public:
  class GetContainedType
  {
  public:
    template <typename NodeTagP>
    class Apply
    {
    private:
      using NodeType = typename Registry::NodeInfo<NodeTagP>::Type;

    public:
      using Type = Mpl::ApplyT<Detail::GetOptionalType, NodeType, OptionalTypeP>;
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
      using Type = TODO;
    }
  };
};

// Node& get<Node>(std::size_t idx)
// std::size_t count<Node>()
// Range<Node> range<Node>()
class Vector
{
public:
  class GetContainedType
  {
  public:
    template <typename NodeTagP>
    class Apply
    {
    private:
      using NodeType = typename Registry::NodeInfo<NodeTagP>::Type;

    public:
      using Type = std::vector<NodeType>;
    };
  };

  class GetGetters
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Detail::GetterWithDefaultPolicy<Getters::VectorGetter<StorageP>>;
    };
  };

  template <typename NodeTagP>
  class GetGenerator
  {
  public:
    class Apply
    {
      using Type = TODO;
    }
  };
};

// Node& get<Node>(std::size_t idx)
// std::size_t count<Node>()
// Range<Node> range<Node>()
// Node& get<Node, AttributeKey>(std::string const& name)
template <typename AttrKeysP> // mpl::vector<StaticString<"name">, StaticString<"c:type">>
class MapVector
{
public:
  class GetContainedType
  {
  public:
    template <typename NodeTagP>
    class Apply
    {
    public:
      using Type = Mpl::ApplyT<Detail::GetMapVectorTupleType, AttrKeysP, NodeTagP>;
    };
  };

  class GetGetters
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Mpl::Map<Mpl::Pair<Getters::VectorGetter<StorageP>,
                                      Detail::MapVectorVectorPolicy>,
                            Mpl::Pair<Getters::MapGetter<StorageP>,
                                      Detail::MapVectorMapPolicy<AttrKeysP>>>;
    };
  };

  template <typename NodeTagP>
  class GetGenerator
  {
  public:
    class Apply
    {
      using Type = TODO;
    }
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
      using Type = Mpl::ApplyT<typename BasicP::GetContainedType, NodeTagP>;
    };
  };

  class GetGetters
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Mpl::ApplyT<typename BasicP::GetGetters, StorageP>;
    };
  };

  class GetAccessKey
  {
    class Apply
    {
    public:
      using Type = typename Registry::NodeInfo<NodeTagP>::Type;
    };
  };

  class GetGenerator
  {
  public:
    class Apply
    {
    public:
      using Type = Mpl::ApplyT<typename BasicP::GetGenerator, NodeTagP>;
    };
  };
};

} // namespace Basic

namespace Ext
{

// Exclusive& get<ExclusiveKey>()
template <typename ExclusiveKeyP, typename BasicAndNodeTagPairListP> // mpl::vector<mpl::pair<BasicP, NodeTagP>, ...>
class Exclusive
{
public:
  class GetContainedType
  {
  public:
    class Apply
    {
    public:
      using Type = boost::make_variant_over<Detail::GetContainedTypesT<BasicAndNodeTagPairListP>>;
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

  class GetAccessKey
  {
    class Apply
    {
    public:
      using Type = ExclusiveKeyP;
    };
  };

  template <typename NodeTagP>
  class GetGenerator
  {
  public:
    class Apply
    {
      using Type = TODO;
    }
  };
};

// Optional<Exclusive>& get<ExclusiveKey>()
template <typename ExclusiveKeyP, typename OptionalTypeP, typename TypesAndTagsP>
class OptExclusive
{
public:
  class GetContainedType
  {
  public:
    class Apply
    {
    private:
      using Variant = boost::make_variant_over<Detail::GetContainedTypesT<BasicAndNodeTagPairListP>>;

    public:
      using Type = Mpl::ApplyT<Detail::GetOptionalType, Variant, OptionalTypeP>;
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

  class GetAccessKey
  {
    class Apply
    {
    public:
      using Type = ExclusiveKeyP;
    };
  };

  template <typename NodeTagP>
  class GetGenerator
  {
  public:
    class Apply
    {
      using Type = TODO;
    }
  };
};

} // namespace Ext

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
