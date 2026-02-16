# Gale - Graphic Abstraction Layer for Ewol

Gale is the low-level windowing and OpenGL rendering layer used by Ewol. It provides application lifecycle management, input event handling, OpenGL wrappers, and a resource management system for GPU objects (shaders, textures, buffers).

**Backend:** LWJGL (Lightweight Java Game Library) with AWT integration.

## Quick Start

### Minimal Application

```java
import org.atriasoft.etk.Uri;
import org.atriasoft.gale.Gale;

public class MyApp {
    public static void main(String[] args) {
        Gale.init();
        // Register resource path for DATA:// URIs
        Uri.setApplication(MyApp.class, "/com/example/myapp/");
        Gale.run(new MyAppApplication(), args);
    }
}
```

### Application Class

```java
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.context.GaleContext;
import org.atriasoft.gale.backend3d.OpenGL;

public class MyAppApplication extends GaleApplication {

    @Override
    public void onCreate(GaleContext context) {
        // Initialize resources (shaders, textures, VBOs)
    }

    @Override
    public void onDraw(GaleContext context) {
        // Render frame
        OpenGL.setViewPort(Vector2f.ZERO, getSize());
        OpenGL.clearColor(Color.BLACK);
        OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);
        // ... draw ...
        markDrawingIsNeeded(); // Request next frame
    }

    @Override
    public void onDestroy(GaleContext context) {
        // Clean up resources
    }
}
```

## Architecture

```
Gale (entry point)
 └─ GaleContext (abstract context, render loop, resource management)
     ├─ ContextLWJGLAWT (concrete backend implementation)
     ├─ ResourceManager (GPU resource lifecycle)
     ├─ MessageSystem (thread-safe async/GUI message queue)
     └─ PeriodicThread (periodic callback timer)
         └─ GaleApplication (user application callbacks)
             ├─ Lifecycle: onCreate → onStart → onResume → onPause → onStop → onDestroy
             ├─ Rendering: onDraw, onRegenerateDisplay
             └─ Input: onKeyboard, onPointer
```

## Key Packages

| Package | Description |
|---------|-------------|
| `org.atriasoft.gale` | Entry point (`Gale`), application base (`GaleApplication`), enums |
| `org.atriasoft.gale.context` | Context management (`GaleContext`), backend implementations |
| `org.atriasoft.gale.backend3d` | OpenGL wrapper (`OpenGL` static class) |
| `org.atriasoft.gale.resource` | GPU resource management (textures, shaders, programs, VBOs, VAOs) |
| `org.atriasoft.gale.key` | Input event types (keyboard, mouse, pointer) |
| `org.atriasoft.gale.tools` | Image loading utilities |

## Application Lifecycle

`GaleApplication` provides these lifecycle callbacks (all receive `GaleContext context`):

| Callback | When |
|----------|------|
| `onCreate` | Application creation - initialize resources here |
| `onStart` | Application is starting |
| `onResume` | Application is becoming active |
| `onPause` | Application is being paused |
| `onStop` | Application is stopping |
| `onDestroy` | Application is being destroyed - clean up resources here |
| `onKillDemand` | System requested application exit |

### Rendering Callbacks

| Callback | When |
|----------|------|
| `onDraw(context)` | Frame rendering requested |
| `onRegenerateDisplay(context)` | Display needs regeneration |
| `onResize(context, size)` | Window resized (receives `Vector2f size`) |

### Input Event Callbacks

| Callback | Parameters |
|----------|------------|
| `onKeyboard(special, type, value, state)` | `KeySpecial`, `KeyKeyboard`, `Character`, `KeyStatus` |
| `onPointer(special, type, pointerID, pos, state)` | `KeySpecial`, `KeyType`, `int`, `Vector2f`, `KeyStatus` |
| `onClipboardEvent(clipboardID)` | Clipboard event |

### Window Management Methods

| Method | Description |
|--------|-------------|
| `setSize(Vector2f)` | Set window size |
| `getSize()` | Get current window size |
| `setTitle(String)` | Set window title |
| `getAspectRatio()` | Get width/height ratio |
| `setCursor(CursorDisplay)` | Change cursor appearance |
| `markDrawingIsNeeded()` | Request next frame redraw |
| `exit()` | Exit the application |

## Input System

### KeyKeyboard (keyboard keys)

Common values: `CHARACTER`, `LEFT`, `RIGHT`, `UP`, `DOWN`, `PAGE_UP`, `PAGE_DOWN`, `START`, `END`, `INSERT`, `F1`-`F12`, `SHIFT_LEFT`, `SHIFT_RIGHT`, `CTRL_LEFT`, `CTRL_RIGHT`, `ALT_LEFT`, `ALT_RIGHT`, `META_LEFT`, `META_RIGHT`, `CAP_LOCK`, `NUM_LOCK`, `BACK`, `HOME`, `MENU`

### KeyStatus (event states)

| Status | Description |
|--------|-------------|
| `down` | Key/button pressed |
| `downRepeat` | Key held (repeat) |
| `up` | Key/button released |
| `pressSingle` | Single click/press |
| `pressDouble` | Double click/press |
| `pressTriple` | Triple click/press |
| `move` | Pointer moved |
| `enter` | Pointer entered widget |
| `leave` | Pointer left widget |
| `abort` | Event transferred away from widget |
| `transfer` | Event received by transfer |

### KeyType (input device)

Values: `mouse`, `finger`, `stylet`

### KeySpecial (modifier state)

Tracks modifier keys state. Convenience methods: `getShift()`, `getCtrl()`, `getAlt()`, `getMeta()`, `getAltGr()`, `getCapsLock()`, `getNumLock()`, `getInsert()`.

## OpenGL Wrapper

The `OpenGL` class (`org.atriasoft.gale.backend3d.OpenGL`) is a **static utility class** that wraps all LWJGL OpenGL calls. All methods are static.

### Clearing & Background

```java
OpenGL.clearColor(Color.BLACK);                              // Set background color
OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);        // Clear color buffer
OpenGL.clear(OpenGL.ClearFlag.clearFlag_depthBuffer);        // Clear depth buffer
```

**ClearFlag values:** `clearFlag_colorBuffer`, `clearFlag_depthBuffer`, `clearFlag_stencilBuffer`

### Viewport

```java
OpenGL.setViewPort(Vector2f.ZERO, windowSize);               // Set viewport (origin, size)
```

### Matrix Stack

```java
OpenGL.setBasicMatrix(Matrix4f.IDENTITY);                    // Reset matrix stack
OpenGL.push();                                                // Push matrix
OpenGL.pop();                                                 // Pop matrix
OpenGL.setMatrix(projectionMatrix);                           // Set current matrix
Matrix4f matrix = OpenGL.getMatrix();                         // Get current projection matrix
Matrix4f camera = OpenGL.getCameraMatrix();                   // Get camera/view matrix
```

### OpenGL Flags

```java
OpenGL.enable(OpenGL.Flag.flag_blend);                       // Enable blending
OpenGL.disable(OpenGL.Flag.flag_blend);                      // Disable blending
OpenGL.enable(OpenGL.Flag.flag_depthTest);                   // Enable depth testing
OpenGL.enable(OpenGL.Flag.flag_cullFace);                    // Enable face culling
```

**Important flags:** `flag_blend`, `flag_depthTest`, `flag_cullFace`, `flag_back`, `flag_front`, `flag_alphaTest`, `flag_texture2D`, `flag_lineSmooth`, `flag_stencilTest`, `flag_scissorTest`

Note: Flags use **deferred state management** - changes are batched and applied efficiently.

### Drawing

```java
OpenGL.drawArrays(OpenGL.RenderMode.TRIANGLE, startIndex, count);
OpenGL.drawElements(OpenGL.RenderMode.TRIANGLE, count);
```

**RenderMode values:** `POINT`, `LINE`, `LINE_STRIP`, `LINE_LOOP`, `TRIANGLE`, `TRIANGLE_STRIP`, `TRIANGLE_FAN`, `QUAD`, `QUAD_STRIP`

### Shader Operations

```java
int shaderId = OpenGL.shaderLoad(uri, ShaderType.VERTEX);    // Load shader from URI
OpenGL.shaderRemove(shaderId);                                // Delete shader

// ShaderType: VERTEX, FRAGMENT
```

### Program Operations

```java
int programId = OpenGL.programCreate();                       // Create program
OpenGL.programAttach(programId, shaderId);                    // Attach shader
OpenGL.programCompile(programId);                             // Link program (returns boolean)
OpenGL.programUse(programId);                                 // Activate program
OpenGL.programUse(-1);                                        // Deactivate

// Attribute/Uniform locations
int loc = OpenGL.programGetAttributeLocation(programId, "name");
int loc = OpenGL.programGetUniformLocation(programId, "name");
OpenGL.programBindAttribute(programId, index, "name");

// Load uniforms
OpenGL.programLoadUniformFloat(location, value);
OpenGL.programLoadUniformInt(location, value);
OpenGL.programLoadUniformMatrix(location, matrix, transpose);
OpenGL.programLoadUniformColor(location, color);
OpenGL.programLoadUniformVector(location, vector);
```

### Buffer Operations

```java
int bufferId = OpenGL.genBuffers();                           // Generate buffer
OpenGL.genBuffers(int[] bufferArray);                         // Generate multiple buffers
OpenGL.deleteBuffers(int[] bufferArray);                      // Delete buffers
OpenGL.bindBuffer(bufferId);                                  // Bind buffer
OpenGL.unbindBuffer();                                        // Unbind

// Upload data to buffer
OpenGL.bufferData(float[] data, Usage.staticDraw);
OpenGL.bufferData(Vector2f[] data, Usage.staticDraw);
OpenGL.bufferData(Vector3f[] data, Usage.staticDraw);
OpenGL.bufferData(Color[] data, Usage.staticDraw);
OpenGL.bufferData(int[] data, Usage.staticDraw);

// Usage: staticDraw, dynamicDraw, streamDraw
```

### Texture Operations

```java
int texId = OpenGL.glGenTextures();                           // Generate texture
OpenGL.glDeleteTextures(texId);                               // Delete texture
OpenGL.activeTexture(unit);                                   // Activate texture unit (0-31)
OpenGL.bindTexture2D(texId);                                  // Bind texture

// Upload texture data
OpenGL.glTexImage2D(level, internalFormat, width, height, border, format, type, buffer);
OpenGL.glTexSubImage2D(level, xOffset, yOffset, width, height, format, type, buffer);

// Texture filtering
OpenGL.setTexture2DFilterLinear();                            // GL_LINEAR
OpenGL.setTexture2DFilterNearest();                           // GL_NEAREST

// Texture wrapping
OpenGL.setTexture2DWrapClampToEdge();                         // Clamp
OpenGL.setTexture2DWrapRepeat();                              // Repeat
```

### Thread Safety

```java
OpenGL.hasContext();                  // Check if current thread has OpenGL context
OpenGL.lock();                        // Lock OpenGL context
OpenGL.unLock();                      // Unlock OpenGL context
```

## Resource System

All GPU resources extend `Resource` and are managed by `ResourceManager`. The system handles:

- **Automatic context management:** Resources are uploaded to the GPU asynchronously when the OpenGL context is available
- **Reference counting:** `keep()` / `release()` for shared resources
- **Named deduplication:** Resources with the same name are shared (factory methods check existing resources first)
- **Priority levels:** Resources have levels (0-4) controlling update order. Shaders (level 0) update before programs (level 1), which update before textures/buffers (level 3)
- **Context loss handling:** Automatic re-upload when OpenGL context is recreated

### Resource Lifecycle

```
create() → localAdd(manager) → [deferred] updateContext() → use → cleanUp()
                                    ↑
                     flush() → manager.update(resource)
```

Every resource follows the pattern:
1. **Factory method** (`create(...)`) checks if resource already exists by name in `ResourceManager`
2. If found, increment reference count and return existing instance
3. If not found, create new instance (constructor calls `localAdd` to register with manager)
4. If OpenGL context is available, call `updateContext()` immediately
5. If not, call `manager.update(this)` to defer until context is ready

### ResourceShader

Represents a GLSL shader (vertex or fragment). Type is auto-detected from file extension.

```java
// Shaders are usually created indirectly via ResourceProgram
// Extension determines type: .vert → VERTEX, .frag → FRAGMENT
ResourceShader shader = ResourceShader.create(new Uri("DATA", "my_shader.vert"));
int glId = shader.getGLID();
ShaderType type = shader.getShaderType();
```

### ResourceProgram

Represents a compiled shader program (vertex + fragment shader pair). Provides a high-level API for attributes and uniforms.

```java
// Create program from vertex and fragment shader URIs
ResourceProgram program = ResourceProgram.create(
    new Uri("DATA", "basic.vert"),
    new Uri("DATA", "basic.frag")
);

// Get attribute/uniform handles (IDs are stable across reloads)
int positionAttr = program.getAttribute("in_position");
int colorUniform = program.getUniform("in_colors");
int matrixUniform = program.getUniform("in_matrixProjection");

// Rendering
program.use();
program.uniformMatrix(matrixUniform, projectionMatrix);     // Send matrix
program.uniformColor(colorUniform, Color.RED);               // Send color (RGBA)
program.uniformColorRGB(colorUniform, Color.RED);            // Send color (RGB only)
program.uniformFloat(uniform, 1.0f);                         // Send 1-4 floats
program.uniformInt(uniform, 42);                              // Send 1-4 ints
program.uniformVector(uniform, new Vector2f(1, 2));           // Send vector (2f/2i/3f/3i/4f)
program.sendAttribute(positionAttr, 3, floatBuffer, 3);       // Send vertex attribute
program.sendAttributePointer(attr, vbo, index);               // Send VBO attribute pointer
program.setTexture0(texUniform, textureGlId);                 // Bind texture unit 0
program.setTexture1(texUniform, textureGlId);                 // Bind texture unit 1
program.unUse();
```

### ResourceTexture2 (preferred texture resource)

Modern texture resource. Can be created dynamically or from URI.

```java
// From file (via ToolImage + EsvgDocument)
ResourceTextureFile tex = ResourceTextureFile.create(new Uri("DATA", "image.png"));

// Dynamic texture
ResourceTexture2 tex = ResourceTexture2.create();
tex.set(imageByteRGBA);                                       // Set image data
tex.setFilterMode(TextureFilter.LINEAR);                       // LINEAR or NEAREST
tex.setRepeat(true);                                           // Repeat wrapping

// Rendering
tex.bindForRendering(0);                                       // Bind to texture unit
// ... draw ...
tex.unBindForRendering();

// Properties
int glId = tex.getRendererId();                                // OpenGL texture ID
Vector2i size = tex.getOpenGlSize();                           // Texture dimensions
Vector2i realSize = tex.getUsableSize();                       // Actual image dimensions
```

### ResourceTextureFile

Extends `ResourceTexture2`. Loads image files via `egami/ToolImage` + `esvg/EsvgDocument` (supports PNG, JPEG, BMP, GIF, SVG with auto-resize to power-of-2).

```java
// Auto-detect size
ResourceTextureFile tex = ResourceTextureFile.create(new Uri("DATA", "icon.png"));

// SVG with explicit size (will be rounded up to next power of 2)
ResourceTextureFile svg = ResourceTextureFile.create(
    new Uri("DATA", "icon.svg"),
    new Vector2i(64, 64)
);
```

### ResourceVirtualArrayObject (VAO)

Wraps OpenGL VAO with VBOs for positions, texture coordinates, normals, colors, and indices.

```java
// Static VAO with all data
ResourceVirtualArrayObject vao = ResourceVirtualArrayObject.create(
    positions,           // float[] (3 components per vertex)
    colors,              // float[] (4 components RGBA)
    textureCoordinates,  // float[] (2 components UV)
    normals,             // float[] (3 components)
    indices              // int[]
);

// Dynamic VAO (for data that changes every frame)
ResourceVirtualArrayObject vao = ResourceVirtualArrayObject.createDynamic();
vao.setPosition(float[] or List<Vector3f>);
vao.setColors(float[] or List<Color>);
vao.setTextureCoordinate(float[] or List<Vector2f>);
vao.setNormals(float[] or List<Vector3f>);
vao.setIndices(int[] or List<Integer>);
vao.setVertexCount(count);
vao.flush();                                                   // Send to GPU

// Rendering
vao.bindForRendering();
vao.render(OpenGL.RenderMode.TRIANGLE);                        // Indexed rendering
vao.renderArrays(OpenGL.RenderMode.TRIANGLE);                  // Non-indexed rendering
vao.render(OpenGL.RenderMode.TRIANGLE, start, stop);           // Range rendering
vao.unBindForRendering();

// Clear and reuse (for dynamic VAOs)
vao.clear();
```

**Standard VBO indices (used in shaders):**

| Constant | Value | Shader Variable | Components |
|----------|-------|-----------------|------------|
| `INDICE_VBO_POSITIONS` | 0 | `in_position` | 3 (xyz) |
| `INDICE_VBO_TEXTURE_COORDINATES` | 1 | `in_extureCoords` | 2 (uv) |
| `INDICE_VBO_NORMALS` | 2 | `in_normal` | 3 (xyz) |
| `INDICE_VBO_COLORS` | 3 | `in_colors` | 4 (rgba) |

### ResourceVirtualBufferObject (VBO)

Lower-level VBO wrapper for custom buffer configurations.

```java
// Create with N buffer slots
ResourceVirtualBufferObject vbo = ResourceVirtualBufferObject.create(3);

// Set data for each slot (supports float[], int[], Vector2f[], Vector3f[], Color[])
vbo.setVboData(0, positionData);     // float[]
vbo.setVboData(1, colorData);        // Color[]
vbo.setVboData(2, texCoordData);     // Vector2f[]
vbo.flush();                          // Send to GPU

// Use with ResourceProgram
program.sendAttributePointer(attrId, vbo, slotIndex);
// or
program.bindVBO(attrId, vbo, slotIndex);

// Properties
int glId = vbo.getOpenGlId(slotIndex);
int elementSize = vbo.getElementSize(slotIndex);  // 1=float, 2=vec2, 3=vec3, 4=color
int bufSize = vbo.bufferSize(slotIndex);
```

### ResourceColored3DObject

Convenience resource for drawing simple 3D primitives with a single color. Uses the built-in `simple3D.vert`/`simple3D.frag` shaders.

```java
ResourceColored3DObject obj = ResourceColored3DObject.create();

// Draw primitives
obj.draw(vertices, color, updateDepthBuffer, depthTest);
obj.draw(vertices, color, transformMatrix, updateDepthBuffer, depthTest);
obj.drawLine(vertices, color, transformMatrix, updateDepthBuffer, depthTest);
obj.drawSphere(radius, lats, longs, transformMatrix, color);
obj.drawCylinder(radius, size, lats, longs, transformMatrix, color);
obj.drawCapsule(radius, size, lats, longs, transformMatrix, color);
obj.drawCone(radius, size, lats, longs, transformMatrix, color);
obj.drawSquare(size, transformMatrix, color);
obj.drawTriangle(p1, p2, p3, transformMatrix, color);
obj.drawTriangles(vertices, indices, transformMatrix, color);
obj.drawCubeLine(min, max, color, transformMatrix, updateDepthBuffer, depthTest);
```

## Thread Safety

Gale operates with multiple threads. Use these patterns:

```java
// From GaleContext - execute on GUI thread
context.postActionToGui(() -> { /* runs on GUI thread */ });

// Execute async action
context.postActionAsync(() -> { /* runs asynchronously */ });
```

### Render Loop

The main loop (`ContextLWJGLAWT.run()`) uses a frame-rate-limited event loop:

1. **Process events** — drain all pending GUI events (`processEventsGui()`)
2. **Render** — `canvas.render()` triggers `paintGL()` which calls `operatingSystemDraw(false)` (only draws if `needRedraw` flag is set) and `swapBuffers()`
3. **Process post-render events** — handle events that arrived during rendering
4. **Sleep** — wait for remaining frame budget to maintain target FPS

The `operatingSystemDraw()` call internally:
1. Processes async message queue
2. Locks OpenGL context
3. Updates resources via `ResourceManager.updateContext()`
4. Calls `application.onDraw()`
5. Flushes OpenGL
6. Cleans removed resources
7. Unlocks OpenGL context

### Frame Rate Control

```java
GaleContext context = ...;
context.setTargetFps(60);     // default: 60 FPS
context.setTargetFps(120);    // for high refresh rate
int fps = context.getTargetFps();
long frameMs = context.getTargetFrameTimeMs();  // e.g. 16ms for 60 FPS
```

The frame rate limiter uses `Thread.sleep()` for the remaining frame budget. Events are processed in batches before each frame, preventing event accumulation during drag operations.

## URI System

Resources are loaded using `Uri` with named groups:

```java
// Set application resource root (for DATA:// scheme)
Uri.setApplication(MyApp.class, "/com/example/myapp/");

// Create URIs
new Uri("DATA", "basic.vert")              // → classpath resource at app root
new Uri("DATA", "basic.frag", "gale")      // → classpath resource in "gale" library
```

The "gale" URI library is automatically registered by `Gale.init()`.

## GLSL Shader Conventions

Shaders use GLSL version 400 core with GL_ES compatibility guards:

```glsl
#version 400 core

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif
```

### Standard Attribute Layout

```glsl
layout (location = 0) in vec3 in_position;       // Vertex position
layout (location = 1) in vec2 in_extureCoords;    // Texture coordinates
layout (location = 2) in vec3 in_normal;           // Normal vector
layout (location = 3) in vec4 in_colors;           // Vertex color
```

### Common Uniforms

```glsl
uniform mat4 in_matrixTransformation;              // Model matrix
uniform mat4 in_matrixProjection;                   // Projection matrix
uniform mat4 in_matrixView;                         // View/camera matrix
uniform vec4 in_colors;                             // Uniform color
```

## Complete Rendering Example

```java
public class MyAppApplication extends GaleApplication {
    private ResourceProgram program;
    private ResourceVirtualArrayObject vao;
    private int matTransform, matProjection, matView;

    @Override
    public void onCreate(GaleContext context) {
        // Load shaders
        program = ResourceProgram.create(
            new Uri("DATA", "basic.vert"),
            new Uri("DATA", "basic.frag")
        );
        matTransform = program.getUniform("in_matrixTransformation");
        matProjection = program.getUniform("in_matrixProjection");
        matView = program.getUniform("in_matrixView");

        // Create geometry
        float[] vertices = { -0.5f, -0.5f, -1.0f,  0.0f, 0.5f, -1.0f,  0.5f, -0.5f, -1.0f };
        float[] colors = { 1, 0, 0, 1,  0, 1, 0, 1,  0, 0, 1, 1 };
        int[] indices = { 0, 1, 2 };
        vao = ResourceVirtualArrayObject.create(vertices, colors, indices);
        vao.setName("my_triangle");
        vao.flush();
    }

    @Override
    public void onDraw(GaleContext context) {
        Vector2f size = getSize();
        OpenGL.setViewPort(Vector2f.ZERO, size);
        OpenGL.setBasicMatrix(Matrix4f.IDENTITY);
        OpenGL.clearColor(Color.BLACK);
        OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);

        OpenGL.push();
        Matrix4f projection = Matrix4f.createMatrixOrtho(
            -getAspectRatio(), getAspectRatio(), -1, 1, -50, 50
        );
        OpenGL.setMatrix(projection);

        program.use();
        vao.bindForRendering();
        program.uniformMatrix(matView, OpenGL.getCameraMatrix());
        program.uniformMatrix(matProjection, projection);
        program.uniformMatrix(matTransform, Matrix4f.IDENTITY);
        vao.render(OpenGL.RenderMode.TRIANGLE);
        vao.unBindForRendering();
        program.unUse();

        OpenGL.pop();
        markDrawingIsNeeded();
    }
}
```

## Common Pitfalls

### OpenGL state management
- **Always restore `blendFunc` after changing it.** Non-standard blending (e.g., additive `GL_SRC_ALPHA, GL_ONE`) leaks into subsequent passes if not reset. Call `OpenGL.blendFuncAuto()` to restore default alpha blending (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`).
- **General rule:** any OpenGL state changed during a render pass (`blendFunc`, `depthFunc`, `stencilOp`, `cullFace`, etc.) must be restored before returning. OpenGL is global mutable state.
- **Flags use deferred state management** — changes are batched. Call `OpenGL.updateAllFlags()` to flush pending state changes before drawing.

## File Structure

```
gale/
├── src/main/org/atriasoft/gale/
│   ├── Gale.java                  # Entry point: init(), run()
│   ├── GaleApplication.java       # Application base class (lifecycle + events)
│   ├── TextureFilter.java         # Enum: LINEAR, NEAREST
│   ├── backend3d/
│   │   └── OpenGL.java            # Static OpenGL wrapper (all GL calls)
│   ├── context/
│   │   ├── GaleContext.java        # Abstract context (render loop, resources)
│   │   └── LWJG_AWT/
│   │       └── ContextLWJGLAWT.java # LWJGL+AWT backend implementation
│   ├── key/
│   │   ├── KeyKeyboard.java        # Keyboard key enum
│   │   ├── KeyStatus.java          # Input event status enum
│   │   ├── KeyType.java            # Input device type (mouse/finger/stylet)
│   │   └── KeySpecial.java         # Modifier key state tracker
│   ├── resource/
│   │   ├── Resource.java                      # Base resource (uid, name, level, refcount)
│   │   ├── ResourceManager.java               # Resource lifecycle manager
│   │   ├── ResourceShader.java                # GLSL shader (.vert/.frag)
│   │   ├── ResourceProgram.java               # Shader program (vertex+fragment)
│   │   ├── ResourceTexture.java               # Texture resource (deprecated)
│   │   ├── ResourceTexture2.java              # Modern texture resource
│   │   ├── ResourceTextureFile.java           # File-based texture (PNG, SVG)
│   │   ├── ResourceVirtualArrayObject.java    # VAO wrapper
│   │   ├── ResourceVirtualBufferObject.java   # VBO wrapper
│   │   └── ResourceColored3DObject.java       # 3D primitive drawing helper
│   ├── tools/
│   │   ├── ImageLoader.java        # Image decoding via javax.imageio
│   │   └── ImageRawData.java       # Raw image data container
│   └── test/
│       ├── sample1/                # Triangle rendering sample
│       └── sample2/                # Advanced sample
└── src/resources/resources/gale/data/
    ├── simple3D.vert               # Built-in 3D vertex shader
    └── simple3D.frag               # Built-in 3D fragment shader
```

## Dependencies

| Library | Usage |
|---------|-------|
| `etk` | Core utilities (Uri, Color, Vector2f/3f, Matrix4f, Dimension) |
| `egami` | Image data types + ImageIO load/store (ToolImage) |
| `esvg` | SVG rendering (EsvgDocument) for texture loading |
| `LWJGL` | Native OpenGL, AWT integration |
| `SLF4J` | Logging |
