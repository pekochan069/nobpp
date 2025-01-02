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
#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace nobpp
{
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <process.h>
    #include <stdio.h>
    #include <strsafe.h>
    #include <tchar.h>
    #include <windows.h>
    #pragma comment(lib, "User32.lib")

    std::vector<std::string> readdir(const wchar_t* target_dir,
                                     std::function<bool(const std::string&)> file_predicate,
                                     std::string prefix = "") {
        std::vector<std::string> files;
        size_t length_of_arg;
        wchar_t szDir[MAX_PATH];
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;

        StringCchLengthW(target_dir, MAX_PATH, &length_of_arg);

        if (length_of_arg > (MAX_PATH - 3)) {
            std::wcout << "Directory path is too long." << target_dir << "\n";
            return files;
        }

        StringCchCopyW(szDir, MAX_PATH, target_dir);
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

                std::wstring wdir_name = std::wstring(target_dir) + L"\\" + ffd.cFileName;
                std::vector<std::string> files_in_dir =
                    readdir(wdir_name.c_str(), file_predicate, std::string(wdir_name.begin(), wdir_name.end()) + "\\");

                files.insert(files.end(), files_in_dir.begin(), files_in_dir.end());
            }

            const std::wstring wfile_name = ffd.cFileName;
            const std::string file_name(wfile_name.begin(), wfile_name.end());

            if (file_predicate(file_name)) {
                files.push_back(prefix + file_name);
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
        const wchar_t* wtarget_cstr = wtarget.c_str();
        return readdir(wtarget_cstr, file_predicate);
    }


    void run_command(const std::string& command) {
        _spawnlp(_P_WAIT, "powershell.exe", "powershell.exe", "-c", command.c_str(), NULL);
    }
#else
    #include <dirent.h>
    std::vector<std::string> readdir(std::string target) {
        return {};
    }

    void run_command(const std::string& command) {}
#endif

    enum struct Language { c, cpp };
    enum struct TargetOS { windows, linux };
    enum struct OptimizationLevel { none, o1, o2, o3, os, oz };

    bool is_c_file(const std::string& file) {
        return file.find(".c") != std::string::npos;
    }

    bool is_c_header_file(const std::string& file) {
        return file.find(".h") != std::string::npos;
    }

    bool is_cpp_file(const std::string& file) {
        static const std::unordered_set<std::string> cpp_extensions = {
            ".cpp", ".cc", ".c++", ".cxx", ".mpp", ".ipp", ".ixx", ".cppm"};
        size_t pos = file.rfind('.');
        if (pos == std::string::npos)
            return false;
        std::string ext = file.substr(pos);
        return cpp_extensions.find(ext) != cpp_extensions.end();
    }

    bool is_cpp_header_file(const std::string& file) {
        static const std::unordered_set<std::string> cpp_header_extensions = {
            ".h", ".hh", ".hpp", ".hxx", ".h++", ".inl"};
        size_t pos = file.rfind('.');
        if (pos == std::string::npos)
            return false;
        std::string ext = file.substr(pos);
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

    class CommandBuilder {
    public:
        CommandBuilder() = default;

        CommandBuilder& set_language(Language lang) {
            self.language = lang;
            return self;
        }

        CommandBuilder& set_target_os(TargetOS os) {
            self.target_os = os;
            return self;
        }

        CommandBuilder& set_optimization_level(OptimizationLevel level) {
            self.optimization_level = level;
            return self;
        }

        CommandBuilder& add_include_dir(const std::string& dir) {
            self.include_dir.push_back(dir);
            return self;
        }

        CommandBuilder& add_include_dirs(const std::vector<std::string>& dirs) {
            for (const std::string& dir : dirs) {
                self.include_dir.push_back(dir);
            }
            return self;
        }

        CommandBuilder& add_file(const std::string& file) {
            self.files.push_back(file);
            return self;
        }

        CommandBuilder& add_files(const std::string target) {
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

        CommandBuilder& add_options(const std::vector<std::string>& opts) {
            for (const std::string& opt : opts) {
                self.options.push_back(opt);
            }
            return self;
        }

        CommandBuilder& set_output(const std::string& output) {
            std::string out = output;

            if (self.target_os == TargetOS::windows && out.find(".exe") == std::string::npos) {
                out += ".exe";
            }

            self.output = out;

            return self;
        }

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

            if (self.output != "") {
                command.push_back("-o");
                command.push_back(output);
            }

            return join(command);
        }

        void run() const {
            const std::string command = create_command();
            run_command(command);
        }

    private:
        CommandBuilder& self = *this;

        Language language = Language::cpp;
        TargetOS target_os = TargetOS::windows;
        OptimizationLevel optimization_level = OptimizationLevel::o3;
        std::vector<std::string> include_dir;
        std::vector<std::string> files;
        std::vector<std::string> options;
        std::string output;
    };

}  // namespace nobpp
