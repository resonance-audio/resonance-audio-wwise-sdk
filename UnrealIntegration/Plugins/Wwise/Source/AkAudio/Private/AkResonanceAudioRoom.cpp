/*=============================================================================
	AkResonanceAudioRoom.cpp:
=============================================================================*/

#include "AkResonanceAudioRoom.h"
#include "AkAudioDevice.h"
#include "AkAuxBus.h"
#include "AkComponent.h"
#include "AkCustomVersion.h"

#include "Components/BrushComponent.h"
#include "Model.h"
#include "Platforms/AkUEPlatform.h"
#include "RotationMatrix.h"

#include <iostream>
#include "AK/SoundEngine/Common/AkTypes.h"

using namespace vraudio;

/*------------------------------------------------------------------------------------
	AAkResonanceAudioRoom
------------------------------------------------------------------------------------*/

// Container to store the currently active rooms in the scene.
static TArray<AAkResonanceAudioRoom*> enabledRooms;

AAkResonanceAudioRoom::AAkResonanceAudioRoom(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Property initialization
	static const FName CollisionProfileName(TEXT("OverlapAll"));
	UBrushComponent* BrushComp = GetBrushComponent();
	if (BrushComp)
	{
		BrushComp->SetCollisionProfileName(CollisionProfileName);
	}

	bColored = true;
	BrushColor = FColor(0, 255, 255, 255);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
}

void AAkResonanceAudioRoom::BeginPlay()
{
	Super::BeginPlay();
}

void AAkResonanceAudioRoom::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FAkCustomVersion::GUID);
}

void AAkResonanceAudioRoom::PostLoad()
{
	Super::PostLoad();
	const int32 AkVersion = GetLinkerCustomVersion(FAkCustomVersion::GUID);
}

void AAkResonanceAudioRoom::UpdateAudioRoom(AAkResonanceAudioRoom* room, bool roomEnabled)
{
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();

	if (roomEnabled)
	{
		if (!enabledRooms.Contains(room))
		{
			enabledRooms.Add(room);
		}
	}
	else
	{
		enabledRooms.RemoveSingleSwap(room, true);
	}

	if (enabledRooms.Max() > 0)
	{
		AAkResonanceAudioRoom* currentRoom = enabledRooms[enabledRooms.Num() - 1];

		FVector actorPosition = ConvertToAkResonanceAudioCoordinates(GetActorLocation());
		FQuat actorRotation = ConvertToAkResonanceAudioRotation(GetActorQuat());
		FVector actorScale = GetActorScale();

		// Create the room data object.
		RoomProperties * roomGameData = nullptr;
		roomGameData = (RoomProperties *)_alloca(sizeof(RoomProperties));
		// Fill in the data.
		roomGameData->position[0] = actorPosition.X;
		roomGameData->position[1] = actorPosition.Y;
		roomGameData->position[2] = actorPosition.Z;
		roomGameData->rotation[0] = actorRotation.X;
		roomGameData->rotation[1] = actorRotation.Y;
		roomGameData->rotation[2] = actorRotation.Z;
		roomGameData->rotation[3] = actorRotation.W;
		roomGameData->dimensions[0] = actorScale.X;
		roomGameData->dimensions[1] = actorScale.Y;
		roomGameData->dimensions[2] = actorScale.Z;
		roomGameData->material_names[0] = ConvertToResonanceMaterialName(LeftWall);
		roomGameData->material_names[1] = ConvertToResonanceMaterialName(RightWall);
		roomGameData->material_names[2] = ConvertToResonanceMaterialName(Floor);
		roomGameData->material_names[3] = ConvertToResonanceMaterialName(Ceiling);
		roomGameData->material_names[4] = ConvertToResonanceMaterialName(FrontWall);
		roomGameData->material_names[5] = ConvertToResonanceMaterialName(BackWall);
		roomGameData->reflection_scalar = Reflectivity;
		roomGameData->reverb_gain = ConvertAmplitudeFromDb(ReverbGainDb);
		roomGameData->reverb_time = ReverbTime;
		roomGameData->reverb_brightness = ReverbBrightness;

		const void *sendData = static_cast<const RoomProperties*>(roomGameData);

		// Send to Resonance.
		AK::SoundEngine::SendPluginCustomGameData(AkAudioDevice->GetIDFromString(RoomEffectsBusName),
													NULL, AkPluginTypeMixer, AKCOMPANYID_GOOGLE, 200,
													sendData, sizeof(RoomProperties));
	}
	else
	{
		AK::SoundEngine::SendPluginCustomGameData(AkAudioDevice->GetIDFromString(RoomEffectsBusName),
													NULL, AkPluginTypeMixer, AKCOMPANYID_GOOGLE, 200,
													nullptr, 0);
	}
}

void AAkResonanceAudioRoom::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	if (bEnable)
		UpdateAudioRoom(this, IsListenerInsideRoom());
}

// Converts given |db| value to its amplitude equivalent where 'dB = 20 * log10(amplitude)'.
float AAkResonanceAudioRoom::ConvertAmplitudeFromDb(float db)
{
	return pow(10.0f, 0.05f * db);
}

/*       RESONANCE AUDIO                UNREAL
		   Y                             Z
		   |					         |    X
		   |						     |   /
		   |						     |  /
		   |						     | /
		   |_______________X			 |/_______________Y
		  /
		 /
		/
	   Z
*/

FVector AAkResonanceAudioRoom::ConvertToAkResonanceAudioCoordinates(const FVector& UnrealVector)
{
	FVector AkResonanceAudioVector;
	AkResonanceAudioVector.X = UnrealVector.Y;
	AkResonanceAudioVector.Y = UnrealVector.Z;
	AkResonanceAudioVector.Z = -UnrealVector.X;
	return AkResonanceAudioVector * AK_SCALE_FACTOR;
}

FQuat AAkResonanceAudioRoom::ConvertToAkResonanceAudioRotation(const FQuat& UnrealQuat)
{
	FQuat AkResonanceAudioQuat;
	AkResonanceAudioQuat.X = -UnrealQuat.Y;
	AkResonanceAudioQuat.Y = -UnrealQuat.Z;
	AkResonanceAudioQuat.Z = UnrealQuat.X;
	AkResonanceAudioQuat.W = UnrealQuat.W;
	return AkResonanceAudioQuat;
}

MaterialName AAkResonanceAudioRoom::ConvertToResonanceMaterialName(SurfaceMaterial UnrealMaterialName)
{
	// These are rough estimates of what scalar gain coefficients may correspond to a given material,
	// though many of these materials have similar gain coefficients and drastically different frequency characteristics.
	switch (UnrealMaterialName)
	{
	case SurfaceMaterial::Transparent:
		return MaterialName::kTransparent;
	case SurfaceMaterial::AcousticCeilingTiles:
		return MaterialName::kAcousticCeilingTiles;
	case SurfaceMaterial::BrickBare:
		return MaterialName::kBrickBare;
	case SurfaceMaterial::BrickPainted:
		return MaterialName::kBrickPainted;
	case SurfaceMaterial::ConcreteBlockCoarse:
		return MaterialName::kConcreteBlockCoarse;
	case SurfaceMaterial::ConcreteBlockPainted:
		return MaterialName::kConcreteBlockPainted;
	case SurfaceMaterial::CurtainHeavy:
		return MaterialName::kCurtainHeavy;
	case SurfaceMaterial::FiberglassInsulation:
		return MaterialName::kFiberGlassInsulation;
	case SurfaceMaterial::GlassThick:
		return MaterialName::kGlassThick;
	case SurfaceMaterial::GlassThin:
		return MaterialName::kGlassThin;
	case SurfaceMaterial::Grass:
		return MaterialName::kGrass;
	case SurfaceMaterial::LinoleumOnConcrete:
		return MaterialName::kLinoleumOnConcrete;
	case SurfaceMaterial::Marble:
		return MaterialName::kMarble;
	case SurfaceMaterial::Metal:
		return MaterialName::kMarble;
	case SurfaceMaterial::ParquetOnConcrete:
		return MaterialName::kParquetOnConcrete;
	case SurfaceMaterial::PlasterRough:
		return MaterialName::kPlasterRough;
	case SurfaceMaterial::PlasterSmooth:
		return MaterialName::kPlasterSmooth;
	case SurfaceMaterial::PlywoodPanel:
		return MaterialName::kPlywoodPanel;
	case SurfaceMaterial::PolishedConcreteOrTile:
		return MaterialName::kPolishedConcreteOrTile;
	case SurfaceMaterial::Sheetrock:
		return MaterialName::kSheetrock;
	case SurfaceMaterial::WaterOrIceSurface:
		return MaterialName::kWaterOrIceSurface;
	case SurfaceMaterial::WoodCeiling:
		return MaterialName::kWoodCeiling;
	case SurfaceMaterial::WoodPanel:
		return MaterialName::kWoodPanel;
	case SurfaceMaterial::Uniform:
		return MaterialName::kUniform;
	default:
		return MaterialName::kTransparent;
	}
}

bool AAkResonanceAudioRoom::IsListenerInsideRoom()
{
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	UAkComponent* Listener = AkAudioDevice->GetSpatialAudioListener();
	bool inBounds = GetBounds().GetBox().IsInside(Listener->GetPosition());
	return inBounds;
}