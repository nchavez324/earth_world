#include "filename.h"

namespace earth_world {
namespace filename {

Filename const kConfigFilename = relativeToSourceDirectory("config.prc");
Filename const kModelsDirectory = relativeToSourceDirectory("models");
Filename const kShadersDirectory = relativeToSourceDirectory("shaders");
Filename const kTexturesDirectory = relativeToSourceDirectory("textures");

Filename relativeToSourceDirectory(std::string relative_filename) {
  char *working_directory_c_string = getcwd(NULL, 0);
  Filename working_directory(working_directory_c_string);
  free(working_directory_c_string);

  Filename filename(working_directory, relative_filename);
  filename.make_absolute();
  return filename;
}

Filename forModel(std::string relative_filename) {
  return Filename(kModelsDirectory, relative_filename);
}

Filename forShader(std::string relative_filename) {
  return Filename(kShadersDirectory, relative_filename);
}

Filename forTexture(std::string relative_filename) {
  return Filename(kTexturesDirectory, relative_filename);
}

}  // namespace filename
}  // namespace earth_world