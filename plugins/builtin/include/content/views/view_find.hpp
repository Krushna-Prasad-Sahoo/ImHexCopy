#pragma once

#include <hex.hpp>

#include <imgui.h>
#include <hex/ui/view.hpp>

#include <atomic>
#include <vector>

namespace hex::plugin::builtin {

    class ViewFind : public View {
    public:
        ViewFind();
        ~ViewFind() override = default;

        void drawContent() override;

    private:

        struct Occurrence {
            Region region;
            enum class DecodeType { ASCII, Binary, UTF16LE, UTF16BE } decodeType;
        };

        struct BinaryPattern {
            u8 mask, value;
        };

        struct SearchSettings {
            int range = 0;

            enum class Mode : int {
                Strings,
                Sequence,
                Regex,
                BinaryPattern
            } mode = Mode::Strings;

            struct Strings {
                int minLength = 5;
                enum class Type : int { ASCII = 0, UTF16LE = 1, UTF16BE = 2, ASCII_UTF16LE = 3, ASCII_UTF16BE = 4 } type = Type::ASCII;
                bool nullTermination = false;

                bool m_lowerCaseLetters = true;
                bool m_upperCaseLetters = true;
                bool m_numbers = true;
                bool m_underscores = true;
                bool m_symbols = true;
                bool m_spaces = true;
                bool m_lineFeeds = false;
            } strings;

            struct Bytes {
                std::string sequence;
            } bytes;

            struct Regex {
                std::string pattern;
            } regex;

            struct BinaryPattern {
                std::string input;
                std::vector<ViewFind::BinaryPattern> pattern;
            } binaryPattern;
        } m_searchSettings, m_decodeSettings;

        std::map<prv::Provider*, std::vector<Occurrence>> m_foundOccurrences, m_sortedOccurrences;
        std::atomic<bool> m_searchRunning;
        bool m_settingsValid = false;

        std::string m_currFilter;
    private:
        static std::vector<Occurrence> searchStrings(Task &task, prv::Provider *provider, Region searchRegion, SearchSettings::Strings settings);
        static std::vector<Occurrence> searchSequence(Task &task, prv::Provider *provider, Region searchRegion, SearchSettings::Bytes settings);
        static std::vector<Occurrence> searchRegex(Task &task, prv::Provider *provider, Region searchRegion, SearchSettings::Regex settings);
        static std::vector<Occurrence> searchBinaryPattern(Task &task, prv::Provider *provider, Region searchRegion, SearchSettings::BinaryPattern settings);

        static std::vector<BinaryPattern> parseBinaryPatternString(std::string string);

        void runSearch();
        std::string decodeValue(prv::Provider *provider, Occurrence occurrence) const;
    };

}