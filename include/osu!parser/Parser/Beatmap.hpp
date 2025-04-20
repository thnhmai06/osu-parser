#pragma once
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

#include "Structures/Beatmap/Sections/DifficultySection.hpp"
#include "Structures/Beatmap/Sections/EditorSection.hpp"
#include "Structures/Beatmap/Sections/GeneralSection.hpp"
#include "Structures/Beatmap/Sections/MetadataSection.hpp"
#include "Structures/Beatmap/Sections/ColourSection.hpp"
#include "Structures/Beatmap/Objects/HitObject.hpp"
#include "Structures/Beatmap/Objects/TimingPoint.hpp"
#include "Structures/Beatmap/Objects/Event.hpp"

namespace OsuParser::Beatmap
{
    static constexpr int MINIMUM_LINE_CHARACTERS = 3;

    class Beatmap
    {
    public:
        explicit Beatmap(const std::string& BeatmapPath) : m_CurrentStream(BeatmapPath)
        {
            this->Reset();
            if (!m_CurrentStream.good()) return;

            std::string CurrentLine;
            std::string CurrentSection = {};
            while (std::getline(m_CurrentStream, CurrentLine))
            {
                if (CurrentSection == "Events")
                {
                    CurrentLine = Utilities::Trim(CurrentLine, true);
                    if (const auto CurrentLineIfAllTrimmed = Utilities::Trim(CurrentLine);
                        CurrentLineIfAllTrimmed.size() >= 2 && CurrentLineIfAllTrimmed[0] == '/' &&
                        CurrentLineIfAllTrimmed[1] == '/') // is comment
                        continue;
                }
                else
                {
                    CurrentLine = Utilities::Trim(CurrentLine);
                    if (CurrentLine.size() >= 2 && CurrentLine[0] == '/' && CurrentLine[1] == '/') // is comment
                        continue;
                }

                if (!CurrentLine.empty() && CurrentLine.front() == '[' && CurrentLine.back() == ']')
                {
                    CurrentSection = Utilities::Split(Utilities::Split(CurrentLine, '[')[1], ']')[0];
                    continue;
                }
                // In >=C++11, std::string can save UTF-8 string (non-ascii char will be saved as 2 bytes, but in that one byte it looks weird)
                if (CurrentLine.size() < MINIMUM_LINE_CHARACTERS)
                    continue;

                this->m_Sections[CurrentSection].push_back(CurrentLine);
            }

            this->General.Parse(this->m_Sections["General"]);
            this->Metadata.Parse(this->m_Sections["Metadata"]);
            this->Editor.Parse(this->m_Sections["Editor"]);
            this->Difficulty.Parse(this->m_Sections["Difficulty"]);
            this->Colours.Parse(this->m_Sections["Colours"]);
            this->TimingPoints.Parse(this->m_Sections["TimingPoints"],
                                     !this->m_Sections["HitObjects"].empty() && !this->Difficulty.SliderMultiplier.
                                     empty());
            if (!this->Difficulty.SliderMultiplier.empty() && !TimingPoints.empty())
            {
                const double SliderMultiplier = std::stod(this->Difficulty.SliderMultiplier);
                this->HitObjects.Parse(this->m_Sections["HitObjects"], SliderMultiplier, this->TimingPoints);
            }
            else this->HitObjects.Parse(this->m_Sections["HitObjects"]);
            this->Events.Parse(this->m_Sections["Events"]);
        }

    private:
        void Reset()
        {
            Version = 14;
            TimingPoints.clear();
            HitObjects.clear();
            Events.objects.clear();
        }

    public:
        std::int32_t Version = 14;
        Sections::General::GeneralSection General;
        Sections::Metadata::MetadataSection Metadata;
        Sections::Editor::EditorSection Editor;
        Sections::Difficulty::DifficultySection Difficulty;
        Sections::Colour::ColourSection Colours;
        Objects::TimingPoint::TimingPoints TimingPoints;
        Objects::HitObject::HitObjects HitObjects;
        Objects::Event::Events Events;

    private:
        std::ifstream m_CurrentStream;
        std::unordered_map<std::string, std::vector<std::string>> m_Sections = {};
    };
}
