#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH

#include "types.hh"

#include <iosfwd>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class SchemaData;

class SchemaDoc
{
public:
  static SchemaDoc
  get_from_schema_data (SchemaData const& data);

  void
  save (std::ostream& os);

private:
  SchemaDoc () = default;

  Xml::Base::Document doc;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH
