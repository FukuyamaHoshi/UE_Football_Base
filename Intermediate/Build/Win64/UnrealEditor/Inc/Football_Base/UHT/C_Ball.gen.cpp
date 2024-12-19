// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Football_Base/C_Ball.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeC_Ball() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_APawn();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Ball();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Ball_NoRegister();
UPackage* Z_Construct_UPackage__Script_Football_Base();
// End Cross Module References

// Begin Class AC_Ball
void AC_Ball::StaticRegisterNativesAC_Ball()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AC_Ball);
UClass* Z_Construct_UClass_AC_Ball_NoRegister()
{
	return AC_Ball::StaticClass();
}
struct Z_Construct_UClass_AC_Ball_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "C_Ball.h" },
		{ "ModuleRelativePath", "C_Ball.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AC_Ball>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AC_Ball_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APawn,
	(UObject* (*)())Z_Construct_UPackage__Script_Football_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Ball_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AC_Ball_Statics::ClassParams = {
	&AC_Ball::StaticClass,
	"Game",
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Ball_Statics::Class_MetaDataParams), Z_Construct_UClass_AC_Ball_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AC_Ball()
{
	if (!Z_Registration_Info_UClass_AC_Ball.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AC_Ball.OuterSingleton, Z_Construct_UClass_AC_Ball_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AC_Ball.OuterSingleton;
}
template<> FOOTBALL_BASE_API UClass* StaticClass<AC_Ball>()
{
	return AC_Ball::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AC_Ball);
AC_Ball::~AC_Ball() {}
// End Class AC_Ball

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Ball_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AC_Ball, AC_Ball::StaticClass, TEXT("AC_Ball"), &Z_Registration_Info_UClass_AC_Ball, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AC_Ball), 2140407288U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Ball_h_1002609689(TEXT("/Script/Football_Base"),
	Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Ball_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Ball_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
