#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <iostream>
#include "io.h"

namespace sfs = std::filesystem;
namespace sch = std::chrono;
using char_type = char;
using string_type = std::basic_string<char_type>;
const char_type endl = '\n';

template<typename _type = string_type >
std::vector<_type> split(const _type& str, const _type& str2, typename _type::const_iterator start_position) noexcept
{
    std::vector<_type> splited;

    bool found = false;
    auto last_last = str.begin();
    auto it1 = start_position; //typename string_type::iterator
    auto it2 = str2.begin();
    auto first_found = start_position;

    while (true)
    {
        if(*it1 == *it2)
        {
            if(it2 != str2.end() -1)
            {
                ++it2;
            }
            else
            {
                splited.emplace_back(last_last, first_found);
                last_last = it1 +1;
                found = false;
                ++it1;
                it2 = str2.begin();
                first_found = it1;
                continue;
            }

            if(found == false)
            {
                found = true;
                first_found = it1;
            }
        }
        else
        {
           if(found == true)
            {
                found = false;
                it1 = first_found + 1;
                it2 = str2.begin();
                first_found = start_position;
            }
        }

        if(it1 != str.end())
        {
            ++it1;
        }
        else
        {
            // not found
            break;
        }
    }

    splited.emplace_back(_type(last_last, str.end()));

    return splited;
}

template<typename _type = string_type >
typename _type::iterator find(const _type& str, const _type& str2, typename _type::iterator start_position) noexcept
{
    bool found = false;
    auto it1 = start_position; //typename string_type::iterator
    auto it2 = str2.begin();
    auto first_found = start_position;

    while (true)
    {
        if(*it1 == *it2)
        {
            if(it2 != str2.end() -1)
            {
                ++it2;
            }
            else
            {
                return it1;
            }

            if(found == false)
            {
                found = true;
                first_found = it1;
            }
        }
        else
        {
           if(found == true)
            {
                found = false;
                it1 = first_found + 1;
                it2 = str2.begin();
                first_found = start_position;
            }
        }

        if(it1 != str.end())
        {
            ++it1;
        }
        else
        {
            // not found
            return it1;
        }
    }
}

template<typename char_type> void timestamp(std::basic_stringstream<char_type>& ss, const std::chrono::milliseconds& dur)
{

    auto hours = (sch::duration_cast<sch::hours>(abs(dur)));
    auto minutes = (sch::duration_cast<sch::minutes>(abs(dur) - hours));
    auto seconds = (sch::duration_cast<sch::seconds>(abs(dur) - hours - minutes));
    auto subseconds = (sch::duration_cast<sch::milliseconds>(abs(dur) - hours - minutes - seconds));

    if (hours.count() > 9)
    {
        ss << hours.count();
    }
    else
    {
        ss << '0' << hours.count();
    }
    ss << ':';

    if (minutes.count() > 9)
    {
        ss << minutes.count();
    }
    else
    {
        ss << '0' << minutes.count();
    }
    ss << ':';

    if (seconds.count() > 9)
    {
        ss << seconds.count();
    }
    else
    {
        ss << '0' << seconds.count();
    }
    ss << ',';

    if (subseconds.count() > 99)
    {
        ss << subseconds.count();
    }
    else
    {
        if (subseconds.count() > 9)
        {
            ss << '0' << subseconds.count();
        }
        else
        {
            ss << "00" << subseconds.count();
        }
    }
}


int main(int argc, char** argv)
{

    // cli args
    /*
        if(argc != 4)
        {
            std::cout << "usage: app [lyricsfile] [osufile] output_name\n";
            exit(0);
        }
    */

    sfs::path lyrics_file_path("lyrics.txt");
    sfs::path osu_file_path("osufile.osu");
    sfs::path output_file_path("lyrics.srt");

    // generate lyric

    std::vector<string_type> lyrics;
    {

        string_type lyrics_file = io::inputf<char_type>(lyrics_file_path);
        string_type str2 = "\n\n";
        lyrics = split(lyrics_file, str2, lyrics_file.begin());
    }

    // generate bookmarks

    std::vector<sch::milliseconds> bookmarks;
    {
        string_type line;
        {
            string_type osu_file = io::inputf<char_type>(osu_file_path);
            string_type str2 = "Bookmarks:";
            auto found = find(osu_file, str2, osu_file.begin()) +1;

            for(auto it = found; it != osu_file.end(); ++it)
            {
                if(*it == '\n')
                {
                    line = string_type(found, it -1);
                    break;
                }
            }
            for(auto it = line.begin(); it != line.end(); ++it)
            {
                if(*it == ',')
                {
                    *it =  ' ';
                }
            }
        }
        std::basic_stringstream<char_type> temp_ss(line);
        int temp_int;
        while(temp_ss >> temp_int)
        {

            bookmarks.emplace_back(sch::milliseconds(temp_int));
            temp_int = 0;
        }
    }



    // generate output srt
    if(lyrics.size() != bookmarks.size())
    {
        std::cout << "<warnning><lyrics|" << lyrics.size() << ">" << "<bookmarks|" << bookmarks.size() << ">\n";
    }

    string_type result;
    {
        std::basic_stringstream<char_type> ss;
        for(auto i = 0; (i != bookmarks.size() -1) && (i != lyrics.size() -1); ++i)
        {
            ss << i << endl;
            timestamp(ss, *(bookmarks.begin() + i) + sch::milliseconds(1));
            ss << " --> ";
            timestamp(ss, *(bookmarks.begin() + i + 1));
            ss << endl << *(lyrics.begin() + i) << "\n\n";
            result += ss.str();
            ss.clear();
            ss.str(string_type());
        }

        if(bookmarks.size() < lyrics.size())
        {
            ss << bookmarks.size() - 1 << endl;
        }
        else
        {
            ss << lyrics.size() - 1 << endl;
        }

        timestamp(ss, *(bookmarks.begin() + bookmarks.size() - 1) );
        ss << " --> ";
        timestamp(ss, *(bookmarks.begin() + bookmarks.size() - 1) + sch::milliseconds(500));
        ss << endl << *(lyrics.begin() + lyrics.size() - 1) << "\n\n";
        result += ss.str();
        ss.clear();
        ss.str(string_type());

    }

    // output file
    io::outputf(output_file_path, result);
}
