#include "checker.hh"
#include "exceptions.hh"

Checker::Checker(AttributesOnly, SVector const& attrs)
  : m_attrs{attrs},
    m_children{}
  {}

Checker::Checker(AttributesOnly, SVector&& attrs)
  : m_attrs{std::move (attrs)},
    m_children{}
  {}

Checker::Checker(ChildrenOnly, SVector const& children)
  : m_attrs{},
    m_children{children}
  {}

Checker::Checker(ChildrenOnly, SVector&& children)
  : m_attrs{},
    m_children{std::move (children)}
  {}

Checker::Checker(SVector const& attrs, SVector const& children)
  : m_attrs{attrs},
    m_children{children}
  {}

Checker::Checker(SVector&& attrs, SVector&& children)
  : m_attrs{std::move (attrs)},
    m_children{std::move (children)}
  {}

void
Checker::process_node (pugi::xml_node const& node) const
{
  for (auto const& attr : m_attrs)
  {
    if (node.attribute (attr.c_str ()))
    {
      throw InvalidParserAssumption {AttributesOnly, node, attr};
    }
  }
  for (auto const& child : m_children)
  {
    if (node.child (child.c_str()))
    {
      throw InvalidParserAssumption {ChildrenOnly, node, child};
    }
  }
}
