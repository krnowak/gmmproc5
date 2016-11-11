// three things for child
// - multiplicity (optional, single, many)
// - access (singular, indexed, keyed)
// - type

// part concept

class Part
{
public:
  class GetContainedType
  {
  public:
    using Type = /* some type */;
  };

  class GetAccessInfo
  {
  public:
    template <typename StorageP>
    class Apply
    {
    public:
      using Type = map<access_key_type, map<getter, getter_policy>>;
    };
  };

  class GetGenerator
  {
    // TODO: finish
  };
}
