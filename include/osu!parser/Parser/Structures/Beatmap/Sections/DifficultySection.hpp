#pragma once
#include <vector>
#include "Section.hpp"

namespace OsuParser::Beatmap::Sections::Difficulty
{
    class DifficultySection final : public Section
    {
    public:
        DifficultySection() = default;
        void Parse(std::vector<std::string>& Lines) override
        {
            this->LoadAttributes(Lines);

            this->HPDrainRate = std::stod(this->GetAttribute("HPDrainRate"));
            this->CircleSize = std::stod(this->GetAttribute("CircleSize"));
            this->OverallDifficulty = std::stod(this->GetAttribute("OverallDifficulty"));
            this->ApproachRate = std::stod(this->GetAttribute("ApproachRate"));
            this->SliderMultiplier = std::stod(this->GetAttribute("SliderMultiplier"));
            this->SliderTickRate = std::stod(this->GetAttribute("SliderTickRate"));
        }

    public:
        double HPDrainRate;
        double CircleSize;
        double OverallDifficulty;
        double ApproachRate;
        double SliderMultiplier = 1.4;
        double SliderTickRate = 1.0;
    };
} // namespace Parser