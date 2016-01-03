#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_COMMON_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_COMMON_HH

#include "types.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

AttributeType
update_attribute_type (Str const& value, AttributeType const& previous_type);

LeafType
update_leaf_type (Xml::Base::Node const& node, LeafType const& previous_type);

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_COMMON_HH
