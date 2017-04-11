#ifndef GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

#include <boost/hana/ext/std/integer_sequence.hpp>

#include <utility>

// TODO: move to detail, merge to detail/storage.hh?
namespace Gmmproc::Xml::Structured::Hana
{

template <typename TypeP,
          typename IndexP,
          typename StructuredTagP>
class StorageMemberParameter
{
public:
  using Type = TypeP;
  using Index = IndexP;
  using StructuredTag = StructuredTagP;
};

template <typename SMParameterP>
class StorageMember
{
private:
  using Type = typename SMParameterP::Type;
  using Index = typename SMParameterP::Index;
  using StructuredTag = typename SMParameterP::StructuredTag;

protected:
  template <typename... InputP>
  StorageMember(InputP... input)
    : member {generate_member (StructuredTag{}, input...)}
  {}

  Type member;
};

template <typename... SMParameterP>
class Storage : private StorageMember<SMParameterP>...
{
private:
  using ThisStorage = Storage<SMParameterP...>;
  using Members = hana::tuple<hana::type<StorageMember<SMParameterP>...>>;

  static constexpr Members members;

public:
  template <typename IndexP>
  decltype(auto)
  operator[] (IndexP index) const
  {
    constexpr auto member_type {ThisStorage::members[index]};
    using MemberType = decltype(member_type)::type;
    auto this_as_member {static_cast<MemberType const*> (this)};

    return this_as_member->member;
  }

  template <typename IndexP>
    decltype(auto)
    operator[] (IndexP index)
  {
    constexpr auto member_type {ThisStorage::members[index]};
    using MemberType = decltype(member_type)::type;
    auto this_as_member {static_cast<MemberType*> (this)};

    return this_as_member->member;
  }
};

template <typename... SMParameterP>
get_the_type_finally (SMParameterP...)
{
  return hana::type_c<Storage<SMParameter...>>;
}

template <typename StructuredTagP,
          typename ContainedTypesP>
constexpr auto
get_storage_type (StructuredTagP structured_tag,
                  ContainedTypesP contained_types)
{
  constexpr auto zipper = [structured_tag](auto type,
                                           auto index)
  {
    return StorageMemberParameter {type, index, structured_tag};
  }
  auto sm_parameters = hana::zip_with (zipper,
                                       contained_types,
                                       std::make_index_sequence (sizeof... (TypeP)));
  return hana::unpack (sm_parameters,
                       [](auto... sm_parameter)
                       {
                         return NoADL::get_the_type_finally (sm_parameter...);
                       });
}

} // namespace Gmmproc::Xml::Structured::Hana

#endif // GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
