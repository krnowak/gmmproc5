#include "gir-paths.hh"
#include "names.hh"
#include "schema-data.hh"
#include "short.hh"

#include <mm/xml/xml.hh>

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

class GirWalker final : public Xml::Walker
{
public:
  void
  fill_data (SchemaData& data)
  {
    n.wrap_up ();
    s.wrap_up ();
    data.names = n.steal ();
    data.short_data = s.steal ();
  }

private:
  virtual bool
  node (Xml::Node& node,
        int depth) override
  {
    n.process_node (node, depth);
    s.process_node (node, depth);
    return true;
  }

  virtual bool
  postprocess_node (Xml::Node& node,
                    int depth) override
  {
    n.postprocess_node (node, depth);
    s.postprocess_node (node, depth);
    return true;
  }

  Names n;
  Short s;
};

StrVector
parse_doc_in_path (GirWalker& walker, std::string const& dir, std::string const& path)
{
  try
  {
    StrVector files;
    Xml::Document doc {path};
    auto const repo = doc.root_tag ();

    if (!repo)
    {
      throw std::runtime_error ("malformed gir file - no toplevel repository tag");
    }
    if (repo->name () != "repository")
    {
      throw std::runtime_error ("malformed gir file - toplevel tag is not named repository");
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
        auto const inc_full_path = dir + "/" + inc_filename;

        files.push_back (inc_full_path);
      }
    }
    walker.walk (doc);
    return files;
  }
  catch (std::exception&)
  {
    std::ostringstream oss;

    oss << "failed to parse file " << path;
    std::throw_with_nested (std::runtime_error {oss.str ()});
  }
}

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
    auto const path = files.front ();

    files.pop ();
    if (parsed.find (path) != parsed.end ())
    {
      continue;
    }

    auto new_files = parse_doc_in_path (walker, paths.dir, path);
    for (auto& path : new_files)
    {
      files.push (std::move (path));
    }
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
