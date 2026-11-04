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

### Closing the application

Closing the window calls `onKillDemand` on the GUI thread; its default answer is `exit(0)`
(`GaleContext.stop(code)`). At its next loop the backend stops the periodic thread, calls
`onPause`, `onStop` and `onDestroy` **on the GUI thread with the OpenGL context current**
(release your resources there: their OpenGL objects are deleted at once), closes the resource
manager, disposes the window and ends the process with the exit code. Override
`onKillDemand` to ask a confirmation; call `exit(code)` to end the application from code
(never `System.exit`, which skips `onDestroy`).

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

