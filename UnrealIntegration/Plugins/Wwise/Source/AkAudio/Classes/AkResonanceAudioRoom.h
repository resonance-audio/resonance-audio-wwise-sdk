/*=============================================================================
	AkResonanceAudioRoom.h:
=============================================================================*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "Matrix.h"
#include "ObjectMacros.h"

#include "ResonanceAudioLibrary/room_properties.h"
#include "AkResonanceAudioRoom.generated.h"

using namespace std;
using namespace vraudio;

UENUM(BlueprintType)
enum class SurfaceMaterial : uint8
{
	// Full acoustic energy absorption.
	Transparent = 0							UMETA(DisplayName = "Transparent"),
	AcousticCeilingTiles = 1				UMETA(DisplayName = "Acoustic Ceiling Tiles"),
	BrickBare = 2							UMETA(DisplayName = "Brick Bare"),
	BrickPainted = 3						UMETA(DisplayName = "Brick Painted"),
	ConcreteBlockCoarse = 4					UMETA(DisplayName = "Concrete Block Coarse"),
	ConcreteBlockPainted = 5				UMETA(DisplayName = "Concrete Block Painted"),
	CurtainHeavy = 6						UMETA(DisplayName = "Curtain Heavy"),
	FiberglassInsulation = 7				UMETA(DisplayName = "Fiber Glass Insulation"),
	GlassThin = 8							UMETA(DisplayName = "Glass Thin"),
	GlassThick = 9							UMETA(DisplayName = "Glass Thick"),
	Grass = 10								UMETA(DisplayName = "Grass"),
	LinoleumOnConcrete = 11					UMETA(DisplayName = "Linoleum On Concrete"),
	Marble = 12								UMETA(DisplayName = "Marble"),
	Metal = 13								UMETA(DisplayName = "Metal"),
	ParquetOnConcrete = 14					UMETA(DisplayName = "Parquet On Concrete"),
	PlasterRough = 15						UMETA(DisplayName = "Plaster Rough"),
	PlasterSmooth = 16						UMETA(DisplayName = "Plaster Smooth"),
	PlywoodPanel = 17						UMETA(DisplayName = "Plywood Panel"),
	PolishedConcreteOrTile = 18				UMETA(DisplayName = "Polished Concrete Or Tile"),
	Sheetrock = 19							UMETA(DisplayName = "Sheetrock"),
	WaterOrIceSurface = 20					UMETA(DisplayName = "Water Or Ice Surface"),
	WoodCeiling = 21						UMETA(DisplayName = "Wood Ceiling"),
	WoodPanel = 22							UMETA(DisplayName = "Wood Panel"),
	Uniform = 23							UMETA(DisplayName = "Uniform")
};

UCLASS(hidecategories = (Advanced, Attachment, Volume), BlueprintType)
class AKAUDIO_API AAkResonanceAudioRoom : public AVolume
{
	GENERATED_UCLASS_BODY()

public:
	/** Whether this volume is currently enabled and able to affect sounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnable = true;

	// Left wall acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial LeftWall;

	// Right wall acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial RightWall;

	// Floor acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial Floor;

	// Ceiling acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial Ceiling;

	// Back wall acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial BackWall;

	// Front wall acoustic material
	UPROPERTY(EditAnywhere, Category = "Acoustic Materials")
	SurfaceMaterial FrontWall;

	// Reflectivity scalar for each surface of the room.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Reflectivity = 1.0f;

	// Reverb gain modifier in decibels.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-24.0", ClampMax = "24.0"))
	float ReverbGainDb = 0.0f;

	// Reverb brightness modifier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float ReverbBrightness = 0.0f;

	// Reverb time modifier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float ReverbTime = 1.0f;

	// Name of the Wwise Audio Bus where the Resonance Audio Room Effects plug-in is attached to.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString RoomEffectsBusName = "Room Effects Bus";

private:
	// Conversion factor between Resonance Audio and Unreal world distance units (1cm in Unreal = 0.01m in Resonance Audio).
	const float AK_SCALE_FACTOR = 0.01f;

	// Unreal class overrides
	void BeginPlay() override;
	void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	void Serialize(FArchive& Ar) override;
	void PostLoad() override;

	// Conversion helpers
	float ConvertAmplitudeFromDb(float db);
	FVector ConvertToAkResonanceAudioCoordinates(const FVector& UnrealVector);
	FQuat ConvertToAkResonanceAudioRotation(const FQuat& UnrealQuat);
	MaterialName ConvertToResonanceMaterialName(SurfaceMaterial UnrealMaterialName);

	bool IsListenerInsideRoom();
	void UpdateAudioRoom(AAkResonanceAudioRoom* room, bool enabled);
};