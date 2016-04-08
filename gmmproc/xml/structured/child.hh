#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Basic
{

class Single
{
  // TODO
};

template <typename OptionalTypeP> // ptr or val
class Optional
{
  // TODO
};

class Vector
{
  // TODO
};

template <typename AttrKeysP> // mpl::vector<StaticString<"name">, StaticString<"c:type">>
class MapVector
{
  // TODO
};

template <typename TypeP, typename TagP> // TypeP - single, optional, vector, mapvector
class Child
{
  // TODO
};

} // namespace Basic

namespace Ext
{

template <typename AliasP, typename TypesAndTagsP> // mpl::vector<std::pair<TypeP, TagP>, ...>
class Exclusive
{
  // TODO
};

template <typename AliasP, typename OptionalTypeP, typename TypesAndTagsP>
class OptExclusive
{
  // TODO
};

} // namespace Ext

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
