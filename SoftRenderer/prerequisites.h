#pragma once

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

class Point;
class Vector2;
class Vector3;
class Vector4;
class Matrix4;

class Camera;
class Color;
class Surface;
class Texture;
class GraphicsBuffer;
struct InputElementDesc;
class InputLayout;
struct SamplerStateDesc;
class SamplerState;
class RenderDevice;

class Window;
class App;

#include <string>
#include <vector>
#include <memory>

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Surface> SurfacePtr;
typedef std::shared_ptr<SamplerState> SamplerStatePtr;