// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <variant>
#include <sstream>
#include <stack>
#include "osu!parser/Parser/Structures/Beatmap/Sections/VariableSection.hpp"
#include "osu!parser/Parser/Utilities.hpp"

namespace OsuParser::Beatmap::Objects::Event
{
	namespace Type
	{
		namespace Objects
		{
			enum class EventObjectType : uint8_t
			{
				Background = 0,
				Video = 1,
				Break = 2,
				Sprite, Animation, Sample
			};

			inline EventObjectType get_event_type_from_string(std::string str)
			{
				std::ranges::transform(str, str.begin(),
					[](const unsigned char c) { return std::tolower(c); });
				if (str == "animation")
					return EventObjectType::Animation;
				if (str == "sprite")
					return EventObjectType::Sprite;
				if (str == "sample")
					return EventObjectType::Sample;
				if (str == "break" || str == "2")
					return EventObjectType::Break;
				if (str == "background" || str == "0")
					return EventObjectType::Background;
				// if (str == "video" || str == "1")
				return EventObjectType::Video;
			}

			inline std::string to_string(const EventObjectType& type)
			{
				switch (type)
				{
				case EventObjectType::Background: return "0";
				case EventObjectType::Video: return "Video"; // or "1"
				case EventObjectType::Break: return "Break"; // or "2"
				case EventObjectType::Sprite: return "Sprite";
				case EventObjectType::Animation: return "Animation";
				case EventObjectType::Sample: return "Sample";
				}
				return {};
			}

			inline std::ostream& operator<<(std::ostream& os, const EventObjectType& type)
			{
				return os << to_string(type);
			}

			namespace Args
			{
				namespace Layer
				{
					enum class ImageLayer : std::uint8_t
					{
						Background = 0, Fail = 1, Pass = 2, Foreground = 3
					};

					inline ImageLayer from_string(std::string str)
					{
						std::ranges::transform(str, str.begin(),
							[](const unsigned char& c) { return std::tolower(c); });
						if (str == "background")
							return ImageLayer::Background;
						if (str == "fail")
							return ImageLayer::Fail;
						if (str == "pass")
							return ImageLayer::Pass;
						return ImageLayer::Foreground;
						// if (str == "foreground")
					}

					inline std::string to_string(const ImageLayer& layer)
					{
						switch (layer)
						{
						case ImageLayer::Background: return "Background";
						case ImageLayer::Fail: return "Fail";
						case ImageLayer::Pass: return "Pass";
						case ImageLayer::Foreground: return "Foreground";
						}
						return {};
					}

					inline std::ostream& operator<<(std::ostream& os, const ImageLayer& layer)
					{
						return os << to_string(layer);
					}

					using AudioLayer = ImageLayer;
				}

				namespace Origin
				{
					enum class Origin : std::uint8_t
					{
						TopLeft = 0, Centre = 1, CentreLeft = 2, TopRight = 3,
						BottomCentre = 4, TopCentre = 5, Custom = 6,
						CentreRight = 7, BottomLeft = 8, BottomRight = 9
					};

					inline Origin from_string(std::string str)
					{
						std::ranges::transform(str, str.begin(),
							[](const unsigned char c) { return std::tolower(c); });
						if (str == "topleft")
							return Origin::TopLeft;
						if (str == "centreleft")
							return Origin::CentreLeft;
						if (str == "topright")
							return Origin::TopRight;
						if (str == "bottomcentre")
							return Origin::BottomCentre;
						if (str == "topcentre")
							return Origin::TopCentre;
						if (str == "custom")
							return Origin::Custom;
						if (str == "centreright")
							return Origin::CentreRight;
						if (str == "bottomleft")
							return Origin::BottomLeft;
						return Origin::BottomRight;
						// if (str == "bottomright")
					}

					inline std::string to_string(const Origin& origin)
					{
						switch (origin)
						{
						case Origin::TopLeft: return "TopLeft";
						case Origin::Centre: return "Centre";
						case Origin::CentreLeft: return "CentreLeft";
						case Origin::TopRight: return "TopRight";
						case Origin::BottomCentre: return "BottomCentre";
						case Origin::TopCentre: return "TopCentre";
						case Origin::Custom: return "Custom";
						case Origin::CentreRight: return "CentreRight";
						case Origin::BottomLeft: return "BottomLeft";
						case Origin::BottomRight: return "BottomRight";
						}
						return {};
					}

					inline std::ostream& operator<<(std::ostream& os, const Origin& origin)
					{
						return os << to_string(origin);
					}
				}

				namespace Loop
				{
					enum class Loop : bool
					{
						LoopForever = false, // 0
						LoopOnce = true // 1
					};

					inline std::ostream& operator<<(std::ostream& os, const Loop& loop)
					{
						return os << static_cast<bool>(loop);
					}

					inline Loop from_string(std::string str)
					{
						std::ranges::transform(str, str.begin(),
							[](const unsigned char c) { return std::tolower(c); });
						if (str == "looponce")
							return Loop::LoopOnce;
						// if (str == "loopforever")
						return Loop::LoopForever;
					}

					inline std::string to_string(const Loop& loop)
					{
						switch (loop)
						{
						case Loop::LoopForever: return "LoopForever";
						case Loop::LoopOnce: return "LoopOnce";
						}
						return {};
					}
				}
			}
		}

		namespace Commands
		{
			enum class EventCommandType : std::uint8_t
			{
				Fade, Move, MoveX, MoveY, Scale, VectorScale,
				Rotate, Color, Parameter, Loop, Trigger
			};

			namespace Args
			{
				namespace Easing
				{
					enum class Easing : std::uint8_t
					{
						Linear = 0, EasingOut = 1, EasingIn = 2,
						QuadIn = 3, QuadOut = 4, QuadInOut = 5,
						CubicIn = 6, CubicOut = 7, CubicInOut = 8,
						QuartIn = 9, QuartOut = 10, QuartInOut = 11,
						QuintIn = 12, QuintOut = 13, QuintInOut = 14,
						SineIn = 15, SineOut = 16, SineInOut = 17,
						ExpoIn = 18, ExpoOut = 19, ExpoInOut = 20,
						CircIn = 21, CircOut = 22, CircInOut = 23,
						ElasticIn = 24, ElasticOut = 25,
						ElasticHalfOut = 26, ElasticQuarterOut = 27,
						ElasticInOut = 28,
						BackIn = 29, BackOut = 30, BackInOut = 31,
						BounceIn = 32, BounceOut = 33, BounceInOut = 34
					};

					inline std::ostream& operator<<(std::ostream& os, const Easing& easing)
					{
						return os << static_cast<uint32_t>(easing);
					}
				}

				namespace Parameter
				{
					enum class Parameter : char
					{
						Horizontal = 'H',
						Vertical = 'V',
						AdditiveColour = 'A'
					};

					inline std::ostream& operator<<(std::ostream& os, const Parameter& parameter)
					{
						return os << static_cast<char>(parameter);
					}
				}
			}
		}
	}

	namespace Commands
	{
		//! DO NOT USE THIS CLASS, ONLY FOR INHERITANCE
		struct BaseCommand
		{
			const Type::Commands::EventCommandType type;
			int32_t startTime = 0; // not default value
			[[nodiscard]] virtual std::string to_string(const uint32_t& depth) const = 0;

			explicit BaseCommand(const Type::Commands::EventCommandType& command_type) : type(command_type)
			{
			}
			virtual ~BaseCommand() = default;
		};

		using Command = std::shared_ptr<BaseCommand>;

		struct Commands
		{
			std::vector<Command> commands;

			[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const
			{
				std::ostringstream oss;
				for (const auto& command : commands)
				{
					oss << '\n';
					oss << command->to_string(depth);
				}
				return oss.str();
			}

			friend std::ostream& operator<<(std::ostream& os, const Commands& commands)
			{
				return os << commands.to_string();
			}

			Commands() = default;

			Commands(Commands&& other) noexcept
			{
				commands = std::move(other.commands);
			}

			Commands& operator=(Commands&& other) noexcept
			{
				if (this != &other)
				{
					commands = std::move(other.commands);
				}
				return *this;
			}
		};

		struct FadeCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			float startOpacity, endOpacity;
			std::vector<float> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "F," << easing << ',' << startTime << ',' << endTime << ','
					<< startOpacity << ',' << endOpacity;
				for (const auto& value : sequence)
					oss << ',' << value;
				return oss.str();
			}

			explicit FadeCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Fade)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endOpacity = startOpacity = std::stof(line[4]);
				if (line.size() > 5 && !line[5].empty()) endOpacity = std::stof(line[5]);
				else return;

				for (auto begin = line.begin() + 6; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(std::stof(*begin));
				}
			}
		};

		struct MoveCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime, startX, startY, endX, endY;
			std::vector<std::pair<int32_t, int32_t>> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "M," << easing << ',' << startTime << ',' << endTime << ','
					<< startX << ',' << startY << ',' << endX << ',' << endY;
				for (const auto& [x, y] : sequence)
					oss << ',' << x << ',' << y;
				return oss.str();
			}

			explicit MoveCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Move)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endX = startX = std::stoi(line[4]);
				endY = startY = std::stoi(line[5]);
				if (line.size() > 6 && !line[6].empty()) endY = std::stoi(line[6]);
				else return;
				if (line.size() > 7 && !line[7].empty()) endY = std::stoi(line[7]);
				else return;
				for (auto begin = line.begin() + 8; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.emplace_back(std::stoi(*begin), std::stoi(*(++begin)));
				}
			}
		};

		struct MoveXCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime, startX, endX;
			std::vector<int32_t> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "MX," << easing << ',' << startTime << ',' << endTime << ','
					<< startX << ',' << endX;
				for (const auto& x : sequence)
					oss << ',' << x;
				return oss.str();
			}

			explicit MoveXCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::MoveX)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endX = startX = std::stoi(line[4]);
				if (line.size() > 5 && !line[5].empty()) endX = std::stoi(line[5]);
				else return;

				for (auto begin = line.begin() + 6; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(std::stoi(*begin));
				}
			}
		};

		struct MoveYCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime, startY, endY;
			std::vector<int32_t> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "MY," << easing << ',' << startTime << ',' << endTime << ','
					<< startY << ',' << endY;
				for (const auto& y : sequence)
					oss << ',' << y;
				return oss.str();
			}

			explicit MoveYCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::MoveY)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endY = startY = std::stoi(line[4]);
				if (line.size() > 5 && !line[5].empty()) endY = std::stoi(line[5]);
				else return;

				for (auto begin = line.begin() + 6; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(std::stoi(*begin));
				}
			}
		};

		struct ScaleCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			float startScale, endScale;
			std::vector<float> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "S," << easing << ',' << startTime << ',' << endTime << ','
					<< startScale << ',' << endScale;
				for (const auto& value : sequence)
					oss << ',' << value;
				return oss.str();
			}

			explicit ScaleCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Scale)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endScale = startScale = std::stof(line[4]);
				if (line.size() > 5 && !line[5].empty()) endScale = std::stof(line[5]);
				else return;

				for (auto begin = line.begin() + 6; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(std::stof(*begin));
				}
			}
		};

		struct VectorScaleCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			float startXScale, startYScale, endXScale, endYScale;
			std::vector<std::pair<float, float>> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "V," << easing << ',' << startTime << ',' << endTime << ','
					<< startXScale << ',' << startYScale << ',' << endXScale << ',' << endYScale;
				for (const auto& [x, y] : sequence)
					oss << ',' << x << ',' << y;
				return oss.str();
			}

			explicit VectorScaleCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::VectorScale)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endXScale = startXScale = std::stof(line[4]);
				endYScale = startYScale = std::stof(line[5]);
				if (line.size() > 6 && !line[6].empty()) endYScale = std::stof(line[6]);
				else return;
				if (line.size() > 7 && !line[7].empty()) endYScale = std::stof(line[7]);
				else return;
				for (auto begin = line.begin() + 8; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.emplace_back(std::stof(*begin), std::stof(*(++begin)));
				}
			}
		};

		struct RotateCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			float startRotate, EndRotate;
			std::vector<float> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "R," << easing << ',' << startTime << ',' << endTime << ','
					<< startRotate << ',' << EndRotate;
				for (const auto& value : sequence)
					oss << ',' << value;
				return oss.str();
			}

			explicit RotateCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Rotate)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				EndRotate = startRotate = std::stof(line[4]);
				if (line.size() > 5 && !line[5].empty()) EndRotate = std::stof(line[5]);
				else return;

				for (auto begin = line.begin() + 6; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(std::stof(*begin));
				}
			}
		};

		struct ColorCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			int32_t startR, startG, startB, endR, endG, endB;
			std::vector<std::tuple<int32_t, int32_t, int32_t>> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "C," << easing << ',' << startTime << ',' << endTime << ','
					<< startR << ',' << startG << ',' << startB << ','
					<< endR << ',' << endG << ',' << endB;
				for (const auto& [r, g, b] : sequence)
					oss << ',' << r << ',' << g << ',' << b;
				return oss.str();
			}

			explicit ColorCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Color)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				endR = startR = std::stoi(line[4]);
				endG = startG = std::stoi(line[5]);
				endB = startB = std::stoi(line[6]);
				if (line.size() > 7 && !line[7].empty()) endR = std::stoi(line[7]);
				else return;
				if (line.size() > 8 && !line[8].empty()) endG = std::stoi(line[8]);
				else return;
				if (line.size() > 9 && !line[9].empty()) endB = std::stoi(line[9]);
				else return;
				for (auto begin = line.begin() + 10; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.emplace_back(std::stoi(*begin), std::stoi(*(++begin)), std::stoi(*(++begin)));
				}
			}
		};

		struct ParameterCommand final : BaseCommand
		{
			Type::Commands::Args::Easing::Easing easing;
			int32_t endTime;
			Type::Commands::Args::Parameter::Parameter parameter;
			std::vector<Type::Commands::Args::Parameter::Parameter> sequence;

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "P," << easing << ',' << startTime << ',' << endTime << ",\"" << parameter << "\"";
				for (const auto& value : sequence)
					oss << ',' << value;
				return oss.str();
			}

			explicit ParameterCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Parameter)
			{
				easing = static_cast<Type::Commands::Args::Easing::Easing>(std::stoi(line[1]));
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
				parameter = static_cast<Type::Commands::Args::Parameter::Parameter>(line[4].front());
				for (auto begin = line.begin() + 5; begin != line.end(); ++begin)
				{
					if (begin->empty()) continue;
					sequence.push_back(static_cast<Type::Commands::Args::Parameter::Parameter>(begin->front()));
				}
			}
		};

		struct LoopCommand final : BaseCommand
		{
			int32_t loopCount;
			Commands commands{};

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "L," << startTime << ',' << loopCount;

				const auto new_depth = depth + 1;
				for (const auto& command : commands.commands)
				{
					oss << '\n';
					oss << command->to_string(new_depth);
				}
				return oss.str();
			}

			explicit LoopCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Loop)
			{
				loopCount = std::stoi(line[1]);
				startTime = std::stoi(line[2]);
			}
		};

		struct TriggerCommand final : BaseCommand
		{
			int32_t endTime;
			std::string triggerType;
			Commands commands{};

			[[nodiscard]] std::string to_string(const uint32_t& depth) const override
			{
				std::ostringstream oss;
				for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "T," << triggerType << ',' << startTime << ',' << endTime;

				const auto new_depth = depth + 1;
				for (const auto& command : commands.commands)
				{
					oss << '\n';
					oss << command->to_string(new_depth);
				}
				return oss.str();
			}

			explicit TriggerCommand(const std::vector<std::string>& line) : BaseCommand(
				Type::Commands::EventCommandType::Trigger)
			{
				triggerType = line[1];
				endTime = startTime = std::stoi(line[2]);
				if (!line[3].empty()) endTime = std::stoi(line[3]);
			}
		};

		inline size_t get_line_depth(const std::string& raw_event_arg)
		{
			if (raw_event_arg.front() != ' ' && raw_event_arg.front() != '_') return 0;
			return std::max(
				raw_event_arg.find_first_not_of(' '),
				raw_event_arg.find_first_not_of('_')
			);
		}
	}

	namespace Objects
	{
		//! DO NOT USE THIS CLASS, ONLY FOR INHERITANCE
		struct BaseEventObject
		{
			const Type::Objects::EventObjectType type;

			[[nodiscard]] virtual std::string to_string() const = 0;

			explicit BaseEventObject(const Type::Objects::EventObjectType& event) : type(event)
			{
			}

			virtual ~BaseEventObject() = default;
		};

		struct BackgroundObject final : BaseEventObject
		{
			const int32_t startTime = 0;
			std::string filename;
			int32_t x_offset = 0, y_offset = 0;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << startTime << ',' << filename << ',' << x_offset << ',' << y_offset;
				return oss.str();
			}

			explicit BackgroundObject(const std::vector<std::string>& line) : BaseEventObject(
				Type::Objects::EventObjectType::Background)
			{
				if (line.size() < 3) return;
				filename = Utilities::Trim(line[2], false, '"');
				if (line.size() > 3 && !line[3].empty()) x_offset = std::stoi(line[3]);
				if (line.size() > 4 && !line[4].empty()) y_offset = std::stoi(line[4]);
			}
		};

		struct VideoObject final : BaseEventObject
		{
			int32_t startTime = 0;
			std::string filename;
			int32_t x_offset = 0, y_offset = 0;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << startTime << ',' << filename << ',' << x_offset << ',' << y_offset;
				return oss.str();
			}

			explicit VideoObject(const std::vector<std::string>& line) : BaseEventObject(Type::Objects::EventObjectType::Video)
			{
				if (line.size() < 3) return;
				filename = Utilities::Trim(line[2], false, '"');
				if (line.size() > 3 && !line[3].empty()) x_offset = std::stoi(line[3]);
				if (line.size() > 4 && !line[4].empty()) y_offset = std::stoi(line[4]);
			}
		};

		struct BreakObject final : BaseEventObject
		{
			int32_t startTime = 0, endTime = 0;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << startTime << ',' << endTime;
				return oss.str();
			}

			explicit BreakObject(const std::vector<std::string>& line) : BaseEventObject(Type::Objects::EventObjectType::Break)
			{
				if (line.size() < 3) return;
				startTime = std::stoi(line[1]);
				endTime = std::stoi(line[2]);
			}
		};

		struct SpriteObject final : BaseEventObject
		{
			Type::Objects::Args::Layer::ImageLayer layer;
			Type::Objects::Args::Origin::Origin origin;
			std::string filepath;
			int32_t x, y;
			Commands::Commands commands;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
				oss << commands;

				return oss.str();
			}

			explicit SpriteObject(const std::vector<std::string>& line) : BaseEventObject(Type::Objects::EventObjectType::Sprite)
			{
				if (line.size() <= 5) return;
				layer = Type::Objects::Args::Layer::from_string(line[1]);
				origin = Type::Objects::Args::Origin::from_string(line[2]);
				filepath = Utilities::Trim(line[3], false, '"');
				x = std::stoi(line[4]);
				y = std::stoi(line[5]);
			}
		};

		struct AnimationObject final : BaseEventObject
		{
			Type::Objects::Args::Layer::ImageLayer layer;
			Type::Objects::Args::Origin::Origin origin;
			std::string filepath;
			int32_t x, y;
			int32_t frameCount, frameDelay;
			Type::Objects::Args::Loop::Loop looptype = Type::Objects::Args::Loop::Loop::LoopForever;
			Commands::Commands commands;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
				oss << ',' << frameCount << ',' << frameDelay << ',' << looptype;
				oss << commands;

				return oss.str();
			}

			explicit AnimationObject(const std::vector<std::string>& line) : BaseEventObject(
				Type::Objects::EventObjectType::Animation)
			{
				if (line.size() <= 7) return;
				layer = Type::Objects::Args::Layer::from_string(line[1]);
				origin = Type::Objects::Args::Origin::from_string(line[2]);
				filepath = Utilities::Trim(line[3], false, '"');
				x = std::stoi(line[4]);
				y = std::stoi(line[5]);
				frameCount = std::stoi(line[6]);
				frameDelay = std::stoi(line[7]);
				if (line.size() > 8 && !line[8].empty()) looptype = Type::Objects::Args::Loop::from_string(line[8]);
			}
		};

		struct SampleObject final : BaseEventObject
		{
			int32_t time;
			Type::Objects::Args::Layer::AudioLayer layer_num;
			std::string filepath;
			uint8_t volume = 100;

			[[nodiscard]] std::string to_string() const override
			{
				std::ostringstream oss;
				oss << type << ',' << time << ',' << layer_num << ',' << '"' << filepath << '"' << ',' << static_cast<
					uint32_t>(volume);
				return oss.str();
			}

			explicit SampleObject(const std::vector<std::string>& line) : BaseEventObject(Type::Objects::EventObjectType::Sample)
			{
				if (line.size() < 4) return;
				time = std::stoi(line[1]);
				layer_num = Type::Objects::Args::Layer::from_string(line[2]);
				filepath = Utilities::Trim(line[3], false, '"');
				if (line.size() > 4 && !line[4].empty()) volume = static_cast<uint8_t>(std::stoi(line[4]));
			}
		};

		struct EventObject
		{
			std::shared_ptr<BaseEventObject> object;

			[[nodiscard]] int32_t get_first_command_start_time(const bool is_commands_greater_sorted = false) const
			{
				switch (object->type)
				{
				case Type::Objects::EventObjectType::Sample:
					return dynamic_cast<const SampleObject*>(object.get())->time;

				case Type::Objects::EventObjectType::Sprite:
				{
					const auto& sprite = dynamic_cast<const SpriteObject*>(object.get());
					if (is_commands_greater_sorted)
						return sprite->commands.commands.front()->startTime;
					return std::ranges::min_element(sprite->commands.commands,
						[](const Commands::Command& a,
							const Commands::Command& b)
						{
							return a->startTime < b->startTime;
						})->get()->startTime;
				}
				case Type::Objects::EventObjectType::Animation:
				{
					const auto& sprite = dynamic_cast<const AnimationObject*>(object.get());
					if (is_commands_greater_sorted)
						return sprite->commands.commands.front()->startTime;
					return std::ranges::min_element(sprite->commands.commands,
						[](const Commands::Command& a,
							const Commands::Command& b)
						{
							return a->startTime < b->startTime;
						})->get()->startTime;
				}

				case Type::Objects::EventObjectType::Background:
					return dynamic_cast<const BackgroundObject*>(object.get())->startTime;
				case Type::Objects::EventObjectType::Video:
					return dynamic_cast<const VideoObject*>(object.get())->startTime;
				case Type::Objects::EventObjectType::Break:
					return dynamic_cast<const BreakObject*>(object.get())->startTime;
				}
				return 0;
			}
		};

		template <typename EventObject>
		std::ostream& operator<<(std::ostream& os, const EventObject& object)
		{
			return os << object.to_string();
		}
	}

	struct Events
	{
		std::vector<Objects::EventObject> objects;

		void Parse(std::vector<std::string>& lines, const bool sort = true,
			const Sections::Variable::VariableSection& variables = {})
		{
			if (!variables.Variables.empty())
				for (auto& line : lines) variables.ProvideVariable(line);

			std::stack<Commands::Commands*> levels;
			for (auto& line : lines)
			{
				std::vector<std::string> SplitObject = Utilities::Split(line, ',');

				const auto depth = Commands::get_line_depth(SplitObject.front());
				SplitObject.front().erase(0, depth); // remove all leading spaces
				while (depth < levels.size()) levels.pop();

				if (!levels.empty()) // => Command line
				{
					auto& [commands] = *levels.top();
					if (SplitObject[0] == "F") // Fade
						commands.emplace_back(std::make_shared<Commands::FadeCommand>(SplitObject));
					else if (SplitObject[0] == "M") // Move
						commands.emplace_back(std::make_shared<Commands::MoveCommand>(SplitObject));
					else if (SplitObject[0] == "MX") // MoveX
						commands.emplace_back(std::make_shared<Commands::MoveXCommand>(SplitObject));
					else if (SplitObject[0] == "MY") // MoveY
						commands.emplace_back(std::make_shared<Commands::MoveYCommand>(SplitObject));
					else if (SplitObject[0] == "S") // Scale
						commands.emplace_back(std::make_shared<Commands::ScaleCommand>(SplitObject));
					else if (SplitObject[0] == "V") // VectorScale
						commands.emplace_back(std::make_shared<Commands::VectorScaleCommand>(SplitObject));
					else if (SplitObject[0] == "R") // Rotate
						commands.emplace_back(std::make_shared<Commands::RotateCommand>(SplitObject));
					else if (SplitObject[0] == "C") // Color
						commands.emplace_back(std::make_shared<Commands::ColorCommand>(SplitObject));
					else if (SplitObject[0] == "P") // Parameter
						commands.emplace_back(std::make_shared<Commands::ParameterCommand>(SplitObject));
					else if (SplitObject[0] == "L") // Loop
					{
						commands.emplace_back(std::make_shared<Commands::LoopCommand>(SplitObject));
						levels.push(
							&dynamic_cast<Commands::LoopCommand*>(commands.back().get())->commands);
					}
					else if (SplitObject[0] == "T") // Trigger
					{
						commands.emplace_back(std::make_shared<Commands::TriggerCommand>(SplitObject));
						levels.push(
							&dynamic_cast<Commands::TriggerCommand*>(commands.back().get())->commands);
					}
				}
				else // => Objects line
				{
					switch (Type::Objects::get_event_type_from_string(SplitObject[0])) // event
					{
					case Type::Objects::EventObjectType::Background:
						objects.emplace_back(std::make_shared<Objects::BackgroundObject>(SplitObject));
						break;
					case Type::Objects::EventObjectType::Video:
						objects.emplace_back(std::make_shared<Objects::VideoObject>(SplitObject));
						break;
					case Type::Objects::EventObjectType::Break:
						objects.emplace_back(std::make_shared<Objects::BreakObject>(SplitObject));
						break;
					case Type::Objects::EventObjectType::Sample:
						objects.emplace_back(std::make_shared<Objects::SampleObject>(SplitObject));
						break;
					case Type::Objects::EventObjectType::Sprite:
					{
						objects.emplace_back(std::make_shared<Objects::SpriteObject>(SplitObject));
						levels.push(&dynamic_cast<Objects::SpriteObject*>(objects.back().object.get())->commands);
					}
					break;
					case Type::Objects::EventObjectType::Animation:
					{
						objects.emplace_back(std::make_shared<Objects::AnimationObject>(SplitObject));
						levels.push(&dynamic_cast<Objects::SpriteObject*>(objects.back().object.get())->commands);
					}
					break;
					}
				}
			}

			if (sort)
			{
				// sort commands inside object
				for (auto& [object] : objects)
				{
					if (object->type == Type::Objects::EventObjectType::Animation)
					{
						const auto animation = dynamic_cast<Objects::AnimationObject*>(object.get());
						std::ranges::sort(animation->commands.commands,
							[](const Commands::Command& a,
								const Commands::Command& b)
							{
								return a->startTime < b->startTime;
							});
					}
					else if (object->type == Type::Objects::EventObjectType::Sprite)
					{
						const auto sprite = dynamic_cast<Objects::SpriteObject*>(object.get());
						std::ranges::sort(sprite->commands.commands,
							[](const Commands::Command& a,
								const Commands::Command& b)
							{
								return a->startTime < b->startTime;
							});
					}
				}

				// sort objects
				std::ranges::sort(objects,
					[](const Objects::EventObject& a, const Objects::EventObject& b)
					{
						if (a.object->type == b.object->type)
							return a.get_first_command_start_time(true) < b.get_first_command_start_time(
								true);
						return a.object->type < b.object->type;
					});
			}
		}
	};
}