#pragma once

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

class Point;
class Vector2;
class Vector3;
class Vector4;
class Matrix4;

class Material;
class Light;

class Camera;
class Color;
class Surface;
class Texture;

class IVertexShader;
class IPixelShader;
class ConstantBuffer;

class GraphicsBuffer;
struct InputElementDesc;
class InputLayout;
struct SamplerStateDesc;
class SamplerState;
struct RasterizerDesc;
class RasterizerState;

class StaticMesh;

class RenderDevice;

class Window;
class App;

#include <string>
#include <vector>
#include <memory>
#include <assert.h>

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Surface> SurfacePtr;
typedef std::shared_ptr<SamplerState> SamplerStatePtr;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;
typedef std::shared_ptr<IVertexShader> VertexShaderPtr;
typedef std::shared_ptr<IPixelShader> PixelShaderPtr;
typedef std::shared_ptr<GraphicsBuffer> GraphicsBufferPtr;
typedef std::shared_ptr<InputLayout> InputLayoutPtr;
typedef std::shared_ptr<RasterizerState> RasterizerStatePtr;
typedef std::shared_ptr<StaticMesh> StaticMeshPtr;