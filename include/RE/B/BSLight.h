#pragma once

#include "RE/B/BSTArray.h"
#include "RE/N/NiPoint3.h"
#include "RE/N/NiRefObject.h"
#include "RE/N/NiSmartPointer.h"

namespace RE
{
	class BSCullingProcess;
	class BSLensFlareRenderData;
	class BSMultiBoundRoom;
	class BSPortal;
	class BSPortalGraph;
	class BSPortalSharedNode;
	class BSTriShape;
	class NiAVObject;
	class NiLight;

	class BSLight : public NiRefObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSLight;
		inline static constexpr auto VTABLE = VTABLE_BSLight;

		struct ShapeData
		{
			std::uint64_t         unk00;       // 00
			std::uint64_t         unk08;       // 08
			NiPointer<BSTriShape> fenceShape;  // 10
		};
		static_assert(sizeof(ShapeData) == 0x18);

		struct Data
		{
			std::uint64_t unk00;  // 00
			std::uint64_t unk08;  // 08
			std::uint32_t unk10;  // 10
			ShapeData*    data;   // 18
		};
		static_assert(sizeof(Data) == 0x20);

		~BSLight() override;  // 00

		// add
		virtual bool SetLight(NiLight* a_light);  // 02
		virtual bool IsShadowLight();             // 03

		// members
		float                         luminance;          // 010
		float                         lodDimmer;          // 014
		Data                          unk018;             // 018
		std::uint64_t                 unk038;             // 038
		std::uint32_t                 unk040;             // 040
		bool                          pointLight;         // 044
		bool                          ambientLight;       // 045
		bool                          dynamic;            // 046
		bool                          portalStrict;       // 047
		NiPointer<NiLight>            light;              // 048
		NiPoint3                      worldTranslate;     // 050
		std::uint32_t                 frustrumCull;       // 05C
		std::uint8_t                  unk060;             // 060
		bool                          affectLand;         // 061
		bool                          affectWater;        // 062
		bool                          neverFades;         // 063
		std::uint32_t                 unk064;             // 064
		std::uint64_t                 unk068;             // 068
		std::uint8_t                  unk070[96];         // 070
		std::uint8_t                  unk0D0;             // 0D0
		std::uint8_t                  pad0D1;             // 0D1
		std::uint16_t                 pad0D2;             // 0D2
		std::uint32_t                 pad0D4;             // 0D4
		BSTArray<BSMultiBoundRoom*>   rooms;              // 0D8
		BSTArray<BSPortal*>           portals;            // 0F0
		BSTArray<BSPortalSharedNode*> portalSharedNodes;  // 108
		BSPortalGraph*                portalGraph;        // 120
		BSCullingProcess*             cullingProcess;     // 128
		NiPointer<NiAVObject>         objectNode;         // 130
		BSLensFlareRenderData*        lensFlareData;      // 138
	private:
		KEEP_FOR_RE()
	};
	static_assert(sizeof(BSLight) == 0x140);
}
