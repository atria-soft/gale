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

