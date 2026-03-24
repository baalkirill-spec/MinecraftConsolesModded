package org.minecraftport.mods;

import java.nio.file.Path;

public record ModDataDefinition(String namespaceId, String category, String identifier, Path sourcePath) {
}
