package org.minecraftport.core;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import java.nio.file.Path;
import org.minecraftport.assets.UiSkinFactory;
import org.minecraftport.mods.ModManager;
import org.minecraftport.options.CoreOptions;
import org.minecraftport.options.WindowPreset;
import org.minecraftport.rendering.FpsOverlay;
import org.minecraftport.ui.TitleMenuScreen;
import org.minecraftport.util.BackgroundRenderer;

public final class MinecraftPortGame extends Game {
    private final Path runtimeRoot;
    private final CoreOptions options;
    private final ModManager modManager;
    private SpriteBatch batch;
    private Skin skin;
    private FpsOverlay fpsOverlay;
    private BackgroundRenderer backgroundRenderer;

    public MinecraftPortGame(Path runtimeRoot, CoreOptions options) {
        this.runtimeRoot = runtimeRoot;
        this.options = options;
        this.modManager = new ModManager(runtimeRoot);
    }

    @Override
    public void create() {
        batch = new SpriteBatch();
        skin = UiSkinFactory.create();
        fpsOverlay = new FpsOverlay();
        backgroundRenderer = new BackgroundRenderer();
        modManager.initialize();
        setScreen(new TitleMenuScreen(this));
    }

    @Override
    public void render() {
        float delta = Gdx.graphics.getDeltaTime();
        fpsOverlay.update(delta);
        super.render();
    }

    @Override
    public void dispose() {
        super.dispose();
        if (batch != null) {
            batch.dispose();
        }
        if (skin != null) {
            skin.dispose();
        }
        if (backgroundRenderer != null) {
            backgroundRenderer.dispose();
        }
    }

    public SpriteBatch batch() {
        return batch;
    }

    public Skin skin() {
        return skin;
    }

    public CoreOptions options() {
        return options;
    }

    public ModManager modManager() {
        return modManager;
    }

    public FpsOverlay fpsOverlay() {
        return fpsOverlay;
    }

    public BackgroundRenderer backgroundRenderer() {
        return backgroundRenderer;
    }

    public Path runtimeRoot() {
        return runtimeRoot;
    }

    public void reloadMods() {
        modManager.refresh();
    }

    public void toggleFpsOverlay() {
        options.toggleFpsOverlay();
        options.save();
    }

    public void cycleWindowPreset() {
        options.cycleWindowPreset();
        WindowPreset preset = options.windowPreset();
        if (preset.width() > 0 && preset.height() > 0) {
            Gdx.graphics.setWindowedMode(preset.width(), preset.height());
        }
        options.save();
    }
}
