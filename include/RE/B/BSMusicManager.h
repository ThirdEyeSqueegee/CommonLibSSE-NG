#pragma once

#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTSingleton.h"

namespace RE
{
	class BSIMusicType;
	struct BSMusicEvent;

	class BSMusicManager :
		public BSTSingletonSDM<BSMusicManager>,  // 00
		public BSTEventSink<BSMusicEvent>        // 08
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSMusicManager;
		inline static constexpr auto VTABLE = VTABLE_BSMusicManager;

		~BSMusicManager() override;  // 00

		// override (BSTEventSink<BSMusicEvent>)
		BSEventNotifyControl ProcessEvent(const BSMusicEvent* a_event, BSTEventSource<BSMusicEvent>* a_eventSource) override;  // 01

		[[nodiscard]] static BSMusicManager* GetSingleton()
		{
			static REL::Relocation<BSMusicManager**> singleton{ RELOCATION_ID(514738, 400896) };
			return *singleton;
		}

		// members
		BSTArray<BSIMusicType*> musicQueue;  // 10
		BSIMusicType*           current;     // 28
		std::uint32_t           unk30;       // 30
		std::uint32_t           pad34;       // 34
	private:
		KEEP_FOR_RE()
	};
	static_assert(sizeof(BSMusicManager) == 0x38);
}
