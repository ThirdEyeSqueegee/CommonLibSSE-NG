#pragma once

#include "RE/B/BSTArray.h"
#include "RE/B/BSTLocklessQueue.h"
#include "RE/N/NiCullingProcess.h"
#include "RE/N/NiSmartPointer.h"

namespace RE
{
	class BSCompoundFrustum;
	class BSCullingProcess;
	class BSMultiBound;
	class BSOcclusionPlane;
	class BSPortalGraphEntry;
	class BSShaderAccumulator;
	class NiAVObject;
	class NiBound;
	class NiFrustumPlanes;

	struct CullJobDescriptor
	{
		NiPointer<BSShaderAccumulator>   shaderAccumulator;           // 00
		NiPointer<BSShaderAccumulator>   shaderAccumulatorSecondary;  // 08
		NiPointer<NiCamera>              camera;                      // 10
		BSCompoundFrustum*               compoundFrustum;             // 18
		NiFrustum*                       frustum;                     // 20
		BSPortalGraphEntry*              portalGraphEntry;            // 28
		BSCullingProcess*                cullingProcess;              // 30
		NiFrustumPlanes*                 customCullPlanes;            // 38
		NiAVObject*                      scene;                       // 40
		BSTArray<NiPointer<NiAVObject>>* cullingObjects;              // 48
		uint32_t                         cullMode;                    // 50
		float                            lightRadius;                 // 54
		uint32_t                         unk58;                       // 58
		bool                             isParabolic;                 // 5C
		bool                             isDirectionalLight;          // 5D
		bool                             ignorePreprocess;            // 5E
		bool                             doCustomCullPlanes;          // 5F
		bool                             cameraRelatedUpdates;        // 60
		bool                             unk61;                       // 61
		bool                             updateAccumulateFlag;        // 62
	};
	static_assert(sizeof(CullJobDescriptor) == 0x68);

	class BSCullingProcess : public NiCullingProcess
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSCullingProcess;
		inline static constexpr auto Ni_RTTI = NiRTTI_BSCullingProcess;
		inline static constexpr auto VTABLE = VTABLE_BSCullingProcess;

		enum class BSCPCullingType
		{
			kNormal = 0,
			kAllPass = 1,
			kAllFail = 2,
			kIgnoreMultiBounds = 3,
			kForceMultiBoundsNoUpdate = 4
		};

		struct Data
		{
		public:
			// members
			std::uint8_t unk00[0x10];  // 00
		};
		static_assert(sizeof(Data) == 0x10);

		// override (NiCullingProcess)
		const NiRTTI* GetRTTI() const override;  // 00

		~BSCullingProcess() override;  // 15

		void Process1(NiAVObject* a_object, std::uint32_t a_arg2) override;                                   // 16
		void Process2(const NiCamera* a_camera, NiAVObject* a_scene, NiVisibleArray* a_visibleSet) override;  // 17
		void AppendVirtual(BSGeometry& a_visible, std::uint32_t a_arg2) override;                             // 18

		// add
		virtual void               AppendNonAccum(NiAVObject& a_object);               // 19
		virtual bool               TestBaseVisibility1(BSMultiBound& a_bound);         // 1A
		virtual bool               TestBaseVisibility2(BSOcclusionPlane& a_bound);     // 1B
		[[nodiscard]] virtual bool TestBaseVisibility3(const NiBound& a_bound) const;  // 1C

		BSTArray<NiPointer<NiAVObject>>                   unk00128;           // 00128
		BSTLocklessQueue::ObjMultiProdCons<Data, 4096, 0> cullQueue;          // 00140
		std::uint64_t                                     unk30160;           // 30160
		std::uint64_t                                     unk30168;           // 30168
		std::uint64_t                                     unk30170;           // 30170
		std::uint64_t                                     unk30178;           // 30178
		std::uint64_t                                     unk30180;           // 30180
		std::uint64_t                                     unk30188;           // 30188
		BSPortalGraphEntry*                               portalGraphEntry;   // 30190
		std::int32_t                                      cullMode;           // 30198
		BSCompoundFrustum*                                compoundFrustum;    // 301A0
		std::uint64_t                                     unk301A8;           // 301A8
		std::uint64_t                                     unk301B0;           // 301B0
		std::uint64_t                                     unk301B8;           // 301B8
		std::uint64_t                                     unk301C0;           // 301C0
		std::uint64_t                                     unk301C8;           // 301C8
		std::uint32_t                                     unk301D0;           // 301D0
		bool                                              recurseToGeometry;  // 301D4
		std::uint8_t                                      unk301D5;           // 301D5
		std::uint16_t                                     unk301D6;           // 301D6
		BSTArray<void*>                                   unk301D8;           // 301D8
		std::uint64_t                                     unk301F0;           // 301F0
	private:
		KEEP_FOR_RE()
	};
	static_assert(sizeof(BSCullingProcess) == 0x301F8);
}
