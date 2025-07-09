#pragma once

namespace KuchCraft {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid) :
			m_UUID(uuid) { }

		UUID(const UUID&) = default;

		inline operator uint64_t() const { return m_UUID; }
		operator const uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;

	};

}

namespace std {

	/// Specialization of std::hash for UUIDs.
	/// Provides a hashing function for UUID instances to enable their usage
	/// in standard hash-based containers like std::unordered_map. The hash 
	/// function simply casts the UUID to a uint64_t and uses that as the hash.
	template <typename T> struct hash;
	template<>
	struct hash<KuchCraft::UUID>
	{
		std::size_t operator()(const KuchCraft::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}