// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Football_Base/C_Piece.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeC_Piece() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Piece();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Piece_NoRegister();
UPackage* Z_Construct_UPackage__Script_Football_Base();
// End Cross Module References

// Begin Class AC_Piece
void AC_Piece::StaticRegisterNativesAC_Piece()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AC_Piece);
UClass* Z_Construct_UClass_AC_Piece_NoRegister()
{
	return AC_Piece::StaticClass();
}
struct Z_Construct_UClass_AC_Piece_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "C_Piece.h" },
		{ "ModuleRelativePath", "C_Piece.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AC_Piece>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AC_Piece_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_Football_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Piece_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AC_Piece_Statics::ClassParams = {
	&AC_Piece::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Piece_Statics::Class_MetaDataParams), Z_Construct_UClass_AC_Piece_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AC_Piece()
{
	if (!Z_Registration_Info_UClass_AC_Piece.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AC_Piece.OuterSingleton, Z_Construct_UClass_AC_Piece_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AC_Piece.OuterSingleton;
}
template<> FOOTBALL_BASE_API UClass* StaticClass<AC_Piece>()
{
	return AC_Piece::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AC_Piece);
AC_Piece::~AC_Piece() {}
// End Class AC_Piece

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Piece_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AC_Piece, AC_Piece::StaticClass, TEXT("AC_Piece"), &Z_Registration_Info_UClass_AC_Piece, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AC_Piece), 714050359U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Piece_h_958015081(TEXT("/Script/Football_Base"),
	Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Piece_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Piece_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
