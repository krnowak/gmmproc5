#ifndef GMMPROC_XML_STRUCTURED_DETAIL_CHILD_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_CHILD_HH

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename NodeTagP>
class SingleGenerator
{
private:
  using NodeInfo = typename Registry::NodeInfo<NodeTagP>;
  using Type = typename NodeInfo<NodeTagP>::Type;
  using Name = typename NodeInfo<NodeTagP>::Name;

public:
  static RetType
  get (Xml::Node& node)
  {
    
  };
};

class GetGroupAccessInfo
{
public:
  template <typename StorageP, typename... ChildP>
  class Apply
  {
  private:
    using AccessInfoVector = Mpl::Vector<Mpl::ApplyT<typename ChildP::GetAccessInfo, StorageP>...>;

  public:
    using Type = Mpl::FoldT<AccessInfoVector, Mpl::Vector<>, Flatten>;
  };
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_CHILD_HH
