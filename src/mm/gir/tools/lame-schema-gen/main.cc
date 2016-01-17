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
 *         <attribute name="name" type="string|int|bool-f|bool-t" unique="0|1"/>
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
 *         <attribute name="name" type="string|int|bool-f|bool-t" unique="0|1"/>
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

namespace
{

void
print_exception (std::exception const& top_ex)
{
  std::queue<std::reference_wrapper<std::exception const> > exceptions;
  std::size_t level = 0;

  exceptions.push (top_ex);
  while (!exceptions.empty ())
  {
    auto const& ex = exceptions.front ().get ();

    exceptions.pop ();
    ++level;
    std::cerr << std::string (level, ' ') << ex.what () << "\n";
    try
    {
      std::rethrow_if_nested (ex);
    }
    catch (std::exception const& nested_ex)
    {
      exceptions.push (nested_ex);
    }
  }
}

} // anonymous namespace

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
    std::cerr << argv[0] << ": caught an exception, description follows\n";
    print_exception (ex);
    return 1;
  }
}
