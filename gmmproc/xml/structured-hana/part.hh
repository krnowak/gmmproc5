#ifndef GMMPROC_XML_STRUCTURED_PART_HH
#define GMMPROC_XML_STRUCTURED_PART_HH

namespace Gmmproc::Xml::Structured
{

enum class PartKind
{
  // its impl is constructed with the structured tag and inputs
  Normal,
  // its impl is default constructed
  Extra,
  // its impl is not a part of a storage
  NonMember
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
