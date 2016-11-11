#ifndef GMMPROC_XML_STRUCTURED_GENERATORS_HH
#define GMMPROC_XML_STRUCTURED_GENERATORS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>

#include <gmmproc/xml/xml.hh>

#include <gmmproc/utils/iterator.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Generators
{

template <typename PartP>
class SingleGenerator
{
private:
  using Type = Detail::Mpl::ApplyT<typename PartP::GetContainedType>;
  using Name = Detail::Mpl::ApplyT<typename PartP::GetName>;

  class Filter
  {
  public:
    bool
    operator() (Xml::NodeView& child) const
    {
      return child->name () == Name::as_view ();
    }
  };

public:
  static Type
  get (Xml::NodeView& node)
  {
    auto const pred = Filter{};
    auto only_child = std::experimental::make_optional<Type> ();
    for (auto& child : Utils::make_filter_range (Filter{}, node->children ()))
    {
      if (only_child)
      {
        throw std::runtime_error ("TODO: expected only one child, got more");
      }
      only_child = child;
    }
    if (!only_child)
    {
      throw std::runtime_error ("TODO: expected only one child, got none");
    }

    return Type {*only_child};
  }
};

} // namespace Generators

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_GENERATORS_HH
