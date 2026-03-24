package org.minecraftport.ui;

import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import org.minecraftport.core.MinecraftPortGame;

public final class PlaceholderScreen extends AbstractStageScreen {
    private final String title;
    private final String message;

    public PlaceholderScreen(MinecraftPortGame game, String title, String message) {
        super(game);
        this.title = title;
        this.message = message;
    }

    @Override
    protected void buildUi() {
        root.defaults().pad(10f).width(320f);
        root.add(new Label(title, game.skin())).row();
        Label messageLabel = new Label(message, game.skin());
        messageLabel.setWrap(true);
        root.add(messageLabel).width(480f).row();

        TextButton back = new TextButton("Back", game.skin());
        back.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.setScreen(new TitleMenuScreen(game));
            }
        });
        root.add(back).width(220f);
    }
}
