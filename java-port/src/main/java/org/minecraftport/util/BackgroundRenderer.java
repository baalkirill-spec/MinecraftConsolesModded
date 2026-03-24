package org.minecraftport.util;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.glutils.ShapeRenderer;
import com.badlogic.gdx.math.Matrix4;

public final class BackgroundRenderer {
    private final ShapeRenderer shapeRenderer = new ShapeRenderer();
    private final Matrix4 projection = new Matrix4();

    public void renderGradient(float width, float height) {
        projection.setToOrtho2D(0f, 0f, width, height);
        shapeRenderer.setProjectionMatrix(projection);
        shapeRenderer.begin(ShapeRenderer.ShapeType.Filled);
        shapeRenderer.rect(0f, 0f, width, height,
                new Color(0.08f, 0.10f, 0.14f, 1f),
                new Color(0.08f, 0.10f, 0.14f, 1f),
                new Color(0.02f, 0.03f, 0.05f, 1f),
                new Color(0.02f, 0.03f, 0.05f, 1f));
        shapeRenderer.end();
    }

    public void dispose() {
        shapeRenderer.dispose();
    }
}
