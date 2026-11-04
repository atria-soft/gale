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
