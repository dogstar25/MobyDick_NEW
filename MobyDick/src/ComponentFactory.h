#pragma once
#include <json/json.h>
#include "components/Component.h"

#include "components/AnimationComponent.h"
#include "components/ActionComponent.h"
#include "components/AttachmentsComponent.h"
#include "components/BrainComponent.h"
#include "components/ChildrenComponent.h"
#include "components/CompositeComponent.h"
#include "components/ContainerComponent.h"
#include "components/CheckPointComponent.h"
#include "components/HudComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/InventoryComponent.h"
#include "components/InterfaceComponent.h"
#include "components/NavigationComponent.h"
#include "components/ParticleComponent.h"
#include "components/ParticleXComponent.h"
#include "components/PoolComponent.h"
#include "components/PhysicsComponent.h"
#include "components/PuzzleComponent.h"
#include "components/RenderComponent.h"
#include "components/MaskedOverlayComponent.h"
#include "components/SoundComponent.h"
#include "components/StateComponent.h"
#include "components/TextComponent.h"
#include "components/TransformComponent.h"
#include "components/VitalityComponent.h"
#include "components/WeaponComponent.h"
#include "components/PlayerControlComponent.h"
#include "components/IMGuiComponent.h"
#include "components/LightComponent.h"
#include "components/LightedTreatmentComponent.h"
#include "components/GridDisplayComponent.h"



class Scene;

class ComponentFactory
{


public:

	ComponentFactory() = default;

	//Create version that has ALL possible inputs
	virtual std::shared_ptr<Component> create(
		Json::Value definitionJSON,
		GameObject* parent,
		std::string gameObjectName,
		std::string gameObjectType,
		Scene* scene,
		float xMapPos,
		float yMapPos,
		float angleAdjust,
		b2Vec2 sizeOverride,
		const int componentType);

};

