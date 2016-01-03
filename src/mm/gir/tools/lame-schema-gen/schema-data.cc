#include "gir-paths.hh"
#include "long.hh"
#include "names.hh"
#include "schema-data.hh"
#include "short.hh"

#include <pugixml.hpp>

#include <memory>
#include <sstream>
#include <stdexcept>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace
{

class GirWalker final : public Xml::Base::Walker
{
public:
  void
  fill_data (SchemaData& data)
  {
    data.names = n.steal ();
    data.short_data = s.steal ();
    data.toplevel_long_node = l.steal ();
  }

private:
  virtual bool
  node (Xml::Base::Node& node, int depth) override
  {
    n.process_node (node, depth);
    s.process_node (node, depth);
    l.process_node (node, depth);
    return true;
  }

  virtual bool
  postprocess_node (Xml::Base::Node& node, int depth) override
  {
    n.postprocess_node (node, depth);
    s.postprocess_node (node, depth);
    l.postprocess_node (node, depth);
    return true;
  }

  Names n;
  Short s;
  Long l;
};

} // anonymous namespace

// static
SchemaData
SchemaData::get_from_gir_paths (GirPaths const& paths)
{
  StrSet parsed;
  GirWalker walker;
  auto files = paths.files;

  while (!files.empty ())
  {
    auto const& path = files.front ();
    files.pop ();
    if (parsed.find (path) != parsed.end ())
    {
      continue;
    }

    Xml::Base::Document doc (path);
    auto const repo = doc.as_node ().child ("repository");
    if (!repo)
    {
      throw std::runtime_error ("malformed gir file - no toplevel repository tag");
    }

    auto const include_child = repo->child ("include");
    if (include_child)
    {
      for (auto const& include : include_child->siblings ("include"))
      {
        auto const name_attr = include.attribute ("name");
        auto const version_attr = include.attribute ("version");
        if (!name_attr || !version_attr)
        {
          throw std::runtime_error ("malformed gir file - include tag has either no name tag or version tag");
        }
        auto const inc_filename = name_attr->value () + "-" + version_attr->value () + ".gir";
        auto const inc_full_path = paths.dir + "/" + inc_filename;

        files.push (inc_full_path);
      }
    }

    walker.walk (doc);
    parsed.insert (path);
  }

  SchemaData data;
  walker.fill_data (data);
  return data;
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
