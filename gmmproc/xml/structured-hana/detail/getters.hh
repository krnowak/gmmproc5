#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/getters.hh>

#include <tuple>
#include <type_traits>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename GetterP>
constexpr auto
get_getters_map_for_getter_with_default_policy (hana::type<GetterP> getter)
{
  return hana::make_map (hana::make_pair (getter, get_default_policy ()));
}

template <typename GetterP>
using GetterWithDefaultPolicy = Mpl::Map<Mpl::Pair<GetterP, typename GetterP::DefaultPolicy>>;

template <typename StorageP>
using SingleGetterWithDefaultPolicy = GetterWithDefaultPolicy<Getters::SingleGetter<StorageP>>;

template <typename... PartP>
class Getters
{
private:
  using StorageType = Storage<PartP...>;

  class GetGettersFromAccessInfoEntry
  {
  public:
    template <typename SequenceP, typename AccessInfoEntryP>
    class Apply
    {
    private:
      using GettersMap = Mpl::SecondT<AccessInfoEntryP>;
      using GettersVector = Mpl::TransformT<GettersMap, typename boost::mpl::lambda<Mpl::First<Mpl::_1>>::type, Mpl::VectorBackInserter>;

    public:
      using Type = Mpl::InsertRangeT<SequenceP, Mpl::EndT<SequenceP>, GettersVector>;
    };
  };

  class GetUniqueGetters
  {
  public:
    template <typename SetP, typename OnePartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename OnePartP::GetAccessInfo,
                                     StorageType>;
      using GettersVector = Mpl::FoldT<AccessInfo,
                                       Mpl::Vector<>,
                                       GetGettersFromAccessInfoEntry>;

    public:
      using Type = Mpl::InsertRangeT<SetP, Mpl::BeginT<SetP>, GettersVector>;
    };
  };

  using UniqueGetters = Mpl::FoldT<Mpl::Vector<PartP...>,
                                   Mpl::Set<>,
                                   GetUniqueGetters>;
  using GettersSubclass = Mpl::InheritSequenceT<UniqueGetters>;

public:
  using Type = GettersSubclass;
};

template <typename SetP>
constexpr auto
to_tuple (SetP set)
{
  return hana::unpack (set, [](auto... type) {
      return hana::make_tuple (type...);
    });
}

template <typename... TypeP>
class Subclass : public TypeP...
{};

template <typename... TypeP>
constexpr auto
get_subclass (hana::tuple<hana::type<TypeP>...>)
{
  return hana::type<Subclass<TypeP...>>;
}

// access_info_map = map<access_key, map<getter, policy>>

template <typename ResolvedStorageTagP>
constexpr auto
get_getters_type (ResolvedStorageTagP resolved)
{
  auto unique_getters = hana::fold_left (resolved.parts,
                                         hana::make_set (),
                                         [resolved](auto set, auto part)
                                         {
                                           auto access_info_map = get_access_info (part, resolved);
                                           auto getters = hana::fold_left (access_info_map,
                                                                           hana::make_tuple (),
                                                                           [](auto getters, auto pair)
                                                                           {
                                                                             auto getters_and_policies = hana::second(pair);
                                                                             return hana::insert_range (getters, 0_c, getters_and_policies.keys ());
                                                                           });
                                           return hana::insert_range (set, getters);
                                         });

  return get_subclass(to_tuple (unique_getters));
}

template <typename ResolvedStorageTagP>
using GettersTypeT = decltype(get_getters_type (std::declval<ResolvedStorageTagP>()))::type;

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
