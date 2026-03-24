package org.minecraftport.options;

public enum WindowPreset {
    CURRENT(0, 0, "Current"),
    HD_720(1280, 720, "1280x720"),
    HD_PLUS(1600, 900, "1600x900"),
    FULL_HD(1920, 1080, "1920x1080");

    private final int width;
    private final int height;
    private final String label;

    WindowPreset(int width, int height, String label) {
        this.width = width;
        this.height = height;
        this.label = label;
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }

    public String label() {
        return label;
    }

    public WindowPreset next() {
        WindowPreset[] values = values();
        return values[(ordinal() + 1) % values.length];
    }

    public static WindowPreset fromName(String raw) {
        if (raw == null || raw.isBlank()) {
            return CURRENT;
        }
        for (WindowPreset value : values()) {
            if (value.name().equalsIgnoreCase(raw)) {
                return value;
            }
        }
        return CURRENT;
    }
}
