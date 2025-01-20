/**
 * @file nobpp.hpp
 * @author pekochan069 (pekochan069@gmail.com)
 * @brief NoBuild C++
 * @version 0.2
 * @date 2025-01-18
 * @link https://github.com/pekochan069/nobpp
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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <process.h>
    #include <stdio.h>
    #include <strsafe.h>
    #include <tchar.h>
    #include <windows.h>
    #pragma comment(lib, "User32.lib")
#elif __linux__
#endif

namespace nobpp {

namespace nanoid {
/*
The MIT License (MIT)

Copyright 2020 Alan Ramírez Herrera <alan5142@hotmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
using string_alphabet = std::string const;
using string_alphabet_parameter = string_alphabet&;

/**
 * Contains a set of useful alphabets
 */
namespace alphabets {
/**
 * Default alphabet
 */
string_alphabet DEFAULT_ALPHABET =
    "_-0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Only numbers (0 to 9)
 */
string_alphabet NUMBERS = "0123456789";

/**
 * Uppercase english letters
 */
string_alphabet UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Lowercase english letters
 */
string_alphabet LOWERCASE = "abcdefghijklmnopqrstuvwxyz";

/**
 * Numbers and english alphabet without lookalikes: 1, l, I, 0, O, o, u, v, 5,
 * S, s
 */
string_alphabet NO_LOOK_ALIKES =
    "2346789ABCDEFGHJKLMNPQRTUVWXYZabcdefghijkmnpqrtwxyz";
}  // namespace alphabets

namespace details {
inline void generate_random_bytes(std::vector<std::uint8_t>::iterator begin,
    std::vector<std::uint8_t>::iterator end) {
    static std::random_device device;
    std::generate(begin, end, []() { return device(); });
}

template <typename F>
struct is_invocable
    : std::is_constructible<
          std::function<void(std::vector<std::uint8_t>::iterator,
              std::vector<std::uint8_t>::iterator)>,
          std::reference_wrapper<typename std::remove_reference<F>::type>> {};
}  // namespace details

/**
 * Generates a random id
 * @tparam RandomGenerator random numeric generator, it must be a callable
 * object with two arguments of type std::vector<uint8_t>::iterator: begin and
 * end
 * @param random_device random device callable object
 * @param alphabet set of characters to be used to generate the random id
 * @param length id length
 * @return string with random id
 */
template <class RandomGenerator,
    class =
        typename std::enable_if<details::is_invocable<RandomGenerator>::value>>
inline std::string generate(RandomGenerator& random_device,
    string_alphabet_parameter alphabet = alphabets::DEFAULT_ALPHABET,
    int length = 21) {
    if (alphabet.empty() || alphabet.size() >= 256) {
        throw std::invalid_argument(
            "Alphabet must contain between 1 and 255 sybols.");
    }
    if (length <= 0) {
        throw std::invalid_argument("Size must be greater than zero.");
    }

    auto mask = (2 << static_cast<int>(
                     std::floor(std::log(alphabet.size() - 1) / std::log(2)))) -
                1;
    auto step =
        static_cast<int>(std::ceil(1.6 * mask * length / alphabet.size()));

    auto id_builder = std::string();
    id_builder.reserve(length);
    auto count = 0;

    auto bytes = std::vector<std::uint8_t>(step);

    while (true) {
        random_device(std::begin(bytes), std::end(bytes));
        for (auto i = 0; i < step; ++i) {
            auto alphabet_index = bytes[i] & mask;
            if (alphabet_index >= alphabet.length())
                continue;

            id_builder.push_back(alphabet[alphabet_index]);
            if (++count == length) {
                return id_builder;
            }
        }
    }
}

/**
 * Generates a random id
 * @details it uses class "nanoid::random_number_generator" as id generator
 * @param alphabet set of characters to be used to generate the random id
 * @param length id length
 * @return string with random id
 */
inline std::string generate(
    string_alphabet_parameter alphabet = alphabets::DEFAULT_ALPHABET,
    int length = 21) {
    return generate(details::generate_random_bytes, alphabet, length);
}

/**
 * Generates a random id
 * @details it uses class "nanoid::random_number_generator" as id generator
 * @param alphabet set of characters to be used to generate the random id
 * @param length id length
 * @return string with random id
 */
inline std::string generate(char const* const alphabet, int length = 21) {
    return generate(string_alphabet_parameter(alphabet), length);
}

/**
 * Generates a random id of given length
 * @param size random id length
 * @return string with random id
 */
inline std::string generate(int size) {
    return generate(alphabets::DEFAULT_ALPHABET, size);
}
}  // namespace nanoid

#ifdef _WIN32

constexpr char PATH_SEPARATOR = '\\';

class Process {
public:
    Process() = default;
    Process(const char* command) : command(command) {}
    Process(const std::string& command) : command(command) {}
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    void set_command(const std::string& command) noexcept {
        self.command = command;
    }

    /**
     * @brief run process
     *
     * @return `false` if error occured.
     * @return `true` if successfully ran the command.
     */
    bool run() {
        if (self.command == "") {
            return false;
        }

        STARTUPINFOW startup_info;
        PROCESS_INFORMATION process_info;

        ZeroMemory(&startup_info, sizeof(startup_info));
        ZeroMemory(&process_info, sizeof(process_info));

        startup_info.cb = sizeof(startup_info);

        std::wstring wcommand(self.command.begin(), self.command.end());

        BOOL create_result = CreateProcessW(nullptr,
            const_cast<wchar_t*>(wcommand.c_str()), nullptr, nullptr, FALSE, 0,
            nullptr, nullptr, &startup_info, &process_info);

        if (!create_result) {
            return false;
        }

        WaitForSingleObject(process_info.hProcess, INFINITE);

        CloseHandle(process_info.hProcess);
        CloseHandle(process_info.hThread);

        self.command = "";

        return true;
    }

private:
    Process& self = *this;

    std::string command = "";
};

void create_process(const std::string& command) {
    if (command == "") {
        return;
    }

    STARTUPINFOW startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&process_info, sizeof(process_info));

    startup_info.cb = sizeof(startup_info);

    std::wstring wcommand(command.begin(), command.end());

    BOOL create_result =
        CreateProcessW(nullptr, const_cast<wchar_t*>(wcommand.c_str()), nullptr,
            nullptr, FALSE, 0, nullptr, nullptr, &startup_info, &process_info);

    if (!create_result) {
        return;
    }

    WaitForSingleObject(process_info.hProcess, INFINITE);

    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
}

std::vector<std::string> readdir(const wchar_t* wtarget_dir,
    std::function<bool(const std::string&)> file_predicate, bool recursive) {
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
            if (!recursive) {
                continue;
            }

            if (wcscmp(ffd.cFileName, L".") == 0 ||
                wcscmp(ffd.cFileName, L"..") == 0) {
                continue;
            }

            std::wstring wdir_name =
                std::wstring(wtarget_dir) + L"\\" + ffd.cFileName;
            std::vector<std::string> files_in_dir =
                readdir(wdir_name.c_str(), file_predicate, recursive);

            files.insert(files.end(), files_in_dir.begin(), files_in_dir.end());
            continue;
        }

        const std::wstring wfile_name = ffd.cFileName;
        const std::string file_name(wfile_name.begin(), wfile_name.end());

        if (file_predicate(file_name)) {
            std::string target_dir(
                wtarget_dir, wtarget_dir + wcslen(wtarget_dir));
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
    std::function<bool(const std::string&)> file_predicate, bool recursive) {
    std::wstring wtarget = std::wstring(target.begin(), target.end());
    std::replace(wtarget.begin(), wtarget.end(), '/', PATH_SEPARATOR);
    const wchar_t* wtarget_cstr = wtarget.c_str();
    return readdir(wtarget_cstr, file_predicate, recursive);
}

bool dir_exists(const std::string& target_dir) {
    const std::wstring wtarget_dir =
        std::wstring(target_dir.begin(), target_dir.end());
    const wchar_t* szDir = wtarget_dir.c_str();
    DWORD dwAttrib = GetFileAttributesW(szDir);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
            (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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

        // Create the last directory on the path (the recursive calls will have
        // taken care of the parent directories by now)
        BOOL result = CreateDirectoryW(target_dir.c_str(), nullptr);
        if (result == FALSE) {
            throw std::runtime_error("Could not create directory");
        }

    } else {  // Specified directory name already exists as a file or directory
        bool isDirectoryOrJunction =
            ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
            ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

        if (!isDirectoryOrJunction) {
            throw std::runtime_error(
                "Could not create directory because a file with the same name "
                "exists");
        }
    }
}

void createDirectoryRecursively(const std::string& target_dir) {
    std::wstring wtarget_dir =
        std::wstring(target_dir.begin(), target_dir.end());
    createDirectoryRecursively(wtarget_dir);
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

std::vector<std::string> split(
    const std::string& str, const char delimiter) noexcept {
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

std::vector<std::string> split(
    const std::string& str, const std::string& delimiter) noexcept {
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

std::string join(
    const std::vector<std::string>& parts, const char delimiter) noexcept {
    if (parts.size() == 0) {
        return "";
    } else if (parts.size() == 1) {
        return parts[0];
    }

    std::string out = "";

    for (size_t i = 0; i < parts.size(); ++i) {
        out += parts[i];

        if (i != parts.size() - 1) {
            out += delimiter;
        }
    }

    return out;
}

std::string join(const std::vector<std::string>& parts,
    const std::string& delimiter) noexcept {
    if (parts.size() == 0) {
        return "";
    } else if (parts.size() == 1) {
        return parts[0];
    }

    std::string out = "";

    for (size_t i = 0; i < parts.size(); ++i) {
        out += parts[i];

        if (i != parts.size() - 1) {
            out += delimiter;
        }
    }

    return out;
}

bool is_c_file(const std::string& path) noexcept {
    const std::vector<std::string> parts = split(path, PATH_SEPARATOR);

    if (parts.size() == 0) {
        return false;
    }

    const std::string file_name = parts[parts.size() - 1];

    return path.find(".c") != std::string::npos;
}

bool is_c_header_file(const std::string& path) noexcept {
    const std::vector<std::string> parts = split(path, PATH_SEPARATOR);

    if (parts.size() == 0) {
        return false;
    }

    const std::string file_name = parts[parts.size() - 1];

    return path.find(".h") != std::string::npos;
}

bool is_cpp_file(const std::string& path) noexcept {
    static const std::unordered_set<std::string> cpp_extensions = {
        ".cpp", ".cc", ".c++", ".cxx", ".mpp", ".ipp", ".ixx", ".cppm"};

    const std::vector<std::string> parts = split(path, PATH_SEPARATOR);

    if (parts.size() == 0) {
        return false;
    }

    const std::string file_name = parts[parts.size() - 1];

    size_t pos = file_name.rfind('.');
    if (pos == std::string::npos)
        return false;
    std::string ext = file_name.substr(pos);
    return cpp_extensions.find(ext) != cpp_extensions.end();
}

bool is_cpp_header_file(const std::string& path) noexcept {
    static const std::unordered_set<std::string> cpp_header_extensions = {
        ".h", ".hh", ".hpp", ".hxx", ".h++", ".inl"};

    const std::vector<std::string> parts = split(path, PATH_SEPARATOR);

    if (parts.size() == 0) {
        return false;
    }

    const std::string file_name = parts[parts.size() - 1];

    size_t pos = file_name.rfind('.');
    if (pos == std::string::npos)
        return false;
    std::string ext = file_name.substr(pos);
    return cpp_header_extensions.find(ext) != cpp_header_extensions.end();
}

enum struct Compiler { clang, gcc };
enum struct Language { c, cpp };
enum struct TargetOS { windows, linux };
enum struct OptimizationLevel { o0, o1, o2, o3, os, oz };
enum struct Mode { debug, release };

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
     * @brief Set the project name
     *
     * @param project_name `const std::string&`
     * @return `CommandBuilder&`
     * @code
     * ```cpp
     * builder.set_project_name("Hello Project");
     * ```
     * @endcode
     */
    CommandBuilder& set_project_name(const std::string& project_name) noexcept {
        self.project_name = project_name;
    }

    /**
     * @brief Set the project name
     *
     * @param project_name `const char*`
     * @return `CommandBuilder&`
     * @code
     * ```cpp
     * builder.set_project_name("Hello Project");
     * ```
     * @endcode
     */
    CommandBuilder& set_project_name(const char* project_name) noexcept {
        self.project_name = project_name;
    }

    /**
     * @brief Set the compiler
     *
     * @param compiler `nobpp::Compiler`
     * @return `CommandBuilder&`
     * @code
     * ```cpp
     * builder.set_compiler(nobpp::Compiler::clang);
     * ```
     * @endcode
     */
    CommandBuilder& set_compiler(Compiler compiler) noexcept {
        self.compiler = compiler;
        return self;
    }

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
    CommandBuilder& set_language(Language language) noexcept {
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
    CommandBuilder& set_target_os(TargetOS os) noexcept {
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
    CommandBuilder& set_optimization_level(OptimizationLevel level) noexcept {
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
    CommandBuilder& add_include_dir(const char* dir) noexcept {
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
    CommandBuilder& add_include_dir(const std::string& dir) noexcept {
        self.include_dirs.push_back(dir);
        return self;
    }

    /**
     * @brief Add include directories from a list of directories
     *
     * @param dirs A list of directories to include
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_include_dirs({"./include", "./src"});
     * ```
     * @endcode
     */
    CommandBuilder& add_include_dirs(
        std::initializer_list<std::string> dirs) noexcept {
        self.include_dirs.reserve(self.include_dirs.size() + dirs.size());
        self.include_dirs.insert(
            self.include_dirs.end(), dirs.begin(), dirs.end());
        return self;
    }

    /**
     * @brief Add include directories from a list of directories
     *
     * @param dirs A list of directories to include
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_include_dirs({"./include", "./src"});
     * ```
     * @endcode
     */
    CommandBuilder& add_include_dirs(std::vector<std::string>&& dirs) noexcept {
        self.include_dirs.reserve(self.include_dirs.size() + dirs.size());
        self.include_dirs.insert(
            self.include_dirs.end(), dirs.begin(), dirs.end());
        return self;
    }

    /**
     * @brief Add include directories from a list of directories
     *
     * @param dirs A list of directories to include
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_include_dirs({"./include", "./src"});
     * ```
     * @endcode
     */
    CommandBuilder& add_include_dirs(
        const std::vector<std::string>& dirs) noexcept {
        self.include_dirs.reserve(self.include_dirs.size() + dirs.size());
        self.include_dirs.insert(
            self.include_dirs.end(), dirs.begin(), dirs.end());
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
    CommandBuilder& add_file(const char* file) noexcept {
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
    CommandBuilder& add_file(const std::string& file) noexcept {
        self.files.push_back(file);
        return self;
    }

    /**
     * @brief Add source files from a directory
     *
     * @param target_directory The directory to search for source files
     * @param recursive Whether to search recursively
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files("./src");
     * ```
     * @endcode
     */
    CommandBuilder& add_files(
        const char* target_directory, bool recursive = true) {
        std::function<bool(const std::string&)> file_predicate;

        if (self.language == Language::c) {
            file_predicate = is_c_file;
        } else {
            file_predicate = is_cpp_file;
        }

        const std::vector<std::string> file_list =
            readdir(target_directory, file_predicate, recursive);
        for (const std::string& file : file_list) {
            self.files.push_back(file);
        }
        return self;
    }

    /**
     * @brief Add source files from a directory
     *
     * @param target_directory The directory to search for source files
     * @param recursive Whether to search recursively
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files("./src");
     * ```
     * @endcode
     */
    CommandBuilder& add_files(
        const std::string& target_directory, bool recursive = true) {
        std::function<bool(const std::string&)> file_predicate;

        if (self.language == Language::c) {
            file_predicate = is_c_file;
        } else {
            file_predicate = is_cpp_file;
        }

        const std::vector<std::string> file_list =
            readdir(target_directory, file_predicate, recursive);
        for (const std::string& file : file_list) {
            self.files.push_back(file);
        }
        return self;
    }

    /**
     * @brief Add source files from a directory with a custom file predicate
     *
     * @param target_directory The directory to search for source files
     * @param file_predicate A function that returns true if the file should be
     * included
     * @param recursive Whether to search recursively
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files("./src", [](const std::string& file) {
     *     return file.find(".cpp") != std::string::npos;
     * });
     * ```
     * @endcode
     */
    CommandBuilder& add_files(const char* target_directory,
        std::function<bool(const std::string&)> file_predicate,
        bool recursive = true) {
        const std::vector<std::string> file_list =
            readdir(target_directory, file_predicate, recursive);
        for (const std::string& file : file_list) {
            self.files.push_back(file);
        }
        return self;
    }

    /**
     * @brief Add source files from a directory with a custom file predicate
     *
     * @param target_directory The directory to search for source files
     * @param file_predicate A function that returns true if the file should be
     * included
     * @param recursive Whether to search recursively
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files("./src", [](const std::string& file) {
     *     return file.find(".cpp") != std::string::npos;
     * });
     * ```
     * @endcode
     */
    CommandBuilder& add_files(const std::string& target_directory,
        std::function<bool(const std::string&)> file_predicate,
        bool recursive = true) {
        const std::vector<std::string> file_list =
            readdir(target_directory, file_predicate, recursive);
        for (const std::string& file : file_list) {
            self.files.push_back(file);
        }
        return self;
    }

    /**
     * @brief Add source files from a list of files
     *
     * @param files  A list of source files
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files({"./src/main.cpp", "./src/utils.cpp"});
     * ```
     * @endcode
     */
    CommandBuilder& add_files(
        std::initializer_list<std::string> files) noexcept {
        self.files.reserve(self.files.size() + files.size());
        self.files.insert(self.files.end(), files.begin(), files.end());
        return self;
    }

    /**
     * @brief Add source files from a list of files
     *
     * @param files  A list of source files
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * std::vector<std::string> files = {"./src/main.cpp", "./src/utils.cpp"};
     * builder.add_files({"./src/main.cpp", "./src/utils.cpp"});
     * ```
     * @endcode
     */
    CommandBuilder& add_files(std::vector<std::string>&& files) noexcept {
        self.files.reserve(self.files.size() + files.size());
        self.files.insert(self.files.end(), files.begin(), files.end());
        return self;
    }

    /**
     * @brief Add source files from a list of files
     *
     * @param files  A list of source files
     * @return `nobpp::CommandBuilder&`
     * @code
     * ```cpp
     * builder.add_files({"./src/main.cpp", "./src/utils.cpp"});
     * ```
     * @endcode
     */
    CommandBuilder& add_files(const std::vector<std::string>& files) noexcept {
        self.files.reserve(self.files.size() + files.size());
        self.files.insert(self.files.end(), files.begin(), files.end());
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
    CommandBuilder& add_option(const char* opt) noexcept {
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
    CommandBuilder& add_option(const std::string& opt) noexcept {
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
    CommandBuilder& add_options(
        std::initializer_list<std::string>&& opts) noexcept {
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
    CommandBuilder& add_options(const std::vector<std::string>& opts) noexcept {
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
    CommandBuilder& set_build_dir(const std::string& dir) noexcept {
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
    CommandBuilder& set_output(const std::string& output) noexcept {
        std::string out = output;

        if (self.target_os == TargetOS::windows &&
            out.find(".exe") == std::string::npos) {
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

        switch (self.compiler) {
            case Compiler::clang:
                if (self.language == Language::c) {
                    command.push_back("clang");
                } else {
                    command.push_back("clang++");
                }
                break;
            case Compiler::gcc:
                if (self.language == Language::c) {
                    command.push_back("gcc");
                } else {
                    command.push_back("g++");
                }
                break;
        }

        switch (self.optimization_level) {
            case OptimizationLevel::o0:
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

        return join(command, ' ');
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
        Process process;
        process.set_command(command);

        create_process(command);

        if (self.project_name == "") {
            std::cout << "Compile finished\n";
        } else {
            std::cout << "Project " << self.project_name
                      << " compile finished\n";
        }
    }

private:
    CommandBuilder& self = *this;

    std::string project_name = "";
    Compiler compiler = Compiler::clang;
    Language language = Language::cpp;
    TargetOS target_os;
    OptimizationLevel optimization_level = OptimizationLevel::o3;
    std::vector<std::string> include_dirs;
    std::vector<std::string> files;
    std::vector<std::string> options;
    std::string build_dir;
    std::string output;
};

/**
 * @brief Creates a pool of process that will be used to run multiple processes
 * at once
 *
 */
class ProcessPool {};

/**
 * @brief Queue of commands that can run multiple commands in parallel
 * @code
 * ```cpp
 * #include "nobpp.hpp"
 *
 * int main() {
 *     const nobpp::CommandBuilder builder1 = nobpp::CommandBuilder();
 *     const nobpp::CommandBuilder builder2 = nobpp::CommandBuilder();
 *     const nobpp::CommandBuilder builder3 = nobpp::CommandBuilder();
 *     const nobpp::CommandBuilder builder4 = nobpp::CommandBuilder();
 *
 *     const nobpp::CommandQueue queue = nobpp::CommandQueue(4);
 *     queue.add_builder(builder1)
 *          .add_builder(builder2)
 *          .add_builder(builder3)
 *          .add_builder(builder4);
 * }
 * ```
 * @endcode
 */
class CommandQueue {
public:
    CommandQueue(size_t max_processes = 8) noexcept {
        self.workers.reserve(max_processes);

        for (size_t i = 0; i < max_processes; ++i) {
            self.workers.emplace_back([this]() { this->create_worker(); });
        }
    }
    CommandQueue(CommandQueue&) = delete;
    CommandQueue& operator=(CommandQueue&) = delete;

    ~CommandQueue() {
        self.all_finished = true;
        self.job_cv.notify_all();

        for (size_t i = 0; i < self.workers.size(); ++i) {
            self.workers[i].join();
        }
    }

    /**
     * @brief Add job to the queue
     *
     * @param builder
     * @return `CommandQueue&`
     */
    CommandQueue& add_builder(const CommandBuilder& builder) {
        if (self.all_finished) {
            std::cout << "Worker Pool disabled\n";
            return self;
        }
        {
            std::lock_guard<std::mutex> lock(self.job_mutex);
            self.queue.push(
                [&builder]() { create_process(builder.create_command()); });
        }
        self.job_cv.notify_one();

        return self;
    }

private:
    CommandQueue& self = *this;

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> queue;

    std::mutex job_mutex;
    std::condition_variable job_cv;

    bool all_finished = false;

private:
    void create_worker() {
        while (true) {
            std::unique_lock<std::mutex> lock(self.job_mutex);

            self.job_cv.wait(lock,
                [this]() { return !this->queue.empty() || self.all_finished; });

            if (self.all_finished && self.queue.empty()) {
                return;
            }

            std::function<void()> job = std::move(self.queue.front());
            self.queue.pop();
            lock.unlock();

            job();
        }
    }
};

}  // namespace nobpp
