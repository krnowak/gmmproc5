#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH

#include <gmmproc/gir/tools/lame-schema-gen/types.hh>

#include <gmmproc/xml/xml.hh>

#include <iosfwd>

namespace Gmmproc
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

  Xml::Document doc;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DOC_HH
