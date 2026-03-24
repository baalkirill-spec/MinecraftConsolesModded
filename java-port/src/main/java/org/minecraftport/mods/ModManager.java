package org.minecraftport.mods;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.stream.Stream;

public final class ModManager {
    private final Path runtimeRoot;
    private final Path modsDirectory;
    private final List<DiscoveredMod> discoveredMods = new ArrayList<>();

    public ModManager(Path runtimeRoot) {
        this.runtimeRoot = runtimeRoot;
        this.modsDirectory = runtimeRoot.resolve("mods");
    }

    public void initialize() {
        refresh();
    }

    public void refresh() {
        discoveredMods.clear();
        try {
            Files.createDirectories(modsDirectory);
            try (Stream<Path> children = Files.list(modsDirectory)) {
                children.sorted(Comparator.comparing(path -> path.getFileName().toString().toLowerCase(Locale.ROOT)))
                        .forEach(this::scanEntry);
            }
        } catch (IOException exception) {
            throw new IllegalStateException("Failed to scan mods directory " + modsDirectory, exception);
        }
    }

    private void scanEntry(Path path) {
        try {
            if (Files.isDirectory(path)) {
                discoveredMods.add(scanFolderMod(path));
            } else if (path.getFileName().toString().toLowerCase(Locale.ROOT).endsWith(".zip")) {
                DiscoveredMod mod = scanZipMetadata(path);
                if (mod != null) {
                    discoveredMods.add(mod);
                }
            }
        } catch (IOException exception) {
            throw new IllegalStateException("Failed to read mod entry " + path, exception);
        }
    }

    private DiscoveredMod scanFolderMod(Path root) throws IOException {
        Path manifest = root.resolve("mod.json");
        Path assets = root.resolve("assets");
        Path data = root.resolve("data");
        Path textures = root.resolve("textures");
        Path minecraft = root.resolve("minecraft");

        if (!Files.exists(manifest) && !Files.isDirectory(assets) && !Files.isDirectory(data)
                && !Files.isDirectory(textures) && !Files.isDirectory(minecraft)) {
            throw new IllegalStateException("Folder mod " + root + " has no supported phase-1 content.");
        }

        JsonObject manifestJson = Files.exists(manifest)
                ? JsonParser.parseString(Files.readString(manifest, StandardCharsets.UTF_8)).getAsJsonObject()
                : new JsonObject();

        List<Path> assetRoots = new ArrayList<>();
        assetRoots.add(root);
        if (Files.isDirectory(assets)) {
            assetRoots.add(assets);
        }

        List<ModDataDefinition> dataDefinitions = new ArrayList<>();
        if (Files.isDirectory(data)) {
            try (Stream<Path> files = Files.walk(data)) {
                files.filter(candidate -> Files.isRegularFile(candidate) && candidate.getFileName().toString().endsWith(".json"))
                        .sorted()
                        .forEach(candidate -> dataDefinitions.add(toDataDefinition(data, candidate)));
            }
        }

        List<String> warnings = new ArrayList<>();
        if (!Files.exists(manifest)) {
            warnings.add("No mod.json found; using folder name fallback metadata.");
        }

        String fallbackId = normalizeId(root.getFileName().toString());
        String id = manifestJson.has("id") ? normalizeId(manifestJson.get("id").getAsString()) : fallbackId;
        String name = manifestJson.has("name") ? manifestJson.get("name").getAsString() : root.getFileName().toString();
        String version = manifestJson.has("version") ? manifestJson.get("version").getAsString() : "0.0.0";

        return new DiscoveredMod(id, name, version, ModSourceType.FOLDER, root, assetRoots, dataDefinitions, warnings);
    }

    private DiscoveredMod scanZipMetadata(Path zipPath) throws IOException {
        try (ZipFile zipFile = new ZipFile(zipPath.toFile())) {
            ZipEntry manifestEntry = zipFile.getEntry("mod.json");
            if (manifestEntry == null) {
                return null;
            }
            JsonObject manifestJson = JsonParser.parseReader(new InputStreamReader(zipFile.getInputStream(manifestEntry), StandardCharsets.UTF_8)).getAsJsonObject();
            String fallbackId = normalizeId(zipPath.getFileName().toString().replaceFirst("\\.zip$", ""));
            String id = manifestJson.has("id") ? normalizeId(manifestJson.get("id").getAsString()) : fallbackId;
            String name = manifestJson.has("name") ? manifestJson.get("name").getAsString() : zipPath.getFileName().toString();
            String version = manifestJson.has("version") ? manifestJson.get("version").getAsString() : "0.0.0";
            return new DiscoveredMod(
                    id,
                    name,
                    version,
                    ModSourceType.ZIP,
                    zipPath,
                    List.of(),
                    List.of(),
                    List.of("Zip mods are metadata-only in the Java port phase-1 skeleton."));
        }
    }

    private ModDataDefinition toDataDefinition(Path dataRoot, Path sourcePath) {
        Path relative = dataRoot.relativize(sourcePath);
        if (relative.getNameCount() < 3) {
            return new ModDataDefinition("invalid", "invalid", "invalid:" + relative, sourcePath);
        }
        String namespace = relative.getName(0).toString().toLowerCase(Locale.ROOT);
        String category = relative.getName(1).toString().toLowerCase(Locale.ROOT);
        String logicalPath = relative.subpath(2, relative.getNameCount()).toString().replace('\\', '/');
        logicalPath = logicalPath.substring(0, logicalPath.length() - ".json".length());
        return new ModDataDefinition(namespace, category, namespace + ":" + logicalPath, sourcePath);
    }

    private String normalizeId(String raw) {
        StringBuilder builder = new StringBuilder();
        for (char ch : raw.toLowerCase(Locale.ROOT).toCharArray()) {
            if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-' || ch == '.') {
                builder.append(ch);
            } else {
                builder.append('_');
            }
        }
        return builder.toString();
    }

    public Path modsDirectory() {
        return modsDirectory;
    }

    public Path runtimeRoot() {
        return runtimeRoot;
    }

    public List<DiscoveredMod> discoveredMods() {
        return List.copyOf(discoveredMods);
    }

    public int totalDataDefinitions() {
        return discoveredMods.stream().map(DiscoveredMod::dataDefinitions).filter(Objects::nonNull).mapToInt(List::size).sum();
    }
}
