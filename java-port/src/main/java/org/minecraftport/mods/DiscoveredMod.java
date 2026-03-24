package org.minecraftport.mods;

import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class DiscoveredMod {
    private final String id;
    private final String name;
    private final String version;
    private final ModSourceType sourceType;
    private final Path sourcePath;
    private final List<Path> assetRoots;
    private final List<ModDataDefinition> dataDefinitions;
    private final List<String> warnings;

    public DiscoveredMod(
            String id,
            String name,
            String version,
            ModSourceType sourceType,
            Path sourcePath,
            List<Path> assetRoots,
            List<ModDataDefinition> dataDefinitions,
            List<String> warnings) {
        this.id = id;
        this.name = name;
        this.version = version;
        this.sourceType = sourceType;
        this.sourcePath = sourcePath;
        this.assetRoots = Collections.unmodifiableList(new ArrayList<>(assetRoots));
        this.dataDefinitions = Collections.unmodifiableList(new ArrayList<>(dataDefinitions));
        this.warnings = Collections.unmodifiableList(new ArrayList<>(warnings));
    }

    public String id() {
        return id;
    }

    public String name() {
        return name;
    }

    public String version() {
        return version;
    }

    public ModSourceType sourceType() {
        return sourceType;
    }

    public Path sourcePath() {
        return sourcePath;
    }

    public List<Path> assetRoots() {
        return assetRoots;
    }

    public List<ModDataDefinition> dataDefinitions() {
        return dataDefinitions;
    }

    public List<String> warnings() {
        return warnings;
    }

    public String displayName() {
        return name == null || name.isBlank() ? id : name;
    }
}
