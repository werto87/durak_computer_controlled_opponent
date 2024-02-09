from conan import ConanFile


class Project(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def configure(self):
        # We can control the options of our dependencies based on current options
        self.options["catch2"].with_main = True
        self.options["catch2"].with_benchmark = True
        self.options["boost"].header_only = True

    def requirements(self):
        self.requires("boost/1.84.0",force=True)
        self.requires("catch2/2.13.9")
        self.requires("durak/1.0.1")
        self.requires("st_tree/1.2.1")
        self.requires("small_memory_tree/4.0.1")
        self.requires("range-v3/0.12.0")
        self.requires("confu_soci/0.3.15")
        self.requires("magic_enum/[>=0.9.5 <10]")