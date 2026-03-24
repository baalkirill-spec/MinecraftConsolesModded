package org.minecraftport.options;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Properties;

public final class CoreOptions {
    private static final String KEY_SHOW_FPS = "showFpsOverlay";
    private static final String KEY_WINDOW_PRESET = "windowPreset";
    private static final String KEY_CUSTOM_SKIN_PATH = "customSkinPath";
    private static final String KEY_RENDER_DISTANCE = "renderDistance";
    private static final String KEY_FANCY_GRAPHICS = "fancyGraphics";

    private final Path filePath;
    private boolean showFpsOverlay = true;
    private WindowPreset windowPreset = WindowPreset.HD_720;
    private String customSkinPath = "custom_skin.png";
    private int renderDistance = 8;
    private boolean fancyGraphics = true;

    private CoreOptions(Path filePath) {
        this.filePath = filePath;
    }

    public static CoreOptions load(Path runtimeRoot) {
        Path filePath = runtimeRoot.resolve("options.properties");
        CoreOptions options = new CoreOptions(filePath);
        options.load();
        return options;
    }

    public void load() {
        try {
            Files.createDirectories(filePath.getParent());
            if (!Files.exists(filePath)) {
                save();
                return;
            }

            Properties properties = new Properties();
            try (InputStream input = Files.newInputStream(filePath)) {
                properties.load(input);
            }

            showFpsOverlay = Boolean.parseBoolean(properties.getProperty(KEY_SHOW_FPS, Boolean.toString(showFpsOverlay)));
            windowPreset = WindowPreset.fromName(properties.getProperty(KEY_WINDOW_PRESET, windowPreset.name()));
            customSkinPath = properties.getProperty(KEY_CUSTOM_SKIN_PATH, customSkinPath);
            renderDistance = Integer.parseInt(properties.getProperty(KEY_RENDER_DISTANCE, Integer.toString(renderDistance)));
            fancyGraphics = Boolean.parseBoolean(properties.getProperty(KEY_FANCY_GRAPHICS, Boolean.toString(fancyGraphics)));
        } catch (IOException | NumberFormatException exception) {
            throw new IllegalStateException("Failed to load Java port options from " + filePath, exception);
        }
    }

    public void save() {
        try {
            Files.createDirectories(filePath.getParent());
            Properties properties = new Properties();
            properties.setProperty(KEY_SHOW_FPS, Boolean.toString(showFpsOverlay));
            properties.setProperty(KEY_WINDOW_PRESET, windowPreset.name());
            properties.setProperty(KEY_CUSTOM_SKIN_PATH, customSkinPath);
            properties.setProperty(KEY_RENDER_DISTANCE, Integer.toString(renderDistance));
            properties.setProperty(KEY_FANCY_GRAPHICS, Boolean.toString(fancyGraphics));
            try (OutputStream output = Files.newOutputStream(filePath)) {
                properties.store(output, "Minecraft Consoles Java Port options");
            }
        } catch (IOException exception) {
            throw new IllegalStateException("Failed to save Java port options to " + filePath, exception);
        }
    }

    public boolean showFpsOverlay() {
        return showFpsOverlay;
    }

    public void toggleFpsOverlay() {
        showFpsOverlay = !showFpsOverlay;
    }

    public WindowPreset windowPreset() {
        return windowPreset;
    }

    public void cycleWindowPreset() {
        windowPreset = windowPreset.next();
    }

    public String customSkinPath() {
        return customSkinPath;
    }

    public int renderDistance() {
        return renderDistance;
    }

    public boolean fancyGraphics() {
        return fancyGraphics;
    }

    public String fpsToggleLabel() {
        return "FPS Overlay: " + (showFpsOverlay ? "ON" : "OFF");
    }

    public String windowPresetLabel() {
        return "Window Size: " + windowPreset.label();
    }

    public Path filePath() {
        return filePath;
    }
}
