// miscLIB By Rarksy: a small header-only library containing multiple functions i use in most of my projects

#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>
#include <ctime>
#pragma comment(lib, "urlmon.lib")

#include <codecvt>
#include <ShlObj_core.h>

#include "json.hpp"

namespace ml
{
    template <typename... Args>
    void log(Args&&... args)
    {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }

    template <typename T>
    std::string to_string(T content)
    {
        return std::to_string(content);
    }

    inline std::string to_string(const char* content)
    {
        return {content};
    }

    inline std::string to_lower(std::string content)
    {
        std::string temp = content;

        std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c) { return std::tolower(c); });

        return temp;
    }

    inline std::wstring string_t_wstring(const std::string& utf8String)
    {
        // Convert std::string to std::wstring
        std::wstring utf16String(utf8String.begin(), utf8String.end());
        return utf16String;
    }

    inline std::string wstring_to_string(const std::wstring& utf16String)
    {
        // Convert std::wstring to std::string
        std::string utf8String(utf16String.begin(), utf16String.end());
        return utf8String;
    }

    inline std::string wide_str_to_str(const std::wstring& wstr)
    {
        int len;
        int slength = (int)wstr.length() + 1;
        len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, 0, 0, 0, 0);
        char* buf = new char[len];
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, buf, len, 0, 0);
        std::string r(buf);
        delete[] buf;
        return r;
    }

    inline std::string open_file_dialog()
    {
        OPENFILENAME ofn;
        wchar_t szFile[260];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            return wide_str_to_str(ofn.lpstrFile);
        }
        else
        {
            return "";
        }
    }

    inline std::filesystem::path get_exe_directory()
    {
        char pathBuffer[MAX_PATH];
        GetModuleFileNameA(NULL, pathBuffer, MAX_PATH);

        return std::filesystem::path(pathBuffer);
    }

    inline bool file_or_directory_exists(const std::string& file_path)
    {
        return std::filesystem::exists(file_path);
    }

    inline bool create_file(const std::string& file_name)
    {
        if (file_or_directory_exists(file_name))
            return false;

        std::ofstream file_to_create(file_name);
        file_to_create.close();

        const bool created = file_or_directory_exists(file_name);

        return created;
    }

    inline bool create_directory(const std::string& directory_path)
    {
        if (file_or_directory_exists(directory_path))
            return false;

        return std::filesystem::create_directories(directory_path);
    }

    inline DWORD get_exe_pid(const std::wstring& processName)
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe32))
        {
            CloseHandle(hSnapshot);
            return 0;
        }

        do
        {
            if (processName == pe32.szExeFile)
            {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        }
        while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return 0;
    }

    inline bool is_exe_running(const std::wstring& processName)
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
            return false;

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (!Process32First(hSnapshot, &pe32))
        {
            CloseHandle(hSnapshot);
            return false;
        }

        do
        {
            if (processName == pe32.szExeFile)
            {
                CloseHandle(hSnapshot);
                return true;
            }
        }
        while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
        return false;
    }

    inline nlohmann::json json_get_data_from_file(const std::string& file_path)
    {
        std::ifstream file_to_read(file_path);
    
        if (file_to_read.is_open())
        {
            if (std::filesystem::is_empty(file_path))
                return nullptr;
    
            nlohmann::json data;
    
            file_to_read >> data;
    
            file_to_read.close();
    
            return data;
        }
    
        return nullptr;
    }

    inline bool json_write_data(const std::string& file_path, const nlohmann::json& json_data) {
        try {
            std::ofstream file_to_write(file_path, std::ios::out | std::ios::trunc);
            if (file_to_write.is_open()) {
                file_to_write << std::setw(4) << json_data << std::endl;
                file_to_write.close();
                return true;
            } else {
                std::cerr << "Error opening file for writing: " << file_path << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
            return false;
        }
    }
    
    inline std::string unix_format_number(time_t num)
    {
        return (num < 10 ? "0" + std::to_string(num) : std::to_string(num));
    }

    inline std::string unix_get_remaining_time(time_t unix_timestamp)
    {
        time_t currentTime;
        time(&currentTime);

        time_t remainingTime = unix_timestamp - currentTime;
        const time_t remainingDays = remainingTime / (24 * 3600);
        remainingTime = remainingTime % (24 * 3600);
        const time_t remainingHours = remainingTime / 3600;
        remainingTime %= 3600;
        const time_t remainingMinutes = remainingTime / 60;
        remainingTime %= 60;
        const time_t remainingSeconds = remainingTime;

        return std::string(
            unix_format_number(remainingDays) + ":" +
            unix_format_number(remainingHours) + ":" +
            unix_format_number(remainingMinutes) + ":" +
            unix_format_number(remainingSeconds)
        );
    }

    inline size_t write_data(void* ptr, size_t size, size_t nmemb, std::ofstream& stream)
    {
        stream.write(static_cast<const char*>(ptr), size * nmemb);
        return size * nmemb;
    }

    inline bool download_file(const std::string& url, const std::string& file_path)
    {
        const HRESULT result = URLDownloadToFileA(NULL, url.c_str(), file_path.c_str(), 0, NULL);

        return result == S_OK;
    }

    inline bool create_desktop_shortcut(const char* shortcutName) {

        if (FAILED(CoInitialize(NULL))) {
            return false;
        }
        
        char desktopPath[MAX_PATH];
        if (FAILED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath))) {
            CoUninitialize();
            return false;
        }
        
        char shortcutPath[MAX_PATH];
        snprintf(shortcutPath, MAX_PATH, "%s\\%s.lnk", desktopPath, shortcutName);
        
        IShellLinkA* pShellLink = NULL;
        HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&pShellLink);
        if (FAILED(hr)) {
            CoUninitialize();
            return false;
        }

        IPersistFile* pPersistFile = NULL;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
        if (FAILED(hr)) {
            pShellLink->Release();
            CoUninitialize();
            return false;
        }
        
        char pathBuffer[MAX_PATH];
        GetModuleFileNameA(NULL, pathBuffer, MAX_PATH);
        pShellLink->SetPath(std::string(std::filesystem::path(pathBuffer).string()).c_str());
        
        wchar_t wsz[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, shortcutPath, -1, wsz, MAX_PATH);
        hr = pPersistFile->Save(wsz, TRUE);
        
        pPersistFile->Release();
        pShellLink->Release();
        CoUninitialize();

        return SUCCEEDED(hr);
    }
}
