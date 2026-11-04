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

## Build / test

Maven module of the multi-module project (see the root `CLAUDE.md` for the build order and commit format):
`. .island/manifest/java-home.sh && /usr/bin/mvn install -DskipTests`.

## Where to find what (`doc/`, read on demand: `cs PATTERN doc/`)

| File | Content |
|------|---------|
| `doc/ARCHITECTURE.md` | Architecture tree, key packages, source file structure |
| `doc/API.md` | Application lifecycle (closing, render and input callbacks, window methods), input system (KeyKeyboard, KeyStatus, KeyType, KeySpecial), OpenGL wrapper (clear, viewport, matrix stack, flags, drawing, shader/program/buffer/texture operations, thread safety) |
| `doc/RESOURCES.md` | Resource system and lifecycle (`create()`/`keep()`/`release()`), ResourceShader, Program, Texture2, TextureFile, VAO, VBO, Colored3DObject |
| `doc/RUNTIME.md` | Thread safety, render loop, frame rate control, URI system, GLSL shader conventions (attributes, uniforms) |
| `doc/EXAMPLES.md` | Complete rendering example |

## Dependencies

| Library | Usage |
|---------|-------|
| `etk` | Core utilities (Uri, Color, Vector2f/3f, Matrix4f, Dimension) |
| `egami` | Image data types + ImageIO load/store (ToolImage) |
| `esvg` | SVG rendering (EsvgDocument) for texture loading |
| `LWJGL` | Native OpenGL, AWT integration |
| `SLF4J` | Logging |

## Common Pitfalls

### OpenGL state management
- **Always restore `blendFunc` after changing it.** Non-standard blending (e.g., additive `GL_SRC_ALPHA, GL_ONE`) leaks into subsequent passes if not reset. Call `OpenGL.blendFuncAuto()` to restore default alpha blending (`GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`).
- **General rule:** any OpenGL state changed during a render pass (`blendFunc`, `depthFunc`, `stencilOp`, `cullFace`, etc.) must be restored before returning. OpenGL is global mutable state.
- **Flags use deferred state management** — changes are batched. Call `OpenGL.updateAllFlags()` to flush pending state changes before drawing.
- Resource lifecycle and thread-safety details: → doc/RESOURCES.md#resource-lifecycle, doc/RUNTIME.md#thread-safety
