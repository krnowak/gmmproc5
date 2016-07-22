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

  class GetAccessInfo
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = ...;
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
