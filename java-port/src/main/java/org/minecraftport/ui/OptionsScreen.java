package org.minecraftport.ui;

import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import org.minecraftport.core.MinecraftPortGame;

public final class OptionsScreen extends AbstractStageScreen {
    public OptionsScreen(MinecraftPortGame game) {
        super(game);
    }

    @Override
    protected void buildUi() {
        root.defaults().pad(8f).width(300f);
        root.add(new Label("Options", game.skin())).row();
        root.add(new Label("Java-port phase 1: settings + FPS + window size", game.skin())).row();

        TextButton fpsButton = new TextButton(game.options().fpsToggleLabel(), game.skin());
        fpsButton.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.toggleFpsOverlay();
                fpsButton.setText(game.options().fpsToggleLabel());
            }
        });
        root.add(fpsButton).row();

        TextButton windowButton = new TextButton(game.options().windowPresetLabel(), game.skin());
        windowButton.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.cycleWindowPreset();
                windowButton.setText(game.options().windowPresetLabel());
            }
        });
        root.add(windowButton).row();

        TextButton back = new TextButton("Back", game.skin());
        back.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.setScreen(new TitleMenuScreen(game));
            }
        });
        root.add(back);
    }
}
