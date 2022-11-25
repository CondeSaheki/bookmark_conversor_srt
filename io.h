#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace io
{
    namespace sfs = std::filesystem;
    
        /* In */
    
    template<typename type, typename char_type> [[nodiscard]] constexpr type input(std::basic_istream<char_type>& in = std::cin) noexcept
    {
        std::basic_stringstream<char_type> temp_ss;
        {
            char_type temp_char = 0;
            while (true)
            {
                if (in.get(temp_char) && temp_ss.put(temp_char))
                {

                    if (temp_char == std::use_facet<std::ctype<char_type>>(std::locale()).widen('\n'))
                    {
                        break; // means the end of the input
                    }
                    temp_char = 0;
                }
                else
                {
                    in.clear();
                    return type(); // in or ss got a bad fail
                }
            }
        }
        if ((temp_ss.str()).empty())
        {
            return type(); // is empty
        }
        temp_ss.seekg(-1, std::ios_base::end); // the end -1 of input sequence
        const std::streampos end = temp_ss.tellg();
        temp_ss.seekg(0);
        temp_ss.clear();
        type temp; // this may fail
        if (temp_ss >> temp)
        {
            if (temp_ss.tellg() != end)
            {
                return type(); // multiple entries is a bad input
            }
        }
        else
        {
            return type(); // is an incompatible type
        }
        return temp; // ok
    }

    template<typename char_type> [[nodiscard]] constexpr std::basic_string<char_type> input_line(std::basic_istream<char_type>& in = std::cin) noexcept
    {
        using type = std::basic_string<char_type>;
        std::basic_stringstream<char_type> temp_ss;
        {
            char_type temp_char = 0;
            while (true)
            {
                if (in.get(temp_char) && temp_ss.put(temp_char))
                {

                    if (temp_char == std::use_facet<std::ctype<char_type>>(std::locale()).widen('\n'))
                    {
                        break; // means the end of the input
                    }
                    temp_char = 0;
                }
                else
                {
                    in.clear();
                    return type(); // in or ss got a bad fail
                }
            }
        }
        if ((temp_ss.str()).empty())
        {
            return type(); // is empty
        }
        return temp_ss.str(); // ok
    }

    /* IN FILE */

    template<typename _Elem>
    [[nodiscard]] std::basic_string<_Elem> inputf(const sfs::path& file_path) noexcept
    {
        std::basic_string<_Elem> file;
        std::basic_ifstream<_Elem> fs;
        fs.open(file_path);
        if (fs.is_open())
        {
            _Elem caracter = 0;
            std::streampos end;
            fs.seekg(0, std::ios_base::end);
            end = fs.tellg();
            fs.seekg(0);
            fs.clear();
            file.reserve(static_cast<int>(end)); // alloc
            
            for (auto pos = 0; pos != static_cast<int>(end); ++pos) // fs.tellg() != end
            {
                //fs >> caracter
                if (caracter = fs.get())
                {
                    file += caracter;
                }
                else
                {
                    exit(1); // the end
                }
                caracter = 0;
            }
        }
        else
        {
            exit(1); // is not open
        }
        fs.close();
        fs.clear();
        return file;
    }

    /* OUT FILE */

    template<typename _Elem>
    void outputf(const sfs::path& file_path, const std::basic_string<_Elem>& str) noexcept
    {
        std::basic_ofstream<_Elem> fs;
        fs.open(file_path);
        if (fs.is_open())
        {
            fs.seekp(0, std::ios_base::end);
            fs.clear();
            if (!(fs << str))
            {
                exit(1);
            }
        }
        else
        {
            exit(1);
        }
        fs.close();
        fs.clear();
    }

}