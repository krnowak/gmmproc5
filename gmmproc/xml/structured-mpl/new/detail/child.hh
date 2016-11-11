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

// TODO: should be exposed outside the Detail namespace
class SimpleGetter
{
public:
  template <NodeP>
  NodeP const&
  get () const;
};

template <typename NodeP, typename StorageP>
class SingleChild : public virtual SimpleGetter, public virtual StorageP
{
private:
public:
  template<>
  SomeNode const&
  get<SomeNode> () const
  {
    return std::get<SomeIndex> (storage);
  }
};

class GetChildImpl
{
public:
  template <typename BasicChildP, typename StorageP>
  class Apply
  {
  private:
    using Basic = GetBasicT<BasicChildP>;
    using NodeTag = GetNodeTagT<BasicChildP>;

  public:
    using Type = typename Basic::GetImpl::Apply<NodeTag, StorageP>::Type;
  };
};

class GetExclusiveImpl
{
public:
  template <typename ExclusiveP, typename StorageP>
  class Apply
  {

  };
};

class GetOptExclusiveImpl
{
public:
  template <typename OptExclusiveP, typename StorageP>
  class Apply
  {

  };
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_CHILD_HH
