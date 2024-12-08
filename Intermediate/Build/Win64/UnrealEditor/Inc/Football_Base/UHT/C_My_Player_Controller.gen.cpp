// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Football_Base/C_My_Player_Controller.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeC_My_Player_Controller() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_APlayerController();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_My_Player_Controller();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_My_Player_Controller_NoRegister();
UPackage* Z_Construct_UPackage__Script_Football_Base();
// End Cross Module References

// Begin Class AC_My_Player_Controller
void AC_My_Player_Controller::StaticRegisterNativesAC_My_Player_Controller()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AC_My_Player_Controller);
UClass* Z_Construct_UClass_AC_My_Player_Controller_NoRegister()
{
	return AC_My_Player_Controller::StaticClass();
}
struct Z_Construct_UClass_AC_My_Player_Controller_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "HideCategories", "Collision Rendering Transformation" },
		{ "IncludePath", "C_My_Player_Controller.h" },
		{ "ModuleRelativePath", "C_My_Player_Controller.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AC_My_Player_Controller>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AC_My_Player_Controller_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APlayerController,
	(UObject* (*)())Z_Construct_UPackage__Script_Football_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AC_My_Player_Controller_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AC_My_Player_Controller_Statics::ClassParams = {
	&AC_My_Player_Controller::StaticClass,
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
	0x009002A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AC_My_Player_Controller_Statics::Class_MetaDataParams), Z_Construct_UClass_AC_My_Player_Controller_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AC_My_Player_Controller()
{
	if (!Z_Registration_Info_UClass_AC_My_Player_Controller.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AC_My_Player_Controller.OuterSingleton, Z_Construct_UClass_AC_My_Player_Controller_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AC_My_Player_Controller.OuterSingleton;
}
template<> FOOTBALL_BASE_API UClass* StaticClass<AC_My_Player_Controller>()
{
	return AC_My_Player_Controller::StaticClass();
}
AC_My_Player_Controller::AC_My_Player_Controller(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(AC_My_Player_Controller);
AC_My_Player_Controller::~AC_My_Player_Controller() {}
// End Class AC_My_Player_Controller

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_My_Player_Controller_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AC_My_Player_Controller, AC_My_Player_Controller::StaticClass, TEXT("AC_My_Player_Controller"), &Z_Registration_Info_UClass_AC_My_Player_Controller, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AC_My_Player_Controller), 1351387928U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_My_Player_Controller_h_2597018487(TEXT("/Script/Football_Base"),
	Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_My_Player_Controller_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_My_Player_Controller_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
