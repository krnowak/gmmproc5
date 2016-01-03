#include "gir-paths.hh"
#include "schema-data.hh"
#include "schema-doc.hh"

#include <exception>
#include <iostream>

/* <lameschema>
 *   <names>
 *     <name name="name"/>
 *     ...
 *   </names>
 *   <short>
 *     <element name="name">
 *       <attributes>
 *         <attribute name="name" type="string|int|bool-f|bool-t" />
 *         ...
 *       </attributes>
 *       <children>
 *         <element name="name" type="optional|single|vector|map|vector-map" extra-type-info="opt or ptr for optional type, attribute name for map or vector-map, nothing for vector or single"/>
 *         ...
 *       </children>
 *     </element>
 *     ...
 *   </short>
 *   <long>
 *     <element name="name" type="optional|single|vector|map|vector-map">
 *       <attributes>
 *         <attribute name="name" type="string|int|bool-f|bool-t" />
 *         ...
 *       </attributes>
 *       <children>
 *         <element name="name" type="optional|single|vector|map|vector-map">
 *           ...
 *         </element>
 *         ...
 *       </children>
 *     </element>
 *   </long>
 * </lameschema>
 */

int
main (int, char **argv)
{
  using namespace Mm::Gir::Tools::LameSchemaGen;

  try
  {
    auto paths = GirPaths::get_from_argv (argv);
    auto data = SchemaData::get_from_gir_paths (paths);
    auto doc = SchemaDoc::get_from_schema_data (data);

    doc.save (std::cout);
  }
  catch (std::exception const& ex)
  {
    std::cerr << argv[0] << ": " << ex.what () << std::endl;
    return 1;
  }
}
