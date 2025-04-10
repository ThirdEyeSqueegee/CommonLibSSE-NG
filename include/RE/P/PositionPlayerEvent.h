#pragma once

namespace RE
{
	// related to cell transitions
	struct PositionPlayerEvent
	{
	public:
		enum class EVENT_TYPE
		{
			kPre = 0,
			kPreUpdatePackages = 1,
			kPostUpdatePackages = 2,
			kPost = 3,
			kFinish = 4
		};

		// members
		REX::EnumSet<EVENT_TYPE, std::uint32_t> type;  // 0
	private:
		KEEP_FOR_RE()
	};
	static_assert(sizeof(PositionPlayerEvent) == 0x4);
}
