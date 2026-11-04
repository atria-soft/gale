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

