#pragma once
#include <algorithm>
#include <optional>
#include <bitset>
#include <osu!parser/Parser/Utilities.hpp>
#include "TimingPoint.hpp"

namespace OsuParser::Beatmap::Objects::HitObject
{
	struct Hitsound
	{
		bool Normal = false;
		bool Whistle = false;
		bool Finish = false;
		bool Clap = false;

		void Import(const std::int32_t HitSound)
		{
			const auto bitmap = std::bitset<4>(HitSound);

			Normal = bitmap[0];
			Whistle = bitmap[1];
			Finish = bitmap[2];
			Clap = bitmap[3];
		}
		[[nodiscard]] std::int32_t ToInt() const
		{
			auto bitmap = std::bitset<4>(0);
			bitmap[0] = Normal;
			bitmap[1] = Whistle;
			bitmap[2] = Finish;
			bitmap[3] = Clap;

			return static_cast<int32_t>(bitmap.to_ulong());
		}

		Hitsound() = default;
		Hitsound(const std::int32_t HitsoundId)
		{
			Import(HitsoundId);
		}
	};
	struct HitSample
	{
		// https://osu.ppy.sh/wiki/en/Client/File_formats/osu_%28file_format%29#hitsounds:~:text=enabled%20by%20default.-,Custom%20hit%20samples,-Usage%20of%20hitSample
	protected:
		static constexpr char DELIMETER = ':';

	public:
		TimingPoint::HitSampleType NormalSet = TimingPoint::HitSampleType::NO_CUSTOM;
		TimingPoint::HitSampleType AdditionSet = TimingPoint::HitSampleType::NO_CUSTOM;
		int Index = 0;
		int Volume = 0;
		std::string Filename;

		HitSample() = default;
		HitSample(const std::string& HitSampleStr)
		{
			if (HitSampleStr.empty())
				return; // not written
			const auto list = Utilities::Split(HitSampleStr, DELIMETER);
			NormalSet = static_cast<TimingPoint::HitSampleType>(std::stoi(list[0]));
			AdditionSet = static_cast<TimingPoint::HitSampleType>(std::stoi(list[1]));
			Index = std::stoi(list[2]);
			Volume = std::stoi(list[3]);
			if (list.size() > 4)
			{
				Filename = list[4];
			}
		}

		std::string ToString() const
		{
			std::string HitSampleStr;
			HitSampleStr.append(std::to_string(static_cast<std::int32_t>(NormalSet)));
			HitSampleStr.push_back(DELIMETER);
			HitSampleStr.append(std::to_string(static_cast<std::int32_t>(AdditionSet)));
			HitSampleStr.push_back(DELIMETER);
			HitSampleStr.append(std::to_string(Index));
			HitSampleStr.push_back(DELIMETER);
			HitSampleStr.append(std::to_string(Volume));
			HitSampleStr.push_back(DELIMETER);
			if (!Filename.empty()) HitSampleStr.append(Filename);
			return HitSampleStr;
		}
	};

	struct HitObjectType
	{
		bool HitCircle = false;
		bool Slider = false;
		bool Spinner = false;
		bool HoldNote = false; // osu!mania

		bool IsNewCombo = false;
		std::int32_t ColourHax = 0;

		void Import(const std::int32_t Value)
		{
			const auto bitset = std::bitset<8>(Value);
			HitCircle = bitset[0];
			Slider = bitset[1];
			Spinner = bitset[3];
			HoldNote = bitset[7];
			IsNewCombo = bitset[2];

			const auto bit4 = bitset[4];
			const auto bit5 = bitset[5];
			const auto bit6 = bitset[6];
			ColourHax = bit4 | bit5 | bit6;
		}

		HitObjectType() = default;
		HitObjectType(const std::int32_t Value)
		{
			Import(Value);
		}
	};
	struct HitObject
	{
		// https://osu.ppy.sh/wiki/en/Client/File_formats/osu_%28file_format%29#sliders:~:text=additional%20objectParams.-,Sliders,-Slider%20syntax%3A
		struct SliderParams
		{
			struct SliderCurve
			{
				enum class CurveType : char
				{
					BEZIER = 'B',  // bezier
					CATMULL = 'C', // centripetal catmull-rom
					LINEAR = 'L',
					PERFECT = 'P'
				};

				CurveType Type;
				std::vector<std::pair<std::int32_t, std::int32_t>> Points = {};

				void Import(const std::string& CurveString)
				{
					std::vector<std::string> Curve = Utilities::Split(CurveString, '|');

					// CurveType
					Type = static_cast<CurveType>(Curve.front().front());
					Curve.erase(Curve.begin());

					// CurvePoints (now Curve is became CurvePoints - aka CurvePoint list)
					for (const std::string& CurvePoint : Curve)
					{
						const std::vector<std::string> SplitPoint = Utilities::Split(CurvePoint, ':');
						Points.emplace_back(std::stoi(SplitPoint[0]), std::stoi(SplitPoint[1]));
					}
				}

				SliderCurve() = default;
				SliderCurve(const std::string& CurveString)
				{
					Import(CurveString);
				}
			};
			struct EdgeHitsound
			{
				struct SampleSet
				{
					TimingPoint::HitSampleType NormalSet = TimingPoint::HitSampleType::NO_CUSTOM;
					TimingPoint::HitSampleType AdditionSet = TimingPoint::HitSampleType::NO_CUSTOM;

					void Import(const std::string& EdgeSet)
					{
						if (EdgeSet.empty())
							return; // not written
						const auto list = Utilities::Split(EdgeSet, ':');
						NormalSet = static_cast<TimingPoint::HitSampleType>(std::stoi(list[0]));
						AdditionSet = static_cast<TimingPoint::HitSampleType>(std::stoi(list[1]));
					}

					SampleSet() = default;
					SampleSet(const std::string& EdgeSetStr)
					{
						Import(EdgeSetStr);
					}
				};

				Hitsound Sound;   // EdgeSounds
				SampleSet Sample; // EdgeSets
			};
			struct EdgeHitsounds : std::vector<EdgeHitsound>
			{
				void Import(const std::string& EdgeSoundsStr, const std::string& EdgeSetsStr)
				{
					const auto EdgeSounds = Utilities::Split(EdgeSoundsStr, '|');
					const auto EdgeSets = Utilities::Split(EdgeSetsStr, '|');

					for (std::int32_t i = 0; i < EdgeSounds.size(); i++)
					{
						EdgeHitsound HitSound;
						HitSound.Sound.Import(std::stoi(EdgeSounds[i]));
						HitSound.Sample.Import(EdgeSets[i]);

						this->push_back(HitSound);
					}
				}

				EdgeHitsounds() = default;
				EdgeHitsounds(const std::string& EdgeSoundsStr, const std::string& EdgeSetsStr)
				{
					Import(EdgeSoundsStr, EdgeSetsStr);
				}
			};

			std::int32_t Slides = 1;  // aka Repeats
			std::double_t Length = 0; // aka PixelLength
			SliderCurve Curve;
			EdgeHitsounds edgeHitsounds; // <edgeSounds + edgeSets> list
		};

		std::int32_t X = 0, Y = 0;
		std::int32_t Time = 0;
		HitObjectType Type;
		Hitsound Hitsound;
		std::optional<SliderParams> SliderParameters = SliderParams();
		std::optional<std::int32_t> EndTime = std::nullopt; // Bonus
		HitSample Hitsample;
	};

	struct HitObjects : std::vector<HitObject>
	{
		void Parse(const std::vector<std::string>& lines, const bool sort = true)
		{
			for (const std::string& ObjectString : lines)
			{
				HitObject Object;
				const auto SplitObject = Utilities::Split(ObjectString, ',');
				Object.X = std::stoi(SplitObject[0]);
				Object.Y = std::stoi(SplitObject[1]);
				Object.Time = std::stoi(SplitObject[2]);
				Object.Type = HitObjectType(std::stoi(SplitObject[3]));
				Object.Hitsound = Hitsound(std::stoi(SplitObject[4]));

				// Parsing objectParams
				/// as Slider
				if (Object.Type.Slider)
				{
					Object.SliderParameters->Curve.Import(SplitObject[5]);
					Object.SliderParameters->Slides = std::stoi(SplitObject[6]);
					Object.SliderParameters->Length = std::stod(SplitObject[7]);
					if (SplitObject.size() >= 10)
						Object.SliderParameters->edgeHitsounds.Import(SplitObject[8], SplitObject[9]);
					while (Object.SliderParameters->edgeHitsounds.size() < Object.SliderParameters->Slides)
						Object.SliderParameters->edgeHitsounds.emplace_back();
				}
				else
					Object.SliderParameters = std::nullopt;
				/// as Spinner
				if (Object.Type.Spinner)
				{
					Object.EndTime = std::stoi(SplitObject[5]);
				}
				// while HoldNote is special case
				if (Object.Type.HoldNote)
				{
					auto list = Utilities::Split(SplitObject[5], ':', true);
					Object.EndTime = std::stoi(list.front());
					Object.Hitsample = HitSample(list.back());
				}

				// Parsing hitSample for other
				if (!Object.Type.HoldNote)
				{
					if (Object.Type.Slider && SplitObject.size() >= 11)
					{
						Object.Hitsample = HitSample(SplitObject[10]);
					}
					else if (Object.Type.Spinner && SplitObject.size() >= 7)
					{
						Object.Hitsample = HitSample(SplitObject[6]);
					}
					else if (Object.Type.HitCircle && SplitObject.size() >= 6)
					{
						Object.Hitsample = HitSample(SplitObject[5]);
					}
				}

				this->push_back(Object);
			}

			if (sort)
				std::ranges::sort(*this, [](const HitObject& A, const HitObject& B) { return A.Time < B.Time; });
		}
		void Parse(
			// hit object will have endTime
			const std::vector<std::string>& lines,
			const double& SliderMultiplier,
			const TimingPoint::TimingPoints& sorted_timing_points)
		{
			Parse(lines, true);

			std::vector<TimingPoint::TimingPoint> UninheritedTimingPoints = {};
			std::vector<TimingPoint::TimingPoint> InheritedTimingPoints = {};
			for (const TimingPoint::TimingPoint& TimingPoint : sorted_timing_points)
			{
				if (TimingPoint.Uninherited)
					UninheritedTimingPoints.push_back(TimingPoint);
				else InheritedTimingPoints.push_back(TimingPoint);
			}

			// Calculate endTime
			for (auto& HitObject : *this)
			{
				if (!HitObject.Type.HoldNote)
				{
					if (HitObject.Type.HitCircle) { HitObject.EndTime = HitObject.Time; }
					else if (HitObject.Type.Slider && !UninheritedTimingPoints.empty()) {

						TimingPoint::TimingPoint currentTimeAsTimingPoint;
						currentTimeAsTimingPoint.Time = HitObject.Time;

						// Get BeatLength
						auto CurrentUninheritedTimingPoint =
							std::ranges::lower_bound(UninheritedTimingPoints, currentTimeAsTimingPoint);
						if (CurrentUninheritedTimingPoint == UninheritedTimingPoints.end())
							CurrentUninheritedTimingPoint = UninheritedTimingPoints.begin();
						const double_t BeatLength = CurrentUninheritedTimingPoint->BeatLength;

						// Get SV
						double_t SV;
						auto CurrentInheritedTimingPoint =
							std::ranges::lower_bound(InheritedTimingPoints, currentTimeAsTimingPoint);
						if (CurrentInheritedTimingPoint == InheritedTimingPoints.end()) SV = 1;
						else SV = SliderMultiplier * (100.0 / std::abs(CurrentInheritedTimingPoint->BeatLength));

						HitObject.EndTime =
							HitObject.Time + static_cast<int32_t>(HitObject.SliderParameters->Length / (SliderMultiplier * 100 * SV) * BeatLength
								* HitObject.SliderParameters->Slides);

					}
				}
			}
		}
	};
} // namespace Parser