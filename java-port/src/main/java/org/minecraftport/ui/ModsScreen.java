package org.minecraftport.ui;

import com.badlogic.gdx.scenes.scene2d.ui.Label;
import com.badlogic.gdx.scenes.scene2d.ui.ScrollPane;
import com.badlogic.gdx.scenes.scene2d.ui.Table;
import com.badlogic.gdx.scenes.scene2d.ui.TextButton;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import org.minecraftport.core.MinecraftPortGame;
import org.minecraftport.mods.DiscoveredMod;

public final class ModsScreen extends AbstractStageScreen {
    public ModsScreen(MinecraftPortGame game) {
        super(game);
    }

    @Override
    protected void buildUi() {
        root.defaults().pad(6f);
        root.add(new Label("Mods", game.skin())).row();
        root.add(new Label("Folder: " + game.modManager().modsDirectory(), game.skin())).width(760f).row();

        Table listTable = new Table();
        listTable.top().left().defaults().left().pad(4f);
        rebuildList(listTable);

        ScrollPane scrollPane = new ScrollPane(listTable, game.skin());
        scrollPane.setFadeScrollBars(false);
        root.add(scrollPane).width(820f).height(420f).row();

        Table buttons = new Table();
        TextButton refresh = new TextButton("Refresh Mods", game.skin());
        refresh.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.reloadMods();
                rebuildList(listTable);
            }
        });
        TextButton back = new TextButton("Back", game.skin());
        back.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, com.badlogic.gdx.scenes.scene2d.Actor actor) {
                game.setScreen(new TitleMenuScreen(game));
            }
        });
        buttons.add(refresh).width(220f).padRight(8f);
        buttons.add(back).width(220f);
        root.add(buttons).row();
    }

    private void rebuildList(Table listTable) {
        listTable.clearChildren();
        listTable.add(new Label(
                "Loaded mods: " + game.modManager().discoveredMods().size()
                        + " | data defs: " + game.modManager().totalDataDefinitions(),
                game.skin())).left().row();

        if (game.modManager().discoveredMods().isEmpty()) {
            Label empty = new Label("No mods found yet. Drop folder mods into runtime/mods with mod.json, assets/, or data/.", game.skin());
            empty.setWrap(true);
            listTable.add(empty).width(760f).left().row();
            return;
        }

        for (DiscoveredMod mod : game.modManager().discoveredMods()) {
            StringBuilder builder = new StringBuilder();
            builder.append(mod.displayName())
                    .append(" [").append(mod.sourceType()).append("]\n")
                    .append("id=").append(mod.id())
                    .append(" version=").append(mod.version())
                    .append("\nassets=").append(mod.assetRoots().size())
                    .append(" data=").append(mod.dataDefinitions().size());
            if (!mod.warnings().isEmpty()) {
                builder.append("\nwarnings: ").append(String.join(" | ", mod.warnings()));
            }
            Label label = new Label(builder.toString(), game.skin());
            label.setWrap(true);
            listTable.add(label).width(760f).left().row();
        }
    }
}
