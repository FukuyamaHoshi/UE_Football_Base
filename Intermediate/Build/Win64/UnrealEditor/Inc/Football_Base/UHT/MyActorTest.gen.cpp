// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Football_Base/MyActorTest.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMyActorTest() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AMyActorTest();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AMyActorTest_NoRegister();
UPackage* Z_Construct_UPackage__Script_Football_Base();
// End Cross Module References

// Begin Class AMyActorTest
void AMyActorTest::StaticRegisterNativesAMyActorTest()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AMyActorTest);
UClass* Z_Construct_UClass_AMyActorTest_NoRegister()
{
	return AMyActorTest::StaticClass();
}
struct Z_Construct_UClass_AMyActorTest_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MyActorTest.h" },
		{ "ModuleRelativePath", "MyActorTest.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AMyActorTest>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AMyActorTest_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_Football_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AMyActorTest_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AMyActorTest_Statics::ClassParams = {
	&AMyActorTest::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AMyActorTest_Statics::Class_MetaDataParams), Z_Construct_UClass_AMyActorTest_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AMyActorTest()
{
	if (!Z_Registration_Info_UClass_AMyActorTest.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AMyActorTest.OuterSingleton, Z_Construct_UClass_AMyActorTest_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AMyActorTest.OuterSingleton;
}
template<> FOOTBALL_BASE_API UClass* StaticClass<AMyActorTest>()
{
	return AMyActorTest::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AMyActorTest);
AMyActorTest::~AMyActorTest() {}
// End Class AMyActorTest

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_MyActorTest_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AMyActorTest, AMyActorTest::StaticClass, TEXT("AMyActorTest"), &Z_Registration_Info_UClass_AMyActorTest, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AMyActorTest), 2604739662U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_MyActorTest_h_2664235458(TEXT("/Script/Football_Base"),
	Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_MyActorTest_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_MyActorTest_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
