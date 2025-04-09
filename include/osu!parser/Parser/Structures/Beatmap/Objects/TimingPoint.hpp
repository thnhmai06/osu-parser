#pragma once
#include <osu!parser/Parser/Utilities.hpp>

enum class EffectBitmap : std::int32_t
{
	KIAI = 1 << 0,
	OMIT_FIRST_BARLINE = 1 << 3,
};

namespace Parser
{
	enum class SampleSetType : std::int32_t
	{
		NO_CUSTOM = 0,
		NORMAL = 1,
		SOFT = 2,
		DRUM = 3
	};

    struct TimingPoint
    {
        struct Effect
        {
			bool kiai = false;
			bool omitFirstBarline = false; // osu!mania, osu!taiko

			void import(const std::int32_t effect)
			{
				kiai = Utilities::IsBitEnabled(effect, static_cast<std::int32_t>(EffectBitmap::KIAI));
				omitFirstBarline = Utilities::IsBitEnabled(effect, static_cast<std::int32_t>(EffectBitmap::OMIT_FIRST_BARLINE));
			}
			[[nodiscard]] std::int32_t to_int() const
			{
				std::int32_t effect = 0;
				if (kiai) effect |= static_cast<std::int32_t>(EffectBitmap::KIAI);
				if (omitFirstBarline) effect |= static_cast<std::int32_t>(EffectBitmap::OMIT_FIRST_BARLINE);
				return effect;
			}

			Effect() = default;
			Effect(const std::int32_t effect) { import(effect); }
        };

        std::int32_t Time;
        std::double_t BeatLength;
        std::int32_t Meter;
        SampleSetType SampleSet;
        std::int32_t SampleIndex;
        std::int32_t Volume;
        bool Uninherited;
        Effect Effects;

		bool operator< (const TimingPoint& other) const { return Time < other.Time; }
		bool operator> (const TimingPoint& other) const { return Time > other.Time; }
		bool operator== (const TimingPoint& other) const { return Time == other.Time; }
		bool operator!= (const TimingPoint& other) const { return Time != other.Time; }
		bool operator<= (const TimingPoint& other) const { return Time <= other.Time; }
		bool operator>= (const TimingPoint& other) const { return Time >= other.Time; }
    };

	struct TimingPoints : std::vector<TimingPoint>
	{
		void Parse(const std::vector<std::string>& lines, const bool sort = true)
		{
			for (const auto& line : lines)
			{
				TimingPoint point;
				const auto split = Utilities::Split(line, ',');
				point.Time = std::stoi(split[0]);
				point.BeatLength = std::stod(split[1]);
				point.Meter = std::stoi(split[2]);
				point.SampleSet = static_cast<SampleSetType>(std::stoi(split[3]));
				point.SampleIndex = std::stoi(split[4]);
				point.Volume = std::stoi(split[5]);
				point.Uninherited = (std::stoi(split[6]) == 1);
				if (split.size() >= 8) point.Effects.import(std::stoi(split[7]));
				this->push_back(point);
			}
			if (sort) std::ranges::sort(*this);
		}
	};
}