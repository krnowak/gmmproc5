#include "long.hh"
#include "names.hh"
#include "short.hh"
#include "types.hh"

#include <pugixml.hpp>

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

/* <lameschema>
 *   <names>
 *     <name name="name"/>
 *     ...
 *   </names>
 *   <short>
 *     <element name="name">
 *       <attributes>
 *         <attribute name="name" type="string|int|bool-f|bool-t" />
 *       </attributes>
 *       <children>
 *         <element name="name" type="optional-opt|optional-ptr|single|vector|map|vector-map" />
 *       </children>
 *     </element>
 *     ...
 *   </short>
 *   <long>
 *     <element name="name" type="optional-opt|optional-ptr|single|vector|map|vector-map">
 *       <attributes>
 *         <attribute name="name" type="string|int|bool-f|bool-t" />
 *       </attributes>
 *       <children>
 *         <element name="name" type="optional-opt|optional-ptr|single|vector|map|vector-map">
 *           ...
 *         </element>
 *       </children>
 *     </node>
 *   </long
 * </lameschema>
 */

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

struct SchemaData
{
  Names n;
  Short s;
  Long l;

  void
  process_node (pugi::xml_node const& node, int depth)
  {
    n.process_node (node, depth);
    s.process_node (node, depth);
    l.process_node (node, depth);
  }

  void
  process_document (pugi::xml_node const& document)
  {
    n.process_document (document);
    s.process_document (document);
    l.process_document (document);
  }
};

class GirWalker : public pugi::xml_tree_walker
{
public:
  SchemaData&&
  steal ()
  {
    return std::move (data);
  }

private:
  bool
  for_each (pugi::xml_node& node) override
  {
    switch (node.type ())
    {
    case pugi::node_element:
      data.process_node (node, depth());
      break;

    case pugi::node_document:
      data.process_document (node);
      break;

    default:
      break;
    }
    return true;
  }

  SchemaData data;
};

class LameSchemaBuilder
{
public:
  std::unique_ptr<pugi::xml_document>
  build_doc (SchemaData const&)
  {
    // TODO
    return {};
  }
};

struct GirPaths
{
  GirPaths() = default;
  GirPaths(Str&& d)
    : dir {std::move (d)}
  {}

  Str dir;
  StrQueue files;
};

GirPaths
get_gir_paths(char **argv)
{
  if (!argv[1])
  {
    throw std::runtime_error ("expected a path to girdir and at least one gir filename");
  }
  GirPaths paths {argv[1]};

  for (auto ptr = &argv[2]; *ptr; ++ptr)
  {
    paths.files.push (paths.dir + '/' + Str{*ptr});
  }
  if (paths.files.empty ())
  {
    throw std::runtime_error ("got a path to girdir, but no gir filenames");
  }
  return paths;
}

SchemaData
parse_gir_files (GirPaths& paths)
{
  StrSet parsed;
  GirWalker walker;

  while (!paths.files.empty ())
  {
    auto const path = paths.files.front ();

    paths.files.pop ();

    if (parsed.find (path) != parsed.end ())
    {
      continue;
    }
    pugi::xml_document doc;
    auto result = doc.load_file (path.c_str (), pugi::parse_default, pugi::encoding_utf8);

    if (!result)
    {
      std::ostringstream oss;

      oss << "failed to parse " << path << ": " << result.description ();
      throw std::runtime_error {oss.str ()};
    }

    auto const repo = doc.child ("repository");

    for (auto const& include : repo.children ("include"))
    {
      auto const inc_name = std::string {include.attribute ("name").value ()};
      auto const inc_version = std::string {include.attribute ("version").value ()};
      auto const inc_filename = inc_name + "-" + inc_version + ".gir";
      auto const inc_full_path = paths.dir + "/" + inc_filename;

      paths.files.push (inc_full_path);
    }

    doc.traverse (walker);
    parsed.insert (path);
  }

  return walker.steal ();
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

int
main(int, char **argv)
{
  using namespace Mm::Gir::Tools::LameSchemaGen;

  try
  {
    auto gir_paths = get_gir_paths (argv);
    auto data = parse_gir_files (gir_paths);
    LameSchemaBuilder builder;
    auto doc = builder.build_doc (data);

    if (doc)
    {
      doc->save (std::cout, "  ");
    }
  }
  catch (std::exception const& ex)
  {
    std::cerr << argv[0] << ": " << ex.what () << std::endl;
    return 1;
  }
}
