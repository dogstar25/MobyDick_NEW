#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

//Base
#include <iostream>
#include <filesystem>
#include <array>
#include <string>
#include <memory>
#include <fstream>
#include <optional>
#include <bitset>
#include <chrono>
#include <queue>
#include <variant>


//Vendors
//#include "vcpkg_installed/x64-windows/x64-windows/include/json/json.h"
#include <json/json.h>
#include <box2d/box2d.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

//MobyDick
#include "../src/BaseConstants.h"
#include "../src/Blueprint.h"
#include "../src/BrainAABBCallback.h"
#include "../src/Camera.h"
#include "../src/Clock.h"
#include "../src/ColorMap.h"
#include "../src/ComponentFactory.h"
#include "../src/ContactFilter.h"
#include "../src/ContactListener.h"
#include "../src/ContextManager.h"
#include "../src/DebugDraw.h"
#include "../src/DebugPanel.h"
#include "../src/EnumMap.h"
#include "../src/game.h"
#include "../src/GameConfig.h"
#include "../src/GameObject.h"
#include "../src/GameObjectDefinition.h"
#include "../src/GameObjectManager.h"
#include "../src/HudManager.h"
#include "../src/LevelManager.h"
#include "../src/ObjectPoolManager.h"
#include "../src/RayCastCallBack.h"
#include "../src/Renderer.h"
#include "../src/RendererSDL.h"
#include "../src/SceneManager.h"
#include "../src/SoundManager.h"
#include "../src/GameStateManager.h"
#include "../src/actions/Action.h"
#include "../src/actions/ActionFactory.h"
#include "../src/actions/ActorWarpAction.h"
#include "../src/actions/DefaultMoveAction.h"
#include "../src/actions/DefaultOnHoverAction.h"
#include "../src/actions/DefaultOnHoverOutAction.h"
#include "../src/actions/DefaultRotateAction.h"
#include "../src/actions/DroneMoveAction.h"
#include "../src/actions/InteractAction.h"
#include "../src/actions/NoAction.h"
#include "../src/actions/PrimitiveMoveAction.h"
#include "../src/actions/UseAction.h"
#include "../src/actions/DroneMoveAction.h"
#include "../src/actions/DroneMoveAction.h"
#include "../src/actions/OnOffToggleAction.h"
#include "../src/components/ActionComponent.h"
#include "../src/components/AnimationComponent.h"
#include "../src/components/AttachmentsComponent.h"
#include "../src/components/BrainComponent.h"
#include "../src/components/CheckPointComponent.h"
#include "../src/components/ChildrenComponent.h"
#include "../src/components/Component.h"
#include "../src/components/CompositeComponent.h"
#include "../src/components/ContainerComponent.h"
#include "../src/components/EnvironmentComponent.h"
#include "../src/components/HudComponent.h"
#include "../src/components/IMGuiComponent.h"
#include "../src/components/InterfaceComponent.h"
#include "../src/components/InventoryComponent.h"
#include "../src/components/NavigationComponent.h"
#include "../src/components/ParticleComponent.h"
#include "../src/components/ParticleXComponent.h"
#include "../src/components/PhysicsComponent.h"
#include "../src/components/PlayerControlComponent.h"
#include "../src/components/PoolComponent.h"
#include "../src/components/PuzzleComponent.h"
#include "../src/components/RenderComponent.h"
#include "../src/components/SoundComponent.h"
#include "../src/components/StateComponent.h"
#include "../src/components/TransformComponent.h"
#include "../src/components/TextComponent.h"
#include "../src/components/VitalityComponent.h"
#include "../src/components/WeaponComponent.h"
#include "../src/cutScenes/CutScene.h"
#include "../src/cutScenes/CutSceneExample.h"
#include "../src/cutScenes/CutSceneFactory.h"
#include "../src/hud/HudItem.h"
#include "../src/hud/HudItemFactory.h"
#include "../src/hud/HudStatusSeries.h"
#include "../src/hud/HudStatusText.h"
#include "../src/IMGui/IMGuiExample.h"
#include "../src/IMGui/IMGuiFactory.h"
#include "../src/IMGui/IMGuiInteractiveMenuBasic.h"
#include "../src/IMGui/IMGuiItem.h"
#include "../src/IMGui/IMGuiText.h"
#include "../src/IMGui/IMGuiUtil.h"
#include "../src/opengl/DrawBatch.h"
#include "../src/opengl/GLDebugCallback.h"
#include "../src/opengl/GLDrawer.h"
#include "../src/opengl/GLRenderer.h"
#include "../src/opengl/LineDrawBatch.h"
#include "../src/opengl/Shader.h"
//#include "../src/opengl/shaderCode.h"
#include "../src/opengl/SpriteDrawBatch.h"
#include "../src/opengl/Vertex.h"
#include "../src/particleEffects/BaseParticleEffects.h"
#include "../src/particleEffects/ParticleEffectsFactory.h"
#include "../src/triggers/Trigger.h"
#include "../src/puzzles/PuzzleFactory.h"
#include "../src/puzzles/Puzzle.h"
#include "../src/puzzles/UnLockDoorPuzzle.h"
#include "../src/triggers/TriggerFactory.h"
#include "../src/EnvironmentEvents/EnvironmentEventFactory.h"
#include "../src/EnvironmentEvents/NoEvent.h"
#include "../src/texture.h"
#include "../src/TextureManager.h"
#include "../src/Timer.h"
#include "../src/Util.h"

