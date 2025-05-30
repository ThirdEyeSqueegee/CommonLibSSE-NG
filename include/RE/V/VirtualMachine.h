#pragma once

#include "RE/A/AttachedScript.h"
#include "RE/B/BSAtomic.h"
#include "RE/B/BSTArray.h"
#include "RE/B/BSTEvent.h"
#include "RE/B/BSTFreeList.h"
#include "RE/B/BSTHashMap.h"
#include "RE/B/BSTMessageQueue.h"
#include "RE/B/BSTSmartPointer.h"
#include "RE/F/FunctionMessage.h"
#include "RE/I/IVMDebugInterface.h"
#include "RE/I/IVMObjectBindInterface.h"
#include "RE/I/IVMSaveLoadInterface.h"
#include "RE/I/IVirtualMachine.h"
#include "RE/L/LinkerProcessor.h"
#include "RE/S/SuspendedStack.h"

namespace RE
{
	namespace BSScript
	{
		class Array;
		class ErrorLogger;
		class IFreezeQuery;
		class IProfilePolicy;
		class ISavePatcherInterface;
		class IStackCallbackFunctor;
		class IStackCallbackSaveInterface;
		class ObjectBindPolicy;
		class Stack;
		struct IMemoryPagePolicy;
		struct IObjectHandlePolicy;
		struct StatsEvent;

		namespace Internal
		{
			class CodeTasklet;

			class VirtualMachine :
				public IVirtualMachine,            // 0000
				public IVMObjectBindInterface,     // 0010
				public IVMSaveLoadInterface,       // 0018
				public IVMDebugInterface,          // 0020
				public BSTEventSource<StatsEvent>  // 0028
			{
			public:
				inline static constexpr auto RTTI = RTTI_BSScript__Internal__VirtualMachine;
				inline static constexpr auto VTABLE = VTABLE_BSScript__Internal__VirtualMachine;

				struct QueuedUnbindRefs
				{
				public:
					// members
					BSTSmartPointer<Object> obj;       // 00
					std::uint32_t           refCount;  // 08
					std::uint32_t           pad0C;     // 0C
				};
				static_assert(sizeof(QueuedUnbindRefs) == 0x10);

				enum class FreezeState
				{
					kNotFrozen = 0,
					kFreezing,
					kFrozen
				};

				~VirtualMachine() override;  // 00

				using RE::BSScript::IVirtualMachine::SendEvent;
				using RE::BSTEventSource<RE::BSScript::StatsEvent>::SendEvent;

				// override (IVirtualMachine)
				void SetLinkedCallback(ITypeLinkedCallback* a_callback) override;                                                                                    // 01
				void TraceStack(const char* a_str, VMStackID a_stackID, Severity a_severity = Severity::kInfo) override;                                             // 02
				void FormatAndPostMessage(const char* a_message, Severity a_severity) override;                                                                      // 03
				void Update(float a_budget) override;                                                                                                                // 04
				void UpdateTasklets(float a_budget) override;                                                                                                        // 05
				void SetOverstressed(bool a_set) override;                                                                                                           // 06 - { overstressed = a_set; }
				bool IsCompletelyFrozen() const override;                                                                                                            // 07
				bool RegisterObjectType(VMTypeID a_typeID, const char* a_className) override;                                                                        // 08
				bool GetScriptObjectType1(const BSFixedString& a_className, BSTSmartPointer<ObjectTypeInfo>& a_outTypeInfoPtr) override;                             // 09
				bool GetScriptObjectType2(VMTypeID a_typeID, BSTSmartPointer<ObjectTypeInfo>& a_outTypeInfoPtr) override;                                            // 0A
				bool GetScriptObjectTypeNoLoad1(const BSFixedString& a_className, BSTSmartPointer<ObjectTypeInfo>& a_typeInfoPtr) const override;                    // 0B
				bool GetScriptObjectTypeNoLoad2(VMTypeID a_typeID, BSTSmartPointer<ObjectTypeInfo>& a_outTypeInfoPtr) const override;                                // 0C
				bool GetTypeIDForScriptObject(const BSFixedString& a_className, VMTypeID& a_typeID) const override;                                                  // 0D
				void GetScriptObjectsWithATypeID(BSScrapArray<BSFixedString>& a_results) const override;                                                             // 0E
				bool GetParentNativeType(const BSFixedString& a_className, BSTSmartPointer<ObjectTypeInfo>& a_typeInfoPtr) override;                                 // 0F
				bool TypeIsValid(const BSFixedString& a_className) override;                                                                                         // 10
				bool ReloadType(const char* a_className) override;                                                                                                   // 11
				void TasksToJobs(BSJobs::JobList& a_jobList) override;                                                                                               // 12
				void CalculateFullReloadList(void) const override;                                                                                                   // 13
				bool CreateObject1(const BSFixedString& a_className, void* a_property, BSTSmartPointer<Object>& a_objPtr) override;                                  // 14
				bool CreateObject2(const BSFixedString& a_className, BSTSmartPointer<Object>& a_result) override;                                                    // 15
				bool CreateArray1(const TypeInfo& a_typeInfo, std::uint32_t a_size, BSTSmartPointer<Array>& a_arrayPtr) override;                                    // 16
				bool CreateArray2(TypeInfo::RawType a_typeID, const BSFixedString& a_className, std::uint32_t a_size, BSTSmartPointer<Array>& a_arrayPtr) override;  // 17
				bool BindNativeMethod(IFunction* a_fn) override;                                                                                                     // 18
				void SetCallableFromTasklets1(const char* a_className, const char* a_stateName, const char* a_fnName, bool a_callable) override;                     // 19
				void SetCallableFromTasklets2(const char* a_className, const char* a_fnName, bool a_callable) override;                                              // 1A - { SetCallableFromTasklets1(a_className, 0, a_fnName, a_callable); }
				// This is where the vtable differs between AE/SE and VR.
#if defined(EXCLUSIVE_SKYRIM_VR)
				void New_1B(void) override;
#endif
#ifndef SKYRIM_CROSS_VR
				void ForEachBoundObject(VMHandle a_handle, IForEachScriptObjectFunctor* a_functor) override;  // 1B, 1C
#endif
#if defined(EXCLUSIVE_SKYRIM_VR)
				void New_1D(void) override;
#endif
#ifndef SKYRIM_CROSS_VR
				bool                       FindBoundObject(VMHandle a_handle, const char* a_className, BSTSmartPointer<Object>& a_result) const override;                                                                                   // 1C
				void                       MoveBoundObjects(VMHandle a_from, VMHandle a_to) override;                                                                                                                                       // 1D
				void                       ResetAllBoundObjects(VMHandle a_handle) override;                                                                                                                                                // 1E
				bool                       CastObject(const BSTSmartPointer<Object>& a_fromObjPtr, const BSTSmartPointer<ObjectTypeInfo>& a_toTypeInfoPtr, BSTSmartPointer<Object>& a_toObjPtr) override;                                   // 1F
				bool                       SetPropertyValue(BSTSmartPointer<Object>& a_obj, const char* a_propertyName, Variable& a_setVal) override;                                                                                       // 20
				bool                       GetPropertyValue(BSTSmartPointer<Object>& a_obj, const char* a_propertyName, Variable& a_getVal) override;                                                                                       // 21
				bool                       GetVariableValue1(const BSTSmartPointer<Object>& a_objPtr, std::uint32_t a_index, Variable& a_out) const override;                                                                               // 22
				bool                       GetVariableValue2(VMHandle a_handle, const BSFixedString& a_className, std::int32_t a_variableIndex, Variable& a_out) const override;                                                            // 23
				void                       SendEvent(VMHandle a_handle, const BSFixedString& a_eventName, IFunctionArguments* a_args) override;                                                                                             // 24
				void                       SendEventAll(const BSFixedString& a_eventName, IFunctionArguments* a_args) override;                                                                                                             // 25
				bool                       DispatchStaticCall(const BSFixedString& a_className, const BSFixedString& a_fnName, IFunctionArguments* a_args, BSTSmartPointer<IStackCallbackFunctor>& a_result) override;                      // 26
				bool                       DispatchMethodCall1(BSTSmartPointer<Object>& a_obj, const BSFixedString& a_fnName, IFunctionArguments* a_args, BSTSmartPointer<IStackCallbackFunctor>& a_result) override;                       // 27
				bool                       DispatchMethodCall2(VMHandle a_handle, const BSFixedString& a_className, const BSFixedString& a_fnName, IFunctionArguments* a_args, BSTSmartPointer<IStackCallbackFunctor>& a_result) override;  // 28
				bool                       DispatchUnboundMethodCall(void) override;                                                                                                                                                        // 29
				bool                       IsWaitingOnLatent(VMStackID a_stackID) const override;                                                                                                                                           // 2A
				void                       ReturnFromLatent(VMStackID a_stackID, const Variable& a_val) override;                                                                                                                           // 2B
				ErrorLogger*               GetErrorLogger() override;                                                                                                                                                                       // 2C - { return errorLogger; }
				IObjectHandlePolicy*       GetObjectHandlePolicy1() override;                                                                                                                                                               // 2D - { return handlePolicy; }
				const IObjectHandlePolicy* GetObjectHandlePolicy2() const override;                                                                                                                                                         // 2E - { return handlePolicy; }
				ObjectBindPolicy*          GetObjectBindPolicy1() override;                                                                                                                                                                 // 2F - { return objectBindPolicy; }
				const ObjectBindPolicy*    GetObjectBindPolicy2() const override;                                                                                                                                                           // 30 - { return objectBindPolicy; }
				ISavePatcherInterface*     GetSavePatcherInterface() override;                                                                                                                                                              // 31 - { return savePatcherInterface; }
				void                       RegisterForLogEvent(BSTEventSink<LogEvent>* a_sink) override;                                                                                                                                    // 32
				void                       UnregisterForLogEvent(BSTEventSink<LogEvent>* a_sink) override;                                                                                                                                  // 33
				void                       RegisterForStatsEvent(BSTEventSink<StatsEvent>* a_sink) override;                                                                                                                                // 34
				void                       UnregisterForStatsEvent(BSTEventSink<StatsEvent>* a_sink) override;                                                                                                                              // 35
#endif

				// override (IVMObjectBindInterface)
				VMHandle GetBoundHandle(const BSTSmartPointer<Object>& a_objPtr) const override;                                                                   // 01
				bool     TypeCanBeBound(const BSFixedString& a_className, VMHandle a_handle) override;                                                             // 02
				void     BindObject(BSTSmartPointer<Object>& a_objPtr, VMHandle a_handle, bool a_conditional) override;                                            // 03
				void     HandleLoadedBinding(BSTSmartPointer<Object>& a_objPtr, VMHandle a_handle, bool a_conditional) override;                                   // 04
				void     RemoveAllBoundObjects(VMHandle a_handle) override;                                                                                        // 05
				void     RemoveAllDiskLoadedBoundObjects(VMHandle a_handle) override;                                                                              // 06
				void     HandleCObjectDeletion(VMHandle a_handle) override;                                                                                        // 07
				void     UnbindObject(const BSTSmartPointer<Object>& a_objPtr) override;                                                                           // 08
				bool     CreateObjectWithProperties(const BSFixedString& a_className, std::uint32_t a_numProperties, BSTSmartPointer<Object>& a_objPtr) override;  // 09
				bool     InitObjectProperties(BSTSmartPointer<Object>& a_objPtr, void* a_property, bool a_arg3) override;                                          // 0A

				// override (IVMSaveLoadInterface)
				void                                                              SaveGame(SaveStorageWrapper* a_wrapper, SkyrimScript::SaveFileHandleReaderWriter* a_handle, bool unk) override;  // 01
				void                                                              LoadGame(LoadStorageWrapper* a_wrapper, SkyrimScript::SaveFileHandleReaderWriter* a_handle) override;            // 02
				void                                                              MarkSaveInvalid(BSStorage* a_storage) override;                                                                  // 03
				std::byte                                                         GetSaveGameVersion() override;                                                                                   // 04 - { return this->saveGameVersion; }
				void                                                              CleanupSave() override;                                                                                          // 05
				void                                                              Unk_06() override;                                                                                               // 06
				void                                                              DropAllRunningData() override;                                                                                   // 07
				void                                                              Unk_08(void* unk) override;                                                                                      // 08 - { return unk; }
				void                                                              Unk_09(void) override;                                                                                           // 09 - { return 0; }
				void                                                              Unk_0A(VMHandle a_handle, Object** a_out) override;                                                              // 0A
				void                                                              Unk_0B(void* unk) override;                                                                                      // 0B - { return unk; }
				void                                                              Unk_0C(void) override;                                                                                           // 0C - { return 0; }
				void                                                              Unk_0D(VMHandle a_handle, Array** a_out) override;                                                               // 0D
				void                                                              Unk_0E(LoadStorageWrapper* a_storage, void* unk) override;                                                       // 0E
				bool                                                              GetStackByID(VMStackID a_stackID, Stack** a_out) override;                                                       // 0F
				WritableStringTable                                               GetWritableStringTable() override;                                                                               // 10 - { return this->writableStringTable; }
				const WritableStringTable                                         GetWritableStringTable2() override;                                                                              // 11 - { return this->writableStringTable; }
				ReadableStringTable                                               GetReadableStringTable() override;                                                                               // 12 - { return this->readableStringTable; }
				BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>*       GetWritableTypeTable() override;                                                                                 // 13 - { return this->writableTypeTable; }
				const BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>* GetWritableTypeTable2() override;                                                                                // 14 - { return this->writableTypeTable; }
				BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>*       GetReadableTypeTable() override;                                                                                 // 15 - { return this->readableTypeTable; }
				bool                                                              CreateEmptyTasklet(Stack* a_stack, Internal::CodeTasklet** a_out) override;                                      // 16

				static VirtualMachine* GetSingleton();

				// members
				ErrorLogger*                                                errorLogger;                 // 0080
				IMemoryPagePolicy*                                          memoryPagePolicy;            // 0088
				IObjectHandlePolicy*                                        handlePolicy;                // 0090
				ObjectBindPolicy*                                           objectBindPolicy;            // 0098
				IFreezeQuery*                                               freezeQuery;                 // 00A0
				IStackCallbackSaveInterface*                                stackCallbackSaveInterface;  // 00A8
				IProfilePolicy*                                             profilePolicy;               // 00B0
				ISavePatcherInterface*                                      savePatcherInterface;        // 00B8
				mutable BSSpinLock                                          typeInfoLock;                // 00C0
				LinkerProcessor                                             linker;                      // 00C8
				BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>  objectTypeMap;               // 0158
				BSTHashMap<VMTypeID, BSFixedString>                         typeIDToObjectType;          // 0188
				BSTHashMap<BSFixedString, VMTypeID>                         objectTypeToTypeID;          // 01B8
				BSTArray<BSTSmartPointer<ObjectTypeInfo>>                   typesToUnload;               // 01E8
				mutable BSSpinLock                                          funcQueueLock;               // 0200
				BSTStaticFreeList<FunctionMessage, 1024>                    funcMsgPool;                 // 0208
				BSTCommonLLMessageQueue<FunctionMessage>                    funcMsgQueue;                // 8220
				BSTArray<FunctionMessage>                                   overflowFuncMsgs;            // 8248
				BSTArray<CodeTasklet*>                                      vmTasks;                     // 8260
				std::uint32_t                                               uiWaitingFunctionMessages;   // 8278
				bool                                                        overstressed;                // 827C
				bool                                                        initialized;                 // 827D
				std::uint16_t                                               pad827E;                     // 827E
				BSTCommonStaticMessageQueue<SuspendedStack, 128>            suspendQueue1;               // 8280
				BSTCommonStaticMessageQueue<SuspendedStack, 128>            suspendQueue2;               // 8AA0
				BSTArray<SuspendedStack>                                    overflowSuspendArray1;       // 92C0
				BSTArray<SuspendedStack>                                    overflowSuspendArray2;       // 92D8
				mutable BSSpinLock                                          suspendQueueLock;            // 92F0
				BSTCommonStaticMessageQueue<SuspendedStack, 128>*           stacksToResume;              // 92F8 - ref to suspendQueue2
				BSTArray<SuspendedStack>*                                   stacksToResumeOverflow;      // 9300 - ref to overflowSuspendArray2
				BSTCommonStaticMessageQueue<SuspendedStack, 128>*           stacksToSuspend;             // 9308 - ref to suspendQueue1
				BSTArray<SuspendedStack>*                                   stacksToSuspendOverflow;     // 9310 - ref to overflowSuspendArray1
				mutable BSSpinLock                                          runningStacksLock;           // 9318
				BSTHashMap<VMStackID, BSTSmartPointer<Stack>>               allRunningStacks;            // 9320
				BSTHashMap<VMStackID, BSTSmartPointer<Stack>>               waitingLatentReturns;        // 9350
				VMStackID                                                   nextStackID;                 // 9380
				mutable BSSpinLock                                          frozenStacksLock;            // 9384
				std::uint32_t                                               pad938C;                     // 938C
				BSScript::Stack*                                            frozenStacks;                // 9390
				std::uint32_t                                               frozenStacksCount;           // 9398
				REX::EnumSet<FreezeState, std::uint32_t>                    freezeState;                 // 939C
				mutable BSSpinLock                                          attachedScriptsLock;         // 93A0
				BSTHashMap<VMHandle, BSTSmallSharedArray<AttachedScript>>   attachedScripts;             // 93A8
				std::uint32_t                                               unk93D8;                     // 93D8
				std::uint32_t                                               unk93DC;                     // 93DC
				BSTArray<BSTSmartPointer<Object>>                           objectsAwaitingCleanup;      // 93E0
				mutable BSSpinLock                                          arraysLock;                  // 93F8
				std::uint32_t                                               nextArrayToClean;            // 9400
				std::uint32_t                                               pad9404;                     // 9404
				BSTArray<BSTSmartPointer<Array>>                            arrays;                      // 9408
				mutable BSSpinLock                                          objectResetLock;             // 9420
				BSTArray<BSTSmartPointer<Object>>                           objectsAwaitingReset;        // 9428
				mutable BSSpinLock                                          objectLock;                  // 9440 - Used for objectTable/arrayTable
				BSTHashMap<VMHandle, BSTSmartPointer<Object>>               objectTable;                 // 9448 - Used when loading a save
				BSTHashMap<VMHandle, BSTSmartPointer<Array>>                arrayTable;                  // 9478 - Used when loading a save
				mutable BSSpinLock                                          queuedUnbindLock;            // 94A8
				BSTArray<QueuedUnbindRefs>                                  queuedUnbinds;               // 94B0
				std::byte                                                   saveGameVersion;             // 94C8 - Set when loading a save
				std::uint32_t                                               unk94CC;                     // 94CC
				std::uint32_t                                               unk94D0;                     // 94D0
				WritableStringTable                                         writableStringTable;         // 94D8 - Created/Used only while saving
				ReadableStringTable                                         readableStringTable;         // 94F0 - Created/Used only while loading a save
				BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>* writeableTypeTable;          // 9508 - Created/Used only while saving
				BSTHashMap<BSFixedString, BSTSmartPointer<ObjectTypeInfo>>* readableTypeTable;           // 9510 - Created/Used only while loading a save
			private:
				KEEP_FOR_RE()
			};
			static_assert(sizeof(VirtualMachine) == 0x9518);
		}
	}
}
