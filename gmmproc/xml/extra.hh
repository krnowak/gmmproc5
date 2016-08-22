#ifndef GMMPROC_XML_EXTRA_HH
#define GMMPROC_XML_EXTRA_HH

#include "xmlfwd.hh"

namespace Gmmproc
{

namespace Xml
{

namespace Extra
{

template <typename ImplP>
inline TextTmplView<ImplP, Utils::ViewType::Mutable>
add_text (NodeTmplView<ImplP, Utils::ViewType::Mutable>& view,
          Type::StringView text)
{
  auto text_view = view->insert_text_at (view->all ().end ());

  text_view->set_text (text);

  return text_view;
}

inline TextView
add_text (NodeView& view,
          Type::StringView text)
{
  return add_text<XmlImpl> (view, std::move (text));
}

template <typename ImplP>
inline NodeTmplView<ImplP, Utils::ViewType::Mutable>
add_child (NodeTmplView<ImplP, Utils::ViewType::Mutable>& view,
           Type::StringView name)
{
  return view->insert_child_at (view->all ().end (), name);
}

inline NodeView
add_child (NodeView& view,
           Type::StringView name)
{
  return add_child<XmlImpl> (view, std::move (name));
}

template <typename ImplP>
inline AttributeTmplView<ImplP, Utils::ViewType::Mutable>
add_attribute (NodeTmplView<ImplP, Utils::ViewType::Mutable>& view,
               Type::StringView name,
               Type::StringView value)
{
  auto attribute = view->insert_attribute_at (view->attributes ().end (), name);

  attribute->set_value (value);

  return attribute;
}


inline AttributeView
add_attribute (NodeView& view,
               Type::StringView name,
               Type::StringView value)
{
  return add_attribute<XmlImpl> (view, std::move (name), std::move (value));
}

} // namespace Extra

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_EXTRA_HH
