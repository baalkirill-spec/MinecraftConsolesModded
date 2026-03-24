package org.minecraftport.rendering;

import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.graphics.g2d.GlyphLayout;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public final class FpsOverlay {
    private final GlyphLayout layout = new GlyphLayout();
    private String label = "0 FPS | 0.00 ms";
    private float elapsed;
    private int frames;

    public void update(float deltaSeconds) {
        elapsed += deltaSeconds;
        frames++;
        if (elapsed >= 0.25f) {
            int fps = Math.max(1, Math.round(frames / elapsed));
            float frameMs = (elapsed * 1000f) / frames;
            label = String.format("%d FPS | %.2f ms", fps, frameMs);
            elapsed = 0f;
            frames = 0;
        }
    }

    public void render(SpriteBatch batch, BitmapFont font, float viewportWidth, float viewportHeight) {
        layout.setText(font, label);
        float x = viewportWidth - layout.width - 12f;
        float y = viewportHeight - 12f;
        font.draw(batch, label, x, y);
    }

    public String label() {
        return label;
    }
}
