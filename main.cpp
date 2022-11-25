#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <iostream>
#include "io.h"

template<typename char_type = char> void timestamp(std::basic_stringstream<char_type>& ss, const std::chrono::milliseconds& dur);
template<typename char_type = char> void replace_all(std::basic_string<char_type>& in, const char_type replace, const char_type with);

int main(int argc, char** argv)
{   
    if(argc != 3)
    {
        std::cout << "usage: \n app.exe lyricsfile.txt osufile.osu ";
        exit(0);
    }

    // types
    using char_type = wchar_t;
    using num_type = int16_t;
    using string_type = std::basic_string<char_type>;
    namespace sch = std::chrono;
    const auto endl = L'\n';

    // get file path 
    auto lyrics_file = io::inputf<char_type>(argv[1]);
    auto osu_file = io::inputf<char_type>(argv[2]);

    // getting bookmarks from osu file
    std::vector<sch::milliseconds> bookmarks;
    {
        auto position_begin = osu_file.find(L"bookmarks: ") + 10;
        auto position_end= osu_file.find(endl, position_begin);
        string_type bookmarks_str(osu_file.begin() + position_begin, osu_file.begin() + position_end);
        replace_all(bookmarks_str, L',',L' ');
        std::basic_stringstream<char_type> ss(bookmarks_str);
        {
            num_type temp;
            while(ss >> temp)
            {
                bookmarks.emplace_back((temp));
                temp = 0;
            }
        }
    }
    
    // splitting lyrics file
    std::vector<string_type> lyrics; 
    {
        typename string_type::iterator position;
        for(auto i = lyrics_file.begin(); i != prev(lyrics_file.end()); ++i)
        {
            if(*i == endl && *(std::next(i)) == endl)
            {
                lyrics.emplace_back(position, i);
                position = std::next(i);
            }
        }
    }

    // error
    if(lyrics.size() != bookmarks.size())
    {
        std::cout << "error bookmarks and lyrics num are incompatible";
        exit(1);
    }

    // re create file
    string_type result;
    {
        std::basic_stringstream<char_type> ss;
        for(auto i = 0; i != bookmarks.size() -1; ++i)
        {
            ss << i << endl;
            timestamp(ss, *(bookmarks.begin() + i));
            ss << L" --> ";
            timestamp(ss, *(bookmarks.begin() + i + 1));
            ss << endl << *(lyrics.begin() + i) << L"\n\n";
            result += ss.str();
            ss.clear();
        }       
        ss << bookmarks.size() << endl;
        timestamp(ss, *(bookmarks.begin() + bookmarks.size()));
        ss << L" --> ";
        timestamp(ss, *(bookmarks.begin() + bookmarks.size()) + sch::milliseconds(2));
        ss << endl << *(lyrics.begin() + bookmarks.size()) << L"\n\n";
        result += ss.str();
        ss.clear();
    }

    // output file
    io::outputf(L"output.srt", result);
}

template<typename char_type = char> void replace_all(std::basic_string<char_type>& in, const char_type replace, const char_type with)
{
    for(auto i = in.begin(); i != in.end(); ++i)
    {
        if(*i == replace) 
        {
            *i = with;
        }
    }
}

template<typename char_type> void timestamp(std::basic_stringstream<char_type>& ss, const std::chrono::milliseconds& dur)
{
    auto hours = (duration_cast<sch::hours>(abs(dur)));
    auto minutes = (duration_cast<sch::minutes>(abs(dur) - hours));
    auto seconds = (duration_cast<sch::seconds>(abs(dur) - hours - minutes));
    auto subseconds = (duration_cast<sch::milliseconds>(abs(dur) - hours - minutes - seconds));

    if (hours.count() > 9)
    {
        ss << hours;
    }
    else
    {
        ss << '0' << hours;
    }
    ss << L':';

    if (minutes.count() > 9)
    {
        ss << minutes;
    }
    else
    {
        ss << L'0' << minutes;
    }
    ss << L':';

    if (seconds.count() > 9)
    {
        ss << seconds;
    }
    else
    {
        ss << L'0' << seconds;
    }
    ss << L',';
    
    if (subseconds.count() > 99)
    {
        ss << subseconds;
    }
    else
    {
        if (subseconds.count() > 9)
        {
            ss << L'0' << subseconds;
        }
        else
        {
            ss << L"00" << subseconds;
        }
    }
}