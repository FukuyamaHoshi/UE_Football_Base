// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Football_Base/C_Tile.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeC_Tile() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AActor();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Tile();
FOOTBALL_BASE_API UClass* Z_Construct_UClass_AC_Tile_NoRegister();
UPackage* Z_Construct_UPackage__Script_Football_Base();
// End Cross Module References

// Begin Class AC_Tile
void AC_Tile::StaticRegisterNativesAC_Tile()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AC_Tile);
UClass* Z_Construct_UClass_AC_Tile_NoRegister()
{
	return AC_Tile::StaticClass();
}
struct Z_Construct_UClass_AC_Tile_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "C_Tile.h" },
		{ "ModuleRelativePath", "C_Tile.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_tileNo_MetaData[] = {
		{ "Category", "Reference" },
		{ "ModuleRelativePath", "C_Tile.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_tileNo;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AC_Tile>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AC_Tile_Statics::NewProp_tileNo = { "tileNo", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AC_Tile, tileNo), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_tileNo_MetaData), NewProp_tileNo_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AC_Tile_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AC_Tile_Statics::NewProp_tileNo,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Tile_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AC_Tile_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_Football_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Tile_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AC_Tile_Statics::ClassParams = {
	&AC_Tile::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_AC_Tile_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_AC_Tile_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AC_Tile_Statics::Class_MetaDataParams), Z_Construct_UClass_AC_Tile_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AC_Tile()
{
	if (!Z_Registration_Info_UClass_AC_Tile.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AC_Tile.OuterSingleton, Z_Construct_UClass_AC_Tile_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AC_Tile.OuterSingleton;
}
template<> FOOTBALL_BASE_API UClass* StaticClass<AC_Tile>()
{
	return AC_Tile::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AC_Tile);
AC_Tile::~AC_Tile() {}
// End Class AC_Tile

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Tile_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AC_Tile, AC_Tile::StaticClass, TEXT("AC_Tile"), &Z_Registration_Info_UClass_AC_Tile, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AC_Tile), 2786073938U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Tile_h_4244701420(TEXT("/Script/Football_Base"),
	Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Tile_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_star1_Project_Football_Base_Game_Data_Football_Base_Source_Football_Base_C_Tile_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
