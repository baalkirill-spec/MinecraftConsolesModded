package org.minecraftport.ui;

import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import org.minecraftport.core.MinecraftPortGame;

public final class TitleMenuScreen extends AbstractStageScreen {
    public TitleMenuScreen(MinecraftPortGame game) {
        super(game);
    }

    @Override
    protected void buildUi() {
        root.defaults().pad(8f).width(280f);
        root.add(new Label("MINECRAFT CONSOLES JAVA PORT", game.skin())).row();
        root.add(new Label("Phase 1 skeleton: loop, options, FPS, UI, mods", game.skin())).row();
        root.add(new Label("Detected mods: " + game.modManager().discoveredMods().size(), game.skin())).row();

        root.add(button("Singleplayer", () -> game.setScreen(new PlaceholderScreen(
                game,
                "Singleplayer",
                "World/gameplay port is intentionally not auto-translated yet. This screen marks the future Java gameplay entrypoint.")))).row();
        root.add(button("Multiplayer", () -> game.setScreen(new PlaceholderScreen(
                game,
                "Multiplayer",
                "Network/session code has not been ported in phase 1. The Java scaffold keeps a dedicated entry point for it.")))).row();
        root.add(button("Options", () -> game.setScreen(new OptionsScreen(game)))).row();
        root.add(button("Mods", () -> game.setScreen(new ModsScreen(game)))).row();
        root.add(button("Quit", com.badlogic.gdx.Gdx.app::exit)).row();
    }

    private TextButton button(String text, Runnable action) {
        TextButton button = new TextButton(text, game.skin());
        button.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                action.run();
            }
        });
        return button;
    }
}
