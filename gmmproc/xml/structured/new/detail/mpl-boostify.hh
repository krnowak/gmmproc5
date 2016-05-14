#ifndef GMMPROC_XML_STRUCTURED_DETAIL_MPL_BOOSTIFY_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_MPL_BOOSTIFY_HH

#include <type_traits>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

namespace Mpl
{

namespace Test
{


template <typename TypeP>
class T2T
{
public:
  using Type = TypeP;
};


class MF
{
public:
  class Apply
  {
  public:
    using Type = T2T<int>;
  };
};

class MF1
{
public:
  template <typename T1P>
  class Apply
  {
  public:
    using Type = T2T<T1P>;
  };
};

class MF2
{
public:
  template <typename T1P, typename T2P>
  class Apply
  {
  public:
    using Type = boost::mpl::vector< T2T<T1P>, T2T<T2P> >;
  };
};

class TF
{
public:
  using Type = T2T<int>;
};

class BMF
{
public:
  class apply
  {
  public:
    using type = T2T<int>;
  };
};

class BMF1
{
public:
  template <typename T1P>
  class apply
  {
  public:
    using type = T2T<T1P>;
  };
};

class BMF2
{
public:
  template <typename T1P, typename T2P>
  class apply
  {
  public:
    using type = boost::mpl::vector< T2T<T1P>, T2T<T2P> >;
  };
};

class BTF
{
public:
  using type = T2T<int>;
};

class MMF
{
public:
  class Apply
  {
  public:
    using Type = T2T<int>;
    using type = Type;
  };
  using apply = Apply;
};

class MMF1
{
public:
  template <typename T1P>
  class Apply
  {
  public:
    using Type = T2T<T1P>;
    using type = Type;
  };
  template <typename T1P>
  using apply = Apply<T1P>;
};

class MMF2
{
public:
  template <typename T1P, typename T2P>
  class Apply
  {
  public:
    using Type = boost::mpl::vector< T2T<T1P>, T2T<T2P> >;
    using type = Type;
  };
  template <typename T1P, typename T2P>
  using apply = Apply<T1P, T2P>;
};

class MTF
{
public:
  using Type = T2T<int>;
  using type = Type;
};

} // namespace Test



template <typename... TypesP>
class MakeVoid
{
public:
  using Type = void;
};

template <typename... TypesP>
using VoidT = typename MakeVoid<TypesP...>::Type;



template <template <typename...> class ApplyTmplP>
class IsTemplateTemplate
{
public:
  using Type = std::true_type;
};



template <typename, typename = VoidT<>>
class HasApplyTemplateMember : public std::false_type
{};
template<typename TypeP>
class HasApplyTemplateMember<TypeP, VoidT<typename IsTemplateTemplate<TypeP::template Apply>::Type> > : public std::true_type
{};

static_assert (HasApplyTemplateMember<Test::MF>::value == false,   "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MF1>::value == true,   "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MF2>::value == true,   "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::BMF>::value == false,  "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::BMF1>::value == false, "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::BMF2>::value == false, "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MMF>::value == false,  "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MMF1>::value == true,  "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MMF2>::value == true,  "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::TF>::value == false,   "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::BTF>::value == false,  "HasApplyTemplateMember works");
static_assert (HasApplyTemplateMember<Test::MTF>::value == false,  "HasApplyTemplateMember works");


template <typename, typename = VoidT<>>
class HasBoostApplyTemplateMember : public std::false_type
{};
template<typename TypeP>
class HasBoostApplyTemplateMember<TypeP, VoidT<typename IsTemplateTemplate<TypeP::template apply>::Type> > : public std::true_type
{};

static_assert (HasBoostApplyTemplateMember<Test::MF>::value == false,  "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MF1>::value == false, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MF2>::value == false, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::BMF>::value == false, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::BMF1>::value == true, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::BMF2>::value == true, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MMF>::value == false, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MMF1>::value == true, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MMF2>::value == true, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::TF>::value == false,  "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::BTF>::value == false, "HasBoostApplyTemplateMember works");
static_assert (HasBoostApplyTemplateMember<Test::MTF>::value == false, "HasBoostApplyTemplateMember works");



template <typename, typename = VoidT<>>
class HasApplyMember : public std::false_type
{};
template<typename TypeP>
class HasApplyMember<TypeP, VoidT<typename TypeP::Apply>> : public std::true_type
{};

static_assert (HasApplyMember<Test::MF>::value == true,    "HasApplyMember works");
static_assert (HasApplyMember<Test::MF1>::value == false,  "HasApplyMember works");
static_assert (HasApplyMember<Test::MF2>::value == false,  "HasApplyMember works");
static_assert (HasApplyMember<Test::BMF>::value == false,  "HasApplyMember works");
static_assert (HasApplyMember<Test::BMF1>::value == false, "HasApplyMember works");
static_assert (HasApplyMember<Test::BMF2>::value == false, "HasApplyMember works");
static_assert (HasApplyMember<Test::MMF>::value == true,   "HasApplyMember works");
static_assert (HasApplyMember<Test::MMF1>::value == false, "HasApplyMember works");
static_assert (HasApplyMember<Test::MMF2>::value == false, "HasApplyMember works");
static_assert (HasApplyMember<Test::TF>::value == false,   "HasApplyMember works");
static_assert (HasApplyMember<Test::BTF>::value == false,  "HasApplyMember works");
static_assert (HasApplyMember<Test::MTF>::value == false,  "HasApplyMember works");



template <typename, typename = VoidT<>>
class HasBoostApplyMember : public std::false_type
{};
template<typename TypeP>
class HasBoostApplyMember<TypeP, VoidT<typename TypeP::apply>> : public std::true_type
{};

static_assert (HasBoostApplyMember<Test::MF>::value == false,   "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MF1>::value == false,  "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MF2>::value == false,  "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::BMF>::value == true,   "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::BMF1>::value == false, "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::BMF2>::value == false, "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MMF>::value == true,   "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MMF1>::value == false, "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MMF2>::value == false, "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::TF>::value == false,   "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::BTF>::value == false,  "HasBoostApplyMember works");
static_assert (HasBoostApplyMember<Test::MTF>::value == false,  "HasBoostApplyMember works");



template <typename MetaFuncP>
class IsMetaFunc : public std::integral_constant<bool, HasApplyTemplateMember<MetaFuncP>::value ^ HasApplyMember<MetaFuncP>::value>
{};

static_assert (IsMetaFunc<Test::MF>::value == true,    "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MF1>::value == true,   "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MF2>::value == true,   "IsMetaFunc works");
static_assert (IsMetaFunc<Test::BMF>::value == false,  "IsMetaFunc works");
static_assert (IsMetaFunc<Test::BMF1>::value == false, "IsMetaFunc works");
static_assert (IsMetaFunc<Test::BMF2>::value == false, "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MMF>::value == true,   "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MMF1>::value == true,  "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MMF2>::value == true,  "IsMetaFunc works");
static_assert (IsMetaFunc<Test::TF>::value == false,   "IsMetaFunc works");
static_assert (IsMetaFunc<Test::BTF>::value == false,  "IsMetaFunc works");
static_assert (IsMetaFunc<Test::MTF>::value == false,  "IsMetaFunc works");



template <typename MetaFuncP>
class IsBoostMetaFunc : public std::integral_constant<bool, HasBoostApplyTemplateMember<MetaFuncP>::value ^ HasBoostApplyMember<MetaFuncP>::value>
{};

static_assert (IsBoostMetaFunc<Test::MF>::value == false,  "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MF1>::value == false, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MF2>::value == false, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::BMF>::value == true,  "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::BMF1>::value == true, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::BMF2>::value == true, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MMF>::value == true,  "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MMF1>::value == true, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MMF2>::value == true, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::TF>::value == false,  "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::BTF>::value == false, "IsBoostMetaFunc works");
static_assert (IsBoostMetaFunc<Test::MTF>::value == false, "IsBoostMetaFunc works");



template <typename, typename = VoidT<>>
class IsTrivialMetaFunc : public std::false_type
{};
template<typename TypeP>
class IsTrivialMetaFunc<TypeP, VoidT<typename TypeP::Type>> : public std::true_type
{};

static_assert (IsTrivialMetaFunc<Test::MF>::value == false,   "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MF1>::value == false,  "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MF2>::value == false,  "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::BMF>::value == false,  "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::BMF1>::value == false, "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::BMF2>::value == false, "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MMF>::value == false,  "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MMF1>::value == false, "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MMF2>::value == false, "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::TF>::value == true,    "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::BTF>::value == false,  "IsTrivialMetaFunc works");
static_assert (IsTrivialMetaFunc<Test::MTF>::value == true,   "IsTrivialMetaFunc works");



template <typename, typename = VoidT<>>
class IsBoostTrivialMetaFunc : public std::false_type
{};
template<typename TypeP>
class IsBoostTrivialMetaFunc<TypeP, VoidT<typename TypeP::type>> : public std::true_type
{};

static_assert (IsBoostTrivialMetaFunc<Test::MF>::value == false,   "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MF1>::value == false,  "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MF2>::value == false,  "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::BMF>::value == false,  "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::BMF1>::value == false, "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::BMF2>::value == false, "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MMF>::value == false,  "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MMF1>::value == false, "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MMF2>::value == false, "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::TF>::value == false,   "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::BTF>::value == true,   "IsBoostTrivialMetaFunc works");
static_assert (IsBoostTrivialMetaFunc<Test::MTF>::value == true,   "IsBoostTrivialMetaFunc works");



template <typename MetaFuncP>
class IsBoostified : public std::integral_constant<bool, IsBoostTrivialMetaFunc<MetaFuncP>::value || IsBoostMetaFunc<MetaFuncP>::value>
{};

static_assert (IsBoostified<Test::MF>::value == false,  "IsBoostified works");
static_assert (IsBoostified<Test::MF1>::value == false, "IsBoostified works");
static_assert (IsBoostified<Test::MF2>::value == false, "IsBoostified works");
static_assert (IsBoostified<Test::BMF>::value == true,  "IsBoostified works");
static_assert (IsBoostified<Test::BMF1>::value == true, "IsBoostified works");
static_assert (IsBoostified<Test::BMF2>::value == true, "IsBoostified works");
static_assert (IsBoostified<Test::MMF>::value == true,  "IsBoostified works");
static_assert (IsBoostified<Test::MMF1>::value == true, "IsBoostified works");
static_assert (IsBoostified<Test::MMF2>::value == true, "IsBoostified works");
static_assert (IsBoostified<Test::TF>::value == false,  "IsBoostified works");
static_assert (IsBoostified<Test::BTF>::value == true,  "IsBoostified works");
static_assert (IsBoostified<Test::MTF>::value == true,  "IsBoostified works");



class Boostify
{
public:
  template <typename MetaFuncP, typename = void>
  class Apply;

  template <typename MetaFuncP>
  class Apply<MetaFuncP, std::enable_if_t<!IsBoostified<MetaFuncP>::value && IsMetaFunc<MetaFuncP>::value && !IsTrivialMetaFunc<MetaFuncP>::value> >
  {
  public:
    class Type
    {
    public:
      template <typename... ArgsP>
      class apply // : public MetaFuncP::template Apply<ArgsP...>
      {
      public:
        using type = typename MetaFuncP::template Apply<ArgsP...>::Type;
      };
    };
  };

  template <typename MetaFuncP>
  class Apply<MetaFuncP, std::enable_if_t<!IsBoostified<MetaFuncP>::value && !IsMetaFunc<MetaFuncP>::value && IsTrivialMetaFunc<MetaFuncP>::value> >
  {
  public:
    class Type
    {
    public:
      using type = typename MetaFuncP::Type;
    };
  };

  template <typename MetaFuncP>
  class Apply<MetaFuncP, std::enable_if_t<IsBoostified<MetaFuncP>::value>>
  {
  public:
    class Type : public MetaFuncP
    {};
  };
};

template <typename MetaFuncP>
using BoostifyT = typename Boostify::Apply<MetaFuncP>::Type;

static_assert (IsBoostified<BoostifyT<Test::MF>>::value == true,   "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MF1>>::value == true,  "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MF2>>::value == true,  "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::BMF>>::value == true,  "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::BMF1>>::value == true, "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::BMF2>>::value == true, "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MMF>>::value == true,  "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MMF1>>::value == true, "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MMF2>>::value == true, "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::TF>>::value == true,   "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::BTF>>::value == true,  "Boostify works");
static_assert (IsBoostified<BoostifyT<Test::MTF>>::value == true,  "Boostify works");

} // namespace Mpl

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_MPL_BOOSTIFY_HH
