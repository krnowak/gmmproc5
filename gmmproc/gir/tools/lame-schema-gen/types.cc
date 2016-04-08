#include "types.hh"

#include <algorithm>
#include <utility>

#include <cctype>

namespace Gmmproc
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Attribute::Type
{
public:
  using TypePtr = std::unique_ptr<Type>;
  virtual ~Type () = default;

  virtual TypePtr
  maybe_create_new (Str const& value) = 0;

  virtual Str
  to_string () const = 0;
};

namespace
{

class UndeterminedType : public Attribute::Type
{
  virtual TypePtr
  maybe_create_new (Str const& value) override;

  virtual Str
  to_string () const override;
};

class BoolType : public Attribute::Type
{
public:
  BoolType (bool explicit_value);

  virtual TypePtr
  maybe_create_new (Str const& value) override;

  virtual Str
  to_string () const override;

private:
  TypePtr
  update_with_value (bool explicit_value);

  bool implicit_value;
  bool mixed_implicit_value = false;
};

class NumericType : public Attribute::Type
{
  virtual TypePtr
  maybe_create_new (Str const& value) override;

  virtual Str
  to_string () const override;
};

class StringType : public Attribute::Type
{
  virtual TypePtr
  maybe_create_new (Str const&) override;

  virtual Str
  to_string () const override;
};

// prereq: first != last
template <typename Iterator>
bool
is_numeric_range (Iterator first, Iterator const &last)
{
  if (*first == '-')
  {
    ++first;
  }
  if (first == last)
  {
    return false;
  }
  return std::find_if (first, last, [](auto c) { return !std::isdigit (c); }) == last;
}

std::pair<bool, bool>
is_string_bool_value (Str const& value)
{
  if ((value == "t") || (value == "y") || (value == "true") || (value == "yes"))
  {
    return std::make_pair (true, true);
  }
  if ((value == "f") || (value == "n") || (value == "false") || (value == "no"))
  {
    return std::make_pair (true, false);
  }
  return std::make_pair (false, false);
}

std::pair<bool, bool>
is_numeric_bool_value (Str const& value)
{
  if (value == "1")
  {
    return std::make_pair (true, true);
  }
  if (value == "0")
  {
    return std::make_pair (true, false);
  }
  return std::make_pair (false, false);
}

Attribute::Type::TypePtr
UndeterminedType::maybe_create_new (Str const& value)
{
  if (value.empty ())
  {
    return TypePtr {std::make_unique<BoolType> (true)};
  }
  if (is_numeric_range (value.cbegin (), value.cend ()))
  {
    auto const bool_desc = is_numeric_bool_value (value);

    if (bool_desc.first)
    {
      return TypePtr {std::make_unique<BoolType> (bool_desc.second)};
    }
    return TypePtr {std::make_unique<NumericType> ()};
  }

  auto const bool_desc = is_string_bool_value (value);

  if (bool_desc.first)
  {
    return TypePtr {std::make_unique<BoolType> (bool_desc.second)};
  }
  return TypePtr {std::make_unique<StringType> ()};
}

Str
UndeterminedType::to_string () const
{
  return "undetermined";
}

BoolType::BoolType (bool explicit_value)
  : implicit_value {!explicit_value}
{}

Attribute::Type::TypePtr
BoolType::maybe_create_new (Str const& value)
{
  if (value.empty ())
  {
    return update_with_value (true);
  }
  if (is_numeric_range (value.cbegin (), value.cend ()))
  {
    auto const bool_desc = is_numeric_bool_value (value);

    if (bool_desc.first)
    {
      return update_with_value (bool_desc.second);
    }
    return TypePtr {std::make_unique<NumericType> ()};
  }

  auto const bool_desc = is_string_bool_value (value);

  if (bool_desc.first)
  {
    return update_with_value (bool_desc.second);
  }
  return TypePtr {std::make_unique<StringType> ()};
}

Str
BoolType::to_string () const
{
  if (mixed_implicit_value)
  {
    return "bool";
  }
  if (implicit_value)
  {
    return "bool-t";
  }
  return "bool-f";
}

Attribute::Type::TypePtr
BoolType::update_with_value (bool explicit_value)
{
  if (!mixed_implicit_value && (implicit_value == explicit_value))
  {
    mixed_implicit_value = true;
  }
  return TypePtr {};
}

Attribute::Type::TypePtr
NumericType::maybe_create_new (Str const& value)
{
  if (value.empty ())
  {
    return TypePtr {std::make_unique<StringType> ()};
  }
  if (!is_numeric_range (value.cbegin (), value.cend ()))
  {
    return TypePtr {std::make_unique<StringType> ()};
  }
  return nullptr;
}

Str
NumericType::to_string () const
{
  return "int";
}

Attribute::Type::TypePtr
StringType::maybe_create_new (Str const&)
{
  return TypePtr {};
}

Str
StringType::to_string () const
{
  return "string";
}

} // anonymous namespace

Attribute::Attribute (Str const& name)
  : Named {name},
    impl {std::make_unique<UndeterminedType> ()},
    unique {true}
{}

Attribute::Attribute (Attribute&&) = default;

Attribute::~Attribute () = default;

Attribute&
Attribute::operator= (Attribute&&) = default;

void
Attribute::update (Str const& value)
{
  auto new_type = impl->maybe_create_new (value);

  if (new_type)
  {
    impl.swap (new_type);
  }
}

Str
Attribute::to_string () const
{
  return impl->to_string ();
}

bool
Attribute::is_unique () const
{
  return unique;
}

void
Attribute::mark_as_common ()
{
  unique = false;
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc
