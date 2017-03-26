#ifndef GMMPROC_XML_STRUCTURED_PART_HH
#define GMMPROC_XML_STRUCTURED_PART_HH

namespace Gmmproc::Xml::Structured
{

enum class PartKind
{
  // its impl is constructed with the node parameter
  Normal,
  // its impl is default constructed
  Extra
};

template <typename TagP>
class Part
{
public:
  using Tag = TagP;

  Tag tag;
};

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_PART_HH
