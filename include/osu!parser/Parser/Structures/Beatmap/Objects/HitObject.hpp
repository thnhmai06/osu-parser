#pragma once
#include <algorithm>
#include <optional>
#include <osu!parser/Parser/Utilities.hpp>

#include "TimingPoint.hpp"

using OsuParser::Utilities::IsBitEnabled;

namespace OsuParser::Beatmap::Objects::HitObject
{
	enum class HitObjectTypeBitmap : std::int32_t
	{
		HIT_CIRCLE = 1 << 0,
		SLIDER = 1 << 1,
		SPINNER = 1 << 3,
		HOLD_NOTE = 1 << 7, // osu!mania

		NEW_COMBO = 1 << 2,
		COLOR_JUMP0 = 1 << 4,
		COLOR_JUMP1 = 1 << 5,
		COLOR_JUMP2 = 1 << 6
	};
	enum class HitsoundBitmap : std::uint8_t
	{
		// https://osu.ppy.sh/wiki/en/Client/File_formats/osu_%28file_format%29#sliders:~:text=mania%20hold%20note.-,Hitsounds,-The%20hitSound%20bit

		NORMAL = 1 << 0,
		// below is additional
		WHISTLE = 1 << 1,
		FINISH = 1 << 2,
		CLAP = 1 << 3
	};

	struct Hitsound
	{
		bool Normal = false;
		bool Whistle = false;
		bool Finish = false;
		bool Clap = false;

		void Import(const std::int32_t HitSound)
		{
			Normal = IsBitEnabled(HitSound, static_cast<std::int32_t>(HitsoundBitmap::NORMAL));
			Whistle = IsBitEnabled(HitSound, static_cast<std::int32_t>(HitsoundBitmap::WHISTLE));
			Finish = IsBitEnabled(HitSound, static_cast<std::int32_t>(HitsoundBitmap::FINISH));
			Clap = IsBitEnabled(HitSound, static_cast<std::int32_t>(HitsoundBitmap::CLAP));
		}
		std::int32_t ToInt() const
		{
			std::int32_t Hitsound = 0;
			if (Normal)
				Hitsound |= static_cast<std::int32_t>(HitsoundBitmap::NORMAL);
			if (Whistle)
				Hitsound |= static_cast<std::int32_t>(HitsoundBitmap::WHISTLE);
			if (Finish)
				Hitsound |= static_cast<std::int32_t>(HitsoundBitmap::FINISH);
			if (Clap)
				Hitsound |= static_cast<std::int32_t>(HitsoundBitmap::CLAP);
			return Hitsound;
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
		std::string GetHitsoundTypeFilename(const HitsoundBitmap HitsoundType)
		{
			if (!Filename.empty())
			{
				// Only provide Normal hitsound
				if (HitsoundType == HitsoundBitmap::NORMAL)
					return Filename;
				return "";
			}

			// sampleSet
			const TimingPoint::HitSampleType SampleSet = (HitsoundType == HitsoundBitmap::NORMAL) ? NormalSet : AdditionSet;
			std::string SampleSetStr;
			switch (SampleSet)
			{
			case TimingPoint::HitSampleType::NORMAL:
				SampleSetStr = "normal";
				break;
			case TimingPoint::HitSampleType::SOFT:
				SampleSetStr = "soft";
				break;
			case TimingPoint::HitSampleType::DRUM:
				SampleSetStr = "drum";
				break;
			default:
				return ""; // No custom sampleSet, use skin hitsound instand
			}

			// hitSound
			std::string HitsoundTypeStr;
			switch (HitsoundType)
			{
			case HitsoundBitmap::WHISTLE:
				HitsoundTypeStr = "whistle";
				break;
			case HitsoundBitmap::FINISH:
				HitsoundTypeStr = "finish";
				break;
			case HitsoundBitmap::CLAP:
				HitsoundTypeStr = "clap";
				break;
			default:
				HitsoundTypeStr = "normal";
				break;
			}

			std::string Filename;
			Filename.append(SampleSetStr);
			Filename.append("-hit");
			Filename.append(HitsoundTypeStr);
			if (Index != 0 && Index != 1)
				Filename.append(std::to_string(Index));
			Filename.append(".wav");
			return Filename;
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
			HitCircle = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::HIT_CIRCLE));
			Slider = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::SLIDER));
			Spinner = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::SPINNER));
			HoldNote = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::HOLD_NOTE));
			IsNewCombo = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::NEW_COMBO));

			const int32_t bit4 = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::COLOR_JUMP0));
			const int32_t bit5 = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::COLOR_JUMP1));
			const int32_t bit6 = IsBitEnabled(Value, static_cast<std::int32_t>(HitObjectTypeBitmap::COLOR_JUMP2));
			ColourHax = (bit6 << 2) | (bit5 << 1) | bit4;
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