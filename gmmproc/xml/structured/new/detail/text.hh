#ifndef GMMPROC_XML_STRUCTURED_DETAIL_TEXT_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_TEXT_HH

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename ContainedTypeP>
class TextBase
{
public:
  class GetContainedType
  {
  public:
    class Apply
    {
    public:
      using Type = ContainedTypeP;
    };
  };

  class GetGetters
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = Detail::GetterWithDefaultPolicy<Getters::TextGetter<StorageP, Mpl::ApplyT<GetAccessKey>>>;
    };
  };

  class GetAccessKey
  {
    class Apply
    {
    public:
      class Type {};
    };
  };

  // TODO: process
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TEXT_HH
