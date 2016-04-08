#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH

#include "types.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class GirPaths
{
public:
  static GirPaths
  get_from_argv (char** argv);

  Str dir;
  StrQueue files;

private:
  GirPaths (Str&& d);
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH
