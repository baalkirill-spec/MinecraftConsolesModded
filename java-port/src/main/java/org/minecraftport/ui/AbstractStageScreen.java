package org.minecraftport.ui;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.ScreenAdapter;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.utils.viewport.ScreenViewport;
import org.minecraftport.core.MinecraftPortGame;

public abstract class AbstractStageScreen extends ScreenAdapter {
    protected final MinecraftPortGame game;
    protected final Stage stage;
    protected final Table root;

    protected AbstractStageScreen(MinecraftPortGame game) {
        this.game = game;
        this.stage = new Stage(new ScreenViewport(), game.batch());
        this.root = new Table();
        this.root.setFillParent(true);
        this.stage.addActor(root);
    }

    protected abstract void buildUi();

    @Override
    public void show() {
        Gdx.input.setInputProcessor(stage);
        root.clearChildren();
        buildUi();
    }

    @Override
    public void render(float delta) {
        Gdx.gl.glClearColor(0.02f, 0.03f, 0.05f, 1f);
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
        game.backgroundRenderer().renderGradient(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
        stage.act(delta);
        stage.draw();
        if (game.options().showFpsOverlay()) {
            game.batch().begin();
            game.fpsOverlay().render(game.batch(), game.skin().getFont("default-font"), Gdx.graphics.getWidth(), Gdx.graphics.getHeight());
            game.batch().end();
        }
    }

    @Override
    public void resize(int width, int height) {
        stage.getViewport().update(width, height, true);
    }

    @Override
    public void dispose() {
        stage.dispose();
    }
}
