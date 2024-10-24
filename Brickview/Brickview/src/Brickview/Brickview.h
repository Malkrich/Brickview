#pragma once

// Core
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Application.h"
#include "Core/Event/Event.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/Layer/Layer.h"
#include "Core/Time.h"
#include "Core/Window.h"

// Render
#include "Renderer/Renderer.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/PerspectiveCamera.h"
#include "Renderer/CameraController.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

// Scene
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

// Lego
#include "Lego/LegoPart.h"
#include "Lego/LegoMeshRegistry.h"

// Metric
#include "Metric/World.h"

// Utils
#include "Utils/StringUtils.h"