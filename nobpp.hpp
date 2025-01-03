/**
 * @file nobpp.hpp
 * @author pekochan069 (pekochan069@gmail.com)
 * @brief NoBuild C++
 * @version 0.1
 * @date 2025-01-02
 *
 * @copyright MIT License
 *
 * Copyright (c) 2025 pekochan069
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * Copyright (c) 2025
 *
 */

#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace nobpp
{
    std::vector<std::string> split(const std::string& str, const char delimiter) {
        std::vector<std::string> parts;
        std::string part;

        for (const char c : str) {
            if (c == delimiter) {
                parts.push_back(part);
                part = "";
            } else {
                part += c;
            }
        }

        parts.push_back(part);

        return parts;
    }

    std::vector<std::string> split(const std::string& str, std::string&& delimiter) {
        std::vector<std::string> parts;
        std::string part;

        for (const char c : str) {
            if (c == delimiter[0]) {
                parts.push_back(part);
                part = "";
            } else {
                part += c;
            }
        }

        parts.push_back(part);

        return parts;
    }

    std::string join(std::vector<std::string> parts, const char delimiter) {
        std::string out = "";

        for (size_t i = 0; i < parts.size(); ++i) {
            out += parts[i];

            if (i != parts.size() - 1) {
                out += delimiter;
            }
        }

        return out;
    }

    std::string join(std::vector<std::string> parts, std::string&& delimiter) {
        std::string out = "";

        for (size_t i = 0; i < parts.size(); ++i) {
            out += parts[i];

            if (i != parts.size() - 1) {
                out += delimiter;
            }
        }

        return out;
    }

#ifdef _WIN32
    constexpr char PATH_SEPARATOR = '\\';

    #define WIN32_LEAN_AND_MEAN
    #include <process.h>
    #include <stdio.h>
    #include <strsafe.h>
    #include <tchar.h>
    #include <windows.h>
    #pragma comment(lib, "User32.lib")

    std::vector<std::string> readdir(const wchar_t* wtarget_dir,
                                     std::function<bool(const std::string&)> file_predicate) {
        std::vector<std::string> files;
        size_t length_of_arg;
        wchar_t szDir[MAX_PATH];
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;

        StringCchLengthW(wtarget_dir, MAX_PATH, &length_of_arg);

        if (length_of_arg > (MAX_PATH - 3)) {
            std::wcout << "Directory path is too long." << wtarget_dir << "\n";
            return files;
        }

        StringCchCopyW(szDir, MAX_PATH, wtarget_dir);
        StringCchCatW(szDir, MAX_PATH, L"\\*");

        hFind = FindFirstFileW(szDir, &ffd);

        if (INVALID_HANDLE_VALUE == hFind) {
            std::cout << "FindFirstFile failed (" << GetLastError() << ")\n";
            return files;
        }

        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0) {
                    continue;
                }

                std::wstring wdir_name = std::wstring(wtarget_dir) + L"\\" + ffd.cFileName;
                std::vector<std::string> files_in_dir = readdir(wdir_name.c_str(), file_predicate);

                files.insert(files.end(), files_in_dir.begin(), files_in_dir.end());
                continue;
            }

            const std::wstring wfile_name = ffd.cFileName;
            const std::string file_name(wfile_name.begin(), wfile_name.end());

            if (file_predicate(file_name)) {
                std::string target_dir(wtarget_dir, wtarget_dir + wcslen(wtarget_dir));
                files.push_back(target_dir + PATH_SEPARATOR + file_name);
            }
        } while (FindNextFileW(hFind, &ffd) != 0);

        DWORD dwError = GetLastError();
        if (dwError != ERROR_NO_MORE_FILES) {
            std::cout << "FindFirstFile failed (" << dwError << ")\n";
        }

        FindClose(hFind);

        return files;
    }

    std::vector<std::string> readdir(const std::string& target,
                                     std::function<bool(const std::string&)> file_predicate) {
        std::wstring wtarget = std::wstring(target.begin(), target.end());
        std::replace(wtarget.begin(), wtarget.end(), '/', PATH_SEPARATOR);
        const wchar_t* wtarget_cstr = wtarget.c_str();
        return readdir(wtarget_cstr, file_predicate);
    }

    bool dir_exists(const std::string& target_dir) {
        const std::wstring wtarget_dir = std::wstring(target_dir.begin(), target_dir.end());
        const wchar_t* szDir = wtarget_dir.c_str();
        DWORD dwAttrib = GetFileAttributesW(szDir);

        return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    void createDirectoryRecursively(const std::wstring& target_dir) {
        static const std::wstring separators(L"\\/");

        // If the specified directory name doesn't exist, do our thing
        DWORD fileAttributes = GetFileAttributesW(target_dir.c_str());
        if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
            // Recursively do it all again for the parent directory, if any
            std::size_t slashIndex = target_dir.find_last_of(separators);
            if (slashIndex != std::wstring::npos) {
                createDirectoryRecursively(target_dir.substr(0, slashIndex));
            }

            // Create the last directory on the path (the recursive calls will have taken
            // care of the parent directories by now)
            BOOL result = CreateDirectoryW(target_dir.c_str(), nullptr);
            if (result == FALSE) {
                throw std::runtime_error("Could not create directory");
            }

        } else {  // Specified directory name already exists as a file or directory
            bool isDirectoryOrJunction = ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                                         ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

            if (!isDirectoryOrJunction) {
                throw std::runtime_error("Could not create directory because a file with the same name exists");
            }
        }
    }

    void createDirectoryRecursively(const std::string& target_dir) {
        std::wstring wtarget_dir = std::wstring(target_dir.begin(), target_dir.end());
        createDirectoryRecursively(wtarget_dir);
    }


    void run_command_sync(const std::string& command) {
        std::wstring wcommand = std::wstring(command.begin(), command.end());
        _wspawnlp(_P_WAIT, _T("powershell.exe"), _T("powershell.exe"), _T("-c"), wcommand.c_str(), NULL);
    }

    void run_command_async(const std::string& command) {
        std::wstring wcommand = std::wstring(command.begin(), command.end());
        _wspawnlp(_P_NOWAIT, _T("powershell.exe"), _T("powershell.exe"), _T("-c"), wcommand.c_str(), NULL);
    }
#else
    constexpr char PATH_SEPARATOR = '/';

    #include <dirent.h>
    std::vector<std::string> readdir(std::string target) {
        return {};
    }

    void run_command_sync(const std::string& command) {}

    void run_command_async(const std::string& command) {}
#endif

    enum struct Language { c, cpp };
    enum struct TargetOS { windows, linux };
    enum struct OptimizationLevel { none, o1, o2, o3, os, oz };

    bool is_c_file(const std::string& path) {
        const std::vector<std::string> parts = split(path, PATH_SEPARATOR);
        const std::string file_name = parts[parts.size() - 1];

        return path.find(".c") != std::string::npos;
    }

    bool is_c_header_file(const std::string& path) {
        const std::vector<std::string> parts = split(path, PATH_SEPARATOR);
        const std::string file_name = parts[parts.size() - 1];

        return path.find(".h") != std::string::npos;
    }

    bool is_cpp_file(const std::string& path) {
        static const std::unordered_set<std::string> cpp_extensions = {
            ".cpp", ".cc", ".c++", ".cxx", ".mpp", ".ipp", ".ixx", ".cppm"};

        const std::vector<std::string> parts = split(path, PATH_SEPARATOR);
        const std::string file_name = parts[parts.size() - 1];

        size_t pos = file_name.rfind('.');
        if (pos == std::string::npos)
            return false;
        std::string ext = file_name.substr(pos);
        return cpp_extensions.find(ext) != cpp_extensions.end();
    }

    bool is_cpp_header_file(const std::string& path) {
        static const std::unordered_set<std::string> cpp_header_extensions = {
            ".h", ".hh", ".hpp", ".hxx", ".h++", ".inl"};

        const std::vector<std::string> parts = split(path, PATH_SEPARATOR);
        const std::string file_name = parts[parts.size() - 1];

        size_t pos = file_name.rfind('.');
        if (pos == std::string::npos)
            return false;
        std::string ext = file_name.substr(pos);
        return cpp_header_extensions.find(ext) != cpp_header_extensions.end();
    }

    std::string join(const std::vector<std::string>& list, std::string&& delimiter = " ") {
        std::string out = "";

        for (auto it = list.cbegin(); it != list.cend(); ++it) {
            out += *it;

            if (it != list.cend() - 1) {
                out += delimiter;
            }
        }

        return out;
    }


    /**
     * @brief Command Builder to create and run build commands
     * @code
     * ```cpp
     * #include "nobpp.hpp"
     *
     * int main() {
     *      nobpp::CommandBuilder builder = nobpp::CommandBuilder();
     *
     *      builder.set_language(nobpp::Language::cpp)
     *          .set_target_os(nobpp::TargetOS::windows)
     *          .set_optimization_level(nobpp::OptimizationLevel::o3)
     *          .add_options({"-ffast-math"})
     *          .add_file("./test.cpp")
     *          .set_build_dir("./bin")
     *          .set_output("test")
     *          .run();
     *
     *      return 0;
     * }
     * ```
     * @endcode
     */
    class CommandBuilder {
    public:
        /**
         * @brief Construct a new Command Builder object
         * @return `nobpp::CommandBuilder`
         * @code
         * ```cpp
         * nobpp::CommandBuilder builder = nobpp::CommandBuilder();
         * ```
         * @endcode
         */
        CommandBuilder() = default;

        /**
         * @brief Set the language of the source files
         *
         * @param language `nobpp::Language::cpp` or `nobpp::Language::c`
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.set_language(nobpp::Language::cpp);
         * ```
         * @endcode
         */
        CommandBuilder& set_language(Language language) {
            self.language = language;
            return self;
        }

        /**
         * @brief Set the target operating system
         *
         * @param os `nobpp::TargetOS::windows` or `nobpp::TargetOS::linux`
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.set_target_os(nobpp::TargetOS::windows);
         * ```
         * @endcode
         */
        CommandBuilder& set_target_os(TargetOS os) {
            self.target_os = os;
            return self;
        }

        /**
         * @brief Set the optimization level
         *
         * @param level `nobpp::OptimizationLevel::none`
         *     | `nobpp::OptimizationLevel::o1`
         *     | `nobpp::OptimizationLevel::o2`
         *     | `nobpp::OptimizationLevel::o3`
         *     | `nobpp::OptimizationLevel::os`,
         *     | `nobpp::OptimizationLevel::oz`
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.set_optimization_level(nobpp::OptimizationLevel::o3);
         * ```
         * @endcode
         */
        CommandBuilder& set_optimization_level(OptimizationLevel level) {
            self.optimization_level = level;
            return self;
        }

        /**
         * @brief Add an include directory
         *
         * @param dir The directory to include
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_include_dir("./include");
         * ```
         * @endcode
         */
        CommandBuilder& add_include_dir(const char* dir) {
            self.include_dirs.push_back(dir);
            return self;
        }

        /**
         * @brief Add an include directory
         *
         * @param dir The directory to include
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_include_dir("./include");
         * ```
         * @endcode
         */
        CommandBuilder& add_include_dir(const std::string& dir) {
            self.include_dirs.push_back(dir);
            return self;
        }

        /**
         * @brief Add a source file
         *
         * @param file The source file to add
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_file("./src/main.cpp");
         * ```
         * @endcode
         */
        CommandBuilder& add_file(const char* file) {
            self.files.push_back(file);
            return self;
        }

        /**
         * @brief Add a source file
         *
         * @param file The source file to add
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_file("./src/main.cpp");
         * ```
         * @endcode
         */
        CommandBuilder& add_file(const std::string& file) {
            self.files.push_back(file);
            return self;
        }

        /**
         * @brief Add source files from a directory
         *
         * @param target The directory to search for source files
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_files("./src");
         * ```
         * @endcode
         */
        CommandBuilder& add_files(const char* target) {
            std::function<bool(const std::string&)> file_predicate;

            if (self.language == Language::c) {
                file_predicate = is_c_file;
            } else {
                file_predicate = is_cpp_file;
            }

            const std::vector<std::string> file_list = readdir(target, file_predicate);
            for (const std::string& file : file_list) {
                self.files.push_back(file);
            }
            return self;
        }

        /**
         * @brief Add source files from a directory
         *
         * @param target The directory to search for source files
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_files("./src");
         * ```
         * @endcode
         */
        CommandBuilder& add_files(const std::string& target) {
            std::function<bool(const std::string&)> file_predicate;

            if (self.language == Language::c) {
                file_predicate = is_c_file;
            } else {
                file_predicate = is_cpp_file;
            }

            const std::vector<std::string> file_list = readdir(target, file_predicate);
            for (const std::string& file : file_list) {
                self.files.push_back(file);
            }
            return self;
        }

        /**
         * @brief Add source files from a directory with a custom file predicate
         *
         * @param target The directory to search for source files
         * @param file_predicate A function that returns true if the file should be included
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_files("./src", [](const std::string& file) {
         *     return file.find(".cpp") != std::string::npos;
         * });
         * ```
         * @endcode
         */
        CommandBuilder& add_files(const char* target, std::function<bool(const std::string&)> file_predicate) {
            const std::vector<std::string> file_list = readdir(target, file_predicate);
            for (const std::string& file : file_list) {
                self.files.push_back(file);
            }
            return self;
        }

        /**
         * @brief Add source files from a directory with a custom file predicate
         *
         * @param target The directory to search for source files
         * @param file_predicate A function that returns true if the file should be included
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_files("./src", [](const std::string& file) {
         *     return file.find(".cpp") != std::string::npos;
         * });
         * ```
         * @endcode
         */
        CommandBuilder& add_files(const std::string& target, std::function<bool(const std::string&)> file_predicate) {
            const std::vector<std::string> file_list = readdir(target, file_predicate);
            for (const std::string& file : file_list) {
                self.files.push_back(file);
            }
            return self;
        }

        /**
         * @brief Add any compiler option
         *
         * @param opt A compiler option
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_option("-ffast-math");
         * ```
         * @endcode
         */
        CommandBuilder& add_option(const char* opt) {
            self.options.push_back(opt);
            return self;
        }

        /**
         * @brief Add any compiler option
         *
         * @param opt A compiler option
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_option("-ffast-math");
         * ```
         * @endcode
         */
        CommandBuilder& add_option(const std::string& opt) {
            self.options.push_back(opt);
            return self;
        }

        /**
         * @brief Add any compiler options
         *
         * @param opts A list of compiler options
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_options({"-ffast-math", "-fno-exceptions"});
         * ```
         * @endcode
         */
        CommandBuilder& add_options(std::initializer_list<std::string>&& opts) {
            for (const std::string& opt : opts) {
                self.options.push_back(opt);
            }
            return self;
        }

        /**
         * @brief Add any compiler options
         *
         * @param opts A list of compiler options
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.add_options({"-ffast-math", "-fno-exceptions"});
         * ```
         * @endcode
         */
        CommandBuilder& add_options(const std::vector<std::string>& opts) {
            for (const std::string& opt : opts) {
                self.options.push_back(opt);
            }
            return self;
        }

        /**
         * @brief Set the build directory
         *
         * @param dir The directory to build the output file in
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.set_build_dir("./bin");
         * ```
         * @endcode
         */
        CommandBuilder& set_build_dir(const std::string& dir) {
            self.build_dir = dir;
            return self;
        }

        /**
         * @brief Set the output file
         *
         * @param output The name of the output file
         * @return `nobpp::CommandBuilder&`
         * @code
         * ```cpp
         * builder.set_output("test");
         * ```
         * @endcode
         */
        CommandBuilder& set_output(const std::string& output) {
            std::string out = output;

            if (self.target_os == TargetOS::windows && out.find(".exe") == std::string::npos) {
                out += ".exe";
            }

            self.output = out;

            return self;
        }

        /**
         * @brief Create a command object
         *
         * @return std::string
         * @code
         * ```cpp
         * std::string command = builder.create_command();
         * ```
         */
        std::string create_command() const {
            std::vector<std::string> command;

            switch (self.language) {
            case Language::cpp:
                command.push_back("clang++");
                break;
            case Language::c:
                command.push_back("clang");
            }

            switch (self.optimization_level) {
            case OptimizationLevel::none:
                command.push_back("-O0");
                break;
            case OptimizationLevel::o1:
                command.push_back("-O1");
                break;
            case OptimizationLevel::o2:
                command.push_back("-O2");
                break;
            case OptimizationLevel::o3:
                command.push_back("-O3");
                break;
            case OptimizationLevel::os:
                command.push_back("-Os");
                break;
            case OptimizationLevel::oz:
                command.push_back("-Oz");
                break;
            }

            for (const std::string& option : self.options) {
                command.push_back(option);
            }

            for (const std::string& file : self.files) {
                command.push_back(file);
            }

            for (const std::string& include_dir : self.include_dirs) {
                command.push_back("-I" + include_dir);
            }

            if (self.build_dir != "" && self.output != "") {
                if (!dir_exists(self.build_dir)) {
                    createDirectoryRecursively(self.build_dir);
                }

                std::string out_file = self.build_dir + "/" + self.output;
                command.push_back("-o");
                command.push_back(out_file);
            } else {
                if (self.output != "") {
                    command.push_back("-o");
                    command.push_back(output);
                }
            }


            return join(command);
        }

        /**
         * @brief Run the command
         *
         * @code
         * ```cpp
         * builder.run();
         * ```
         * @endcode
         */
        void run() const {
            const std::string command = create_command();
            run_command_sync(command);
        }

    private:
        CommandBuilder& self = *this;

        Language language = Language::cpp;
        TargetOS target_os = TargetOS::windows;
        OptimizationLevel optimization_level = OptimizationLevel::o3;
        std::vector<std::string> include_dirs;
        std::vector<std::string> files;
        std::vector<std::string> options;
        std::string build_dir;
        std::string output;
    };
}  // namespace nobpp
