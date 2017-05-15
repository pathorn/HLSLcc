#include "hlslcc.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

const char *g_executableName = NULL;

void Error(const std::string& msg)
{
  std::cout << g_executableName << ": Error " << msg << " (use -h for usage)";
  exit(EXIT_FAILURE);
}

void PrintHelp()
{
  std::cout << "Usage: hlslcc [options] [flags] input_file\n"
               "\n"
               "Options:\n"
               "  -h              prints this message\n"
               "  -o <file>       save translation to <file>\n"
               "\n"
               "  --language      one of the following:\n"
               "                  100es, 300es, 310es, 120, 130, 140, 150\n"
               "                  330, 400, 410, 420, 430, 440, metal\n"
               "\n"
               "Flags:\n"
               "  --uniform-buffer-object                   HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT\n"
               "\n"      
               "  --origin-upper-left                       HLSLCC_FLAG_ORIGIN_UPPER_LEFT\n"
               "\n"      
               "  --pixel-center-integer                    HLSLCC_FLAG_PIXEL_CENTER_INTEGER\n"
               "\n"      
               "  --global-consts-never-in-ubo              HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO\n"
               "\n"      
               "  --gs-enabled                              HLSLCC_FLAG_GS_ENABLED\n"
               "\n"      
               "  --tess-enabled                            HLSLCC_FLAG_TESS_ENABLED\n"
               "\n"      
               "  --dual-source-blending                    HLSLCC_FLAG_DUAL_SOURCE_BLENDING\n"
               "\n"      
               "  --inout-semantic-names                    HLSLCC_FLAG_INOUT_SEMANTIC_NAMES\n"
               "\n"      
               "  --inout-append-semantic-names             HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES\n"
               "\n"      
               "  --combine-texture-samplers                HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS\n"
               "\n"      
               "  --disable-explicit-locations              HLSLCC_FLAG_DISABLE_EXPLICIT_LOCATIONS\n"
               "\n"      
               "  --disable-globals-struct                  HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT\n"
               "\n"      
               "  --gles31-image-qualifiers                 HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS\n"
               "\n"      
               "  --sampler-precision-encoded-in-name       HLSLCC_FLAG_SAMPLER_PRECISION_ENCODED_IN_NAME\n"
               "\n"      
               "  --separable-shader-objects                HLSLCC_FLAG_SEPARABLE_SHADER_OBJECTS\n"
               "\n"      
               "  --wrap-ubo                                HLSLCC_FLAG_WRAP_UBO\n"
               "\n"      
               "  --remove-unused-globals                   HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS\n"
               "\n"      
               "  --translate-matrices                      HLSLCC_FLAG_TRANSLATE_MATRICES\n"
               "\n"      
               "  --vulkan-bindings                         HLSLCC_FLAG_VULKAN_BINDINGS\n"
               "\n"      
               "  --metal-shadow-sampler-linear             HLSLCC_FLAG_METAL_SHADOW_SAMPLER_LINEAR\n"
               "\n"      
               "  --nvn-target                              HLSLCC_FLAG_NVN_TARGET\n"
               "\n"      
               "  --vulkan-specialization-constants         HLSLCC_FLAG_VULKAN_SPECIALIZATION_CONSTANTS\n"
               "\n"      
               "  --shader-framebuffer-fetch                HLSLCC_FLAG_SHADER_FRAMEBUFFER_FETCH\n" << std::endl;
}

void ToLower(std::string &s)
{
  std::transform(std::begin(s), std::end(s), std::begin(s), std::tolower);
}

std::string GetArgument(
  const int         argc,
  const char *const *argv,
  const int         index
)
{
  std::string result;
  if ((index >= 0) && (index < argc)) {
    result = argv[index];
  }
  return result;
}

GLLang ProcessLanguageArgument(const std::string &optArg)
{
  GLLang language = LANG_DEFAULT;
  if (optArg == "100es") { 
    language = LANG_ES_100;
  } else if (optArg == "300es") { 
    language = LANG_ES_300; 
  } else if (optArg == "310es") { 
    language = LANG_ES_310; 
  } else if (optArg == "120") {
    language = LANG_120;
  } else if (optArg == "130") {
    language = LANG_130;
  } else if (optArg == "140") {
    language = LANG_140; 
  } else if (optArg == "150") {
    language = LANG_150;
  } else if (optArg == "330") { 
    language = LANG_330;
  } else if (optArg == "400") { 
    language = LANG_400;
  } else if (optArg == "410") { 
    language = LANG_410;
  } else if (optArg == "420") { 
    language = LANG_420;
  } else if (optArg == "430") { 
    language = LANG_430;
  } else if (optArg == "440") { 
    language = LANG_440;
  } else if (optArg == "metal") {
    language = LANG_METAL;
  }
  return language;
}

unsigned int ProcessFlagArgument(const std::string &opt)
{
  unsigned int flag = 0;
  if ((opt == "uniform-buffer-object") || (opt == "ubo")) {
    flag = HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;
  } else if (opt == "origin-upper-left") {                       
    flag = HLSLCC_FLAG_ORIGIN_UPPER_LEFT;
  } else if (opt == "pixel-center-integer") {                    
    flag = HLSLCC_FLAG_PIXEL_CENTER_INTEGER;
  } else if (opt == "global-consts-never-in-ubo") {              
    flag = HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO;
  } else if (opt == "gs-enabled") {                              
    flag = HLSLCC_FLAG_GS_ENABLED;
  } else if (opt == "tess-enabled") {                            
    flag = HLSLCC_FLAG_TESS_ENABLED;
  } else if (opt == "dual-source-blending") {                    
    flag = HLSLCC_FLAG_DUAL_SOURCE_BLENDING;
  } else if (opt == "inout-semantic-names") {                    
    flag = HLSLCC_FLAG_INOUT_SEMANTIC_NAMES;
  } else if (opt == "inout-append-semantic-names") {             
    flag = HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES;
  } else if (opt == "combine-texture-samplers") {                
    flag = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS;
  } else if (opt == "disable-explicit-locations") {              
    flag = HLSLCC_FLAG_DISABLE_EXPLICIT_LOCATIONS;
  } else if (opt == "disable-globals-struct") {                  
    flag = HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
  } else if (opt == "gles31-image-qualifiers") {                 
    flag = HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS;
  } else if (opt == "sampler-precision-encoded-in-name") {       
    flag = HLSLCC_FLAG_SAMPLER_PRECISION_ENCODED_IN_NAME;
  } else if (opt == "separable-shader-objects") {                
    flag = HLSLCC_FLAG_SEPARABLE_SHADER_OBJECTS;
  } else if (opt == "wrap-ubo") {                                
    flag = HLSLCC_FLAG_WRAP_UBO;
  } else if (opt == "remove-unused-globals") {                   
    flag = HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS;
  } else if (opt == "translate-matrices") {                      
    flag = HLSLCC_FLAG_TRANSLATE_MATRICES;
  } else if (opt == "vulkan-bindings") {                         
    flag = HLSLCC_FLAG_VULKAN_BINDINGS;
  } else if (opt == "metal-shadow-sampler-linear") {             
    flag = HLSLCC_FLAG_METAL_SHADOW_SAMPLER_LINEAR;
  } else if (opt == "nvn-target") {                              
    flag = HLSLCC_FLAG_NVN_TARGET;
  } else if (opt == "vulkan-specialization-constants") {         
    flag = HLSLCC_FLAG_VULKAN_SPECIALIZATION_CONSTANTS;
  } else if (opt == "shader-framebuffer-fetch") {                
    flag = HLSLCC_FLAG_SHADER_FRAMEBUFFER_FETCH;
  }
  return flag;
}

void ProcessArguments(
  const int         argc,
  const char *const *argv,
  std::string       *outInputFilename,
  std::string       *outOutputFilename,
  unsigned int      *outFlags,
  GLLang            *outLlanguage,
  bool              *outPrintTranslation
)
{
  std::string inputFilename;
  std::string outputFilename;
  unsigned int flags = 0;
  GLLang language = LANG_DEFAULT;
  bool printTranslation = false;

  int argIndex = 1;
  for (; argIndex < argc; ++argIndex) {
    std::string arg = GetArgument(argc, argv, argIndex);
    if (arg[0] == '-') {
      switch (arg[1]) {
        // -h
        case 'h' : {
          PrintHelp();
          exit(EXIT_SUCCESS);
        }
        break;

        // -H
        case 'H' : {
          printTranslation = true;
        }
        break;

        // -o <file>
        case 'o' : {
          outputFilename = GetArgument(argc, argv, argIndex + 1);
          if (outputFilename.empty()) {
            Error("-o missing file argument");
          }

          ++argIndex;
        }
        break;

        // --* arguments
        case '-': {
          std::string opt(arg.c_str() + 2);
          ToLower(opt);
          
          // output file
          if (opt == "language") {
            std::string optArg = GetArgument(argc, argv, argIndex + 1);
            if (optArg.empty()) {
              Error("--language missing argument");
            }

            ToLower(optArg);
            language = ProcessLanguageArgument(optArg);

            if (language == LANG_DEFAULT) {
              Error("invalid argument for --language");
            }

            ++argIndex;
          }
          // flags
          else {
            unsigned int flag = ProcessFlagArgument(opt);
            if (flag == 0) {
              Error(("unrecognized option " + arg).c_str());
            }

            flags |= flag;
          }
        }
        break;

        default: {
          Error(("unrecognized option " + arg).c_str());
        }
        break;
      }
    }
    else {
      // input file
      if (inputFilename.empty()) {
        inputFilename = arg;
      }
      else {
        Error(("unrecognized argument " + arg).c_str());
      }
    }
  }

  if (outInputFilename != NULL) {
    *outInputFilename = inputFilename;
  }

  if (outOutputFilename != NULL) {
    *outOutputFilename = outputFilename;
  }

  if (outFlags != NULL) {
    *outFlags = flags;
  }

  if (outPrintTranslation != NULL) {
    *outPrintTranslation = printTranslation;
  }
}

int main(int argc, char **argv)
{
  g_executableName = argv[0];

  if (argc == 1) {
    PrintHelp();
    exit(EXIT_SUCCESS);
  }


  std::string inputFilename;
  std::string outputFilename;
	unsigned int flags = 0;
	GLLang language = LANG_DEFAULT;
  GlExtensions extensions = {};
  GLSLCrossDependencyData dependencies;
	HLSLccSamplerPrecisionInfo samplerPrecisions;
	HLSLccReflection reflectionCallbacks;
  GLSLShader shader;
  bool printTranslation = false;

  ProcessArguments(argc, argv, &inputFilename, &outputFilename, &flags, &language, &printTranslation);

  // Force UBOs if using Vulkan bindings otherwise there's a crash
  if (flags & HLSLCC_FLAG_VULKAN_BINDINGS) {
    flags |= HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;
  }


  if (inputFilename.empty()) {
    Error("missing input file");
  }
  else if (outputFilename.empty()) {
    Error("missing output file");
  }

  // Make sure input file is accessible
  std::ifstream is(inputFilename.c_str(), std::ios::binary);
  if (! is.is_open()) {
    Error("cannot open input file: " + inputFilename);
  }
  is.close();

  int ret = TranslateHLSLFromFile(inputFilename.c_str(), flags, language, &extensions, &dependencies, samplerPrecisions, reflectionCallbacks, &shader);
  if (ret == 0) {
    std::cout << "Error: " << "translation failed from " << inputFilename << " to " << outputFilename << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (shader.sourceCode.empty()) {
    std::cout << "Warning: translated source is empty, nothing to output" << std::endl;
    exit(EXIT_SUCCESS);
  }

  std::ofstream os(outputFilename.c_str());
  if (! os.is_open()) {
    std::cout << "Error: " << "cannot open output file: " << outputFilename << std::endl;
    exit(EXIT_SUCCESS);
  }

  os.write(shader.sourceCode.c_str(), shader.sourceCode.length());
  os.close();

  if (printTranslation) {
    std::cout << shader.sourceCode << std::endl;
  }
  else {
    std::cout << "Successfully translated " << inputFilename << " to " << outputFilename << std::endl;
  }

  return EXIT_SUCCESS;
}