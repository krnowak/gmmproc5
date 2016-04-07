// node
// - attributes
//   - name
//   - type
//     - bool
//       - implicitly false
//       - implicitly true
//     - integral
//     - string
// - children
//   - multiplicity
//     - optional (0 or 1)
//       - value-like (std::optional)
//       - pointer-like (std::unique_ptr)
//     - single (exactly 1)
//     - multi (0 or more)
//       - vector (indexed)
//       - mapvector (indexed and keyed)
//   - type
//     - child type as tag
//     - adapter type
//       - variant with multiple children types
// - has text

// ...C - concept
// ...P - template parameter

// NodeAttrC - concept for an attribute of a node
// - using Name - mpl::string
// - using Type - attribute type (std::string, bool, int)
// - static void set(Type& attr, Base::Node& node)
// NodeChildTagC
// - TagToTypeT<NodeChildTagC> == NodeChildC
// NodeChildC - concept for a child element of a node
// - using Name - mpl::string
// - using Type - child type (NodeChildC, std::vector<NodeChildC>, ...)
// - using 
// - static void set(Type& child, Base::Node& node)
// - ?
// - using Tag - NodeChildTagC

// AttrListP - mpl::vector<NodeAttrC...>
// ChildListP - mpl::vector<NodeChildC...>
// HasTextP - std::bool_constant
// Tag<AttrList, ChildListP, HasTextP>

template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
class Storage
{
private:
  using AllAttrTypes = GetAttributeTypeListT<AttrListP>;
  using AllChildContainerTypes = GetChildContainerTypeListT<ChildListP>;
  using AllTypes = GetAllTypesT<AllAttrTypes, AllChildContainerTypes, HasTextTypeP>;

public:
  using TupleType = GetStdTupleTypeT<AllTypes>;

protected:
  TupleType storage;
};

// plain child - a "specialization" of a generic child
// - multiplicity
// - child type (tag?)
// generic child


// namespace Xml::Structured
// {
//
// template <typename TypeP, typename TagP>
// class SimpleChild
// {
// public:
//   using Types = mpl::vector<FooT<TypeP>>;
//   ...
// };
//
// template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
// class Node
// {
//   ...
// };
//
// } // namespace Xml::Structured
//
// namespace Xml::Structured::Node
// {
//
// template <...>
// class Single, Optional, MapVector, Vector
// {
//   ...
// };
//
// } // namespace Xml::Structured::Node
//
// namespace Xml::Structured::Attribute
// {
//
// class String
// {
// public:
//   template <typename NameP>
//   using Foo = ...;
// };
//
// template <int DefaultVP>
// class Int
// { ... };
//
// template <bool DefaultVP>
// class Bool
// { ... };
//
// } // namespace Xml::Structured
//
// namespace Xml::Structured::Registry
// {
//
// template <typename AttributeP>
// class AttrInfo;
// { using Name; using Type; };
//
// template <typename NodeP>
// class NodeInfo;
// { using Name; using Node; };
//
// } // namespace Xml::Structured::Registry
//
// namespace Gir::Tag
// {
//
// class Doc;
// class Type;
// class Array;
//
// } // namespace Gir::Tag
//
// namespace Gir::Attribute
// {
//
// class Name;
//
// } // namespace Gir::Attribute
//
// namespace Xml::Structured::Registry
// {
//
// template <>
// class AttrInfo<Attribute::Name>
// {
// public:
//   using Name = staticstring<'n', 'a', 'm', 'e'>
//   using Type = String
// };
//
// } // namespace Xml::Structured::Registry
//
// namespace Node
// {
//
// using Type = Node<
//   mpl::vector<ATTRIBUTES>,
//   mpl::vector<
//     OptVariant<Pointer, mpl::vector<Tags::Type, Tags::Array>>
//   >
// >;
//
// using Array = Node<
//   mpl::vector<>,
//   mpl::vector<
//     OptVariant<Pointer, mpl::vector<Type, Array>>
//   >
// >;
//
// using Field = Node<
//   mpl::vector<
//     NameAttr
//   >,
//   mpl::vector<
//     PlainChild<Optional<Value>, DocTag>,
//     Variant<mpl::vector<TypeTag, ArrayTag>>
//   >
// >;
//
// template <typename ChildTagP>
// using DocNode = Node<
//   mpl::vector<
//     // no attributes
//   >,
//   mpl::vector<
//     PlainChild<Single, ChildTagP>
//   >
// >
// using Document = DocNode<FieldTag>
//
// template <typename TagP>
// class NodeInfo;
//
// template <>
// class NodeInfo<TypeTag>
// {
// public:
//   using Name = StaticString<'t', 'y', 'p', 'e'>;
//   using Node = Type;
// };
//
// Document doc;
// doc.process(xml_doc);
//
// auto const& field = doc.get<Field> ();
// auto const& name = field.get<What? An attribute name? Some name typedef, so it looks like a separate type?> ();

// getters - single, optional, vector, mapvector
// child - either a child or an aggregate (e.g. variant)

template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
class ProcessorAndGetters
{
private:
  using StorageType = Storage<AttrListP, ChildListP, HasTextTypeP>;

public:
  using All = TagsToGettersT<UniqueTags, StorageType>;
};

template <typename AttrListP, typename ChildListP, typename HasTextTypeP = NoText>
class Node : public typename ProcessorAndGetters<AttrListP, ChildListP, HasTextTypeP>::All
{

};
