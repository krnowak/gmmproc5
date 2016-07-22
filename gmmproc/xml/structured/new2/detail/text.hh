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

class TextGenerator
{
public:
  static auto
  generate (Xml::Node& node)
  {
    return node.get_text ();
  }
};

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

  class GetAccessInfo
  {
    template <typename StorageP>
    class Apply
    {
    private:
      using AccessKey = TODO;
      using GettersMap = Detail::GetterWithDefaultPolicy<Getters::TextGetter<StorageP>>;

    public:
      using Type = Mpl::Vector<Mpl::Pair<AccessKey, GettersMap>>;
    };
  };

  class GetGenerator
  {
  public:
    class Apply
    {
    public:
      using Type = TextGenerator;
    }
  };
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TEXT_HH
