package org.minecraftport.assets;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Color;
import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.graphics.g2d.BitmapFont;
import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.ScrollPane;
import com.badlogic.gdx.scenes.scene2d.ui.Skin;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.Drawable;
import com.badlogic.gdx.scenes.scene2d.utils.TextureRegionDrawable;

public final class UiSkinFactory {
    private UiSkinFactory() {
    }

    public static Skin create() {
        Skin skin = new Skin();
        BitmapFont font = new BitmapFont();
        font.getData().markupEnabled = true;
        skin.add("default-font", font);

        Pixmap pixmap = new Pixmap(1, 1, Pixmap.Format.RGBA8888);
        pixmap.setColor(Color.WHITE);
        pixmap.fill();
        Texture white = new Texture(pixmap);
        pixmap.dispose();
        skin.add("white", white);

        TextButton.TextButtonStyle buttonStyle = new TextButton.TextButtonStyle();
        buttonStyle.font = font;
        buttonStyle.fontColor = Color.WHITE;
        buttonStyle.up = tint(skin, new Color(0.18f, 0.20f, 0.24f, 1f));
        buttonStyle.over = tint(skin, new Color(0.30f, 0.34f, 0.40f, 1f));
        buttonStyle.down = tint(skin, new Color(0.12f, 0.14f, 0.18f, 1f));
        skin.add("default", buttonStyle);

        Label.LabelStyle labelStyle = new Label.LabelStyle(font, Color.WHITE);
        skin.add("default", labelStyle);

        ScrollPane.ScrollPaneStyle scrollPaneStyle = new ScrollPane.ScrollPaneStyle();
        scrollPaneStyle.background = tinted(white, new Color(0.08f, 0.09f, 0.12f, 0.95f));
        scrollPaneStyle.vScroll = tinted(white, new Color(0.18f, 0.20f, 0.24f, 1f));
        scrollPaneStyle.vScrollKnob = tinted(white, new Color(0.30f, 0.34f, 0.40f, 1f));
        skin.add("default", scrollPaneStyle);
        return skin;
    }

    private static Drawable tint(Skin skin, Color color) {
        return tinted(skin.get("white", Texture.class), color);
    }

    private static Drawable tinted(Texture texture, Color color) {
        return new TextureRegionDrawable(new TextureRegion(texture)).tint(color);
    }
}
