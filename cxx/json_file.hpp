//
// @author trimnalt AT gmail DOT com
// @version initial
// @date 2018-07-13
//

#ifndef ZBB_JSON_FILE_HPP
#define ZBB_JSON_FILE_HPP


#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include "./json.hpp"


namespace zbb {
struct json_file {
    json_file() = delete;
    ~json_file() = delete;

    static std::string load_bytes(std::string const& file) {
        std::ifstream ifs(file.c_str(), std::ios_base::binary);
        auto const ifs_size = size(ifs);
        if (0 == ifs_size || ifs_size > max_size) {
            return std::string{};
        }
        std::unique_ptr<char[]> buf{new char[ifs_size]};
        if (!ifs.read(buf.get(), ifs_size)) {
            return std::string{};
        }
        return std::string{buf.get(), ifs_size};
    }
    static nlohmann::json load_json(std::string const& file) {
        try {
            return nlohmann::json::parse(load_bytes(file));
        } catch(std::exception const& e) {
            std::cerr << e.what();
            return nlohmann::json{};
        }
    }
    static bool save_bytes(std::string const& file, std::string const& bytes) {
        if (!make(file)) {
            return false;
        }
        auto const tmp_file = file + ".tmp";
        return write(tmp_file, bytes) && rename(tmp_file, file);
    }
    static bool save_json(std::string const& file, nlohmann::json const& j) {
        if (j.is_null()) {
            return false;
        }
        try {
            return save_bytes(file, j.dump(4));
        } catch(std::exception const& e) {
            std::cerr << e.what();
            return false;
        }
    }
private:
    static bool make(std::string const& file) {
        namespace fs = ::std::experimental::filesystem;
        auto dir = fs::path(file);
        if (dir.empty()) {
            return true;
        }
        if (fs::exists(dir)) {
            return true;
        }
        if (dir.has_filename()) {
            dir.remove_filename();
        }
        if (dir.empty()) {
            return true;
        }
        if (fs::exists(dir)) {
            return true;
        }
        return fs::create_directories(dir);
    }
    static bool write(std::string const& file, std::string const& bytes) {
        try {
            if (bytes.empty()) {
                return false;
            }
            std::ofstream ofs{file.c_str(), std::ios_base::binary
                                          | std::ios_base::trunc};
            return static_cast<bool>(ofs.write(bytes.c_str(), bytes.size()));
        } catch(std::exception const& e) {
            std::cerr << e.what();
            return false;
        }
    }
    static bool rename(std::string const& from, std::string const& to) {
        namespace fs = ::std::experimental::filesystem;
        try {
            fs::rename(fs::path{from}, fs::path{to});
            return true;
        } catch (std::exception const& e) {
            std::cerr << e.what();
            return false;
        }
    }
    static std::size_t size(std::ifstream& ifs) {
        auto const pos = ifs.tellg();
        ifs.seekg(0, std::ios_base::end);
        auto const s = static_cast<std::size_t>(ifs.tellg());
        ifs.seekg(pos, std::ios_base::beg);
        return s;
    }
    static std::size_t size(std::ofstream& ofs) {
        auto const pos = ofs.tellp();
        ofs.seekp(0, std::ios_base::end);
        auto const s = static_cast<std::size_t>(ofs.tellp());
        ofs.seekp(pos, std::ios_base::beg);
        return s;
    }
private:
    static auto constexpr max_size = 1024u * 1024u;
}; // json_file
} // zbb


#endif // ZBB_JSON_FILE_HPP
