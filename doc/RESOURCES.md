## Resource System

All GPU resources extend `Resource` and are managed by `ResourceManager`. The system handles:

- **Automatic context management:** Resources are uploaded to the GPU asynchronously when the OpenGL context is available
- **Reference counting:** `keep()` / `release()` for shared resources
- **Named deduplication:** Resources with the same name are shared (factory methods check existing resources first)
- **Priority levels:** Resources have levels (0-4) controlling update order. Shaders (level 0) update before programs (level 1), which update before textures/buffers (level 3)
- **Context loss handling:** Automatic re-upload when OpenGL context is recreated

### Resource Lifecycle

```
create() → [constructor registers it, count 1] → [deferred] updateContext() → use → release()
                                    ↑                                                  ↓ count 0
                     flush() → manager.update(resource)          leaves the manager, cleanUp() deletes its GL objects
```

Every resource follows the pattern:
1. **Factory method** (`create(...)`) looks for a living resource of the same name with
   `keepExisting(name, Type.class)`: found → it is kept (count + 1) and returned; another type → `IllegalStateException`
2. If not found, create new instance (the constructor registers it once, count 1; never call `localAdd` again)
3. If OpenGL context is available, call `updateContext()` immediately
4. If not, call `manager.update(this)` to defer until context is ready

**Releasing:** call `release()` once per `create()`/`keep()`, from any thread. At count 0 the
resource leaves the manager (a later `create` of the same name builds a new one), queued
updates are ignored, and `cleanUp()` deletes its OpenGL objects: at once on the OpenGL thread,
at the next `updateContext()` otherwise. A released resource must not be used any more
(`isReleased()`); a second `release()` is logged and ignored. Do not delete a resource's GL ids
yourself (`glDeleteVertexArrays`...): `release()` does it, and `removeContext()` forgets the ids.
To delete raw GL objects of your own from a thread without context, use
`ResourceManager.runOnGlThread(Runnable)`. The manager logs (DEBUG) the number of living
resources when it changes: a number that grows after each level/match is a missing `release()`.

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

