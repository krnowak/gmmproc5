#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH

#include <gmmproc/gir/tools/lame-schema-gen/types.hh>

namespace Gmmproc
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

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_GIR_PATHS_HH
