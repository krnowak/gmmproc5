#include "names.hh"

#include <utility>

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

void
maybe_emplace (StrSet& names,
               Str&& name)
{
  if (!name.empty ())
  {
    names.emplace (std::move (name));
  }
}

} // anonymous namespace

void
Names::process_node_vfunc (Xml::Base::Node const& node,
                           int)
{
  maybe_emplace (names, node.name ());

  for (auto attr : node.attributes ())
  {
    maybe_emplace (names, attr.name ());
  }
}

void
Names::postprocess_node_vfunc (Xml::Base::Node const&,
                               int)
{}

void
Names::wrap_up_vfunc ()
{}

StrSet&&
Names::steal ()
{
  return std::move (names);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
