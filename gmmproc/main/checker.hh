#pragma once

// TODO: maybe just namespace pugi { class xml_node; }
#include <pugixml.hpp>

#include "common.hh"

class Checker
{
public:
  Checker(AttributesOnly, SVector const& attrs);
  Checker(AttributesOnly, SVector&& attrs);
  Checker(ChildrenOnly, SVector const& children);
  Checker(ChildrenOnly, SVector&& children);
  Checker(SVector const& attrs, SVector const& children);
  Checker(SVector&& attrs, SVector&& children);

  void
  process_node (pugi::xml_node const& node) const;

private:
  SVector m_attrs;
  SVector m_children;
};
