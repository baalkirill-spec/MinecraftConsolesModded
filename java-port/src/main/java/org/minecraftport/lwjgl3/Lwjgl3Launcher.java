package org.minecraftport.lwjgl3;

import com.badlogic.gdx.backends.lwjgl3.Lwjgl3Application;
import com.badlogic.gdx.backends.lwjgl3.Lwjgl3ApplicationConfiguration;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import org.minecraftport.core.MinecraftPortGame;
import org.minecraftport.mods.ModManager;
import org.minecraftport.options.CoreOptions;
import org.minecraftport.options.WindowPreset;

public final class Lwjgl3Launcher {
    private Lwjgl3Launcher() {
    }

    public static void main(String[] args) {
        Path runtimeRoot = Paths.get("runtime");
        CoreOptions options = CoreOptions.load(runtimeRoot);

        if (Arrays.asList(args).contains("--headless-check")) {
            ModManager manager = new ModManager(runtimeRoot);
            manager.initialize();
            System.out.println("Java port headless check OK");
            System.out.println("Options file: " + options.filePath());
            System.out.println("Mods dir: " + manager.modsDirectory());
            System.out.println("Mods loaded: " + manager.discoveredMods().size());
            System.out.println("FPS overlay enabled: " + options.showFpsOverlay());
            return;
        }

        Lwjgl3ApplicationConfiguration configuration = new Lwjgl3ApplicationConfiguration();
        configuration.setTitle("Minecraft Consoles Java Port");
        configuration.useVsync(true);
        WindowPreset preset = options.windowPreset();
        int width = preset.width() > 0 ? preset.width() : 1280;
        int height = preset.height() > 0 ? preset.height() : 720;
        configuration.setWindowedMode(width, height);
        new Lwjgl3Application(new MinecraftPortGame(runtimeRoot, options), configuration);
    }
}
