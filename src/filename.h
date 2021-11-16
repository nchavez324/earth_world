#ifndef EARTH_WORLD_FILENAME_H
#define EARTH_WORLD_FILENAME_H

#include "panda3d/filename.h"

namespace earth_world {
namespace filename {

extern Filename const kConfigFilename;
extern Filename const kModelsDirectory;
extern Filename const kShadersDirectory;
extern Filename const kTexturesDirectory;

/** @return Resolves the given filename relative to the source directory. */
Filename relativeToSourceDirectory(std::string relative_filename);

/** @return Resolves the given filename relative to the models directory. */
Filename forModel(std::string relative_filename);

/** @return Resolves the given filename relative to the shaders directory. */
Filename forShader(std::string relative_filename);

/** @return Resolves the given filename relative to the textures directory. */
Filename forTexture(std::string relative_filename);

}  // namespace filename
}  // namespace earth_world

#endif  // EARTH_WORLD_FILENAME_H
