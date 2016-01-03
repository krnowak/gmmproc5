#include "gir-paths.hh"

#include <stdexcept>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

GirPaths::GirPaths (Str&& d)
  : dir {std::move (d)}
{}

// static
GirPaths
GirPaths::get_from_argv (char** argv)
{
  if (!argv[1])
  {
    throw std::runtime_error ("expected a path to girdir and at least one gir filename");
  }
  if (!argv[2])
  {
    throw std::runtime_error ("got a path to girdir, but no gir filenames");
  }

  GirPaths paths {argv[1]};
  for (auto ptr = &argv[2]; *ptr; ++ptr)
  {
    paths.files.push (paths.dir + '/' + Str{*ptr});
  }

  return paths;
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
