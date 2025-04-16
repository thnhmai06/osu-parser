#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <variant>
#include <sstream>

namespace Parser::Beatmap::Event
{
	namespace Type
	{
		enum class Event : uint8_t { Sprite, Animation, Sample };
		inline Event from_string(std::string str)
		{
			std::ranges::transform(str, str.begin(),
				[](const unsigned char c) { return std::tolower(c); });
			if (str == "sprite")
				return Event::Sprite;
			if (str == "animation")
				return Event::Animation;
			return Event::Sample;
			// if (str == "sample")
		}
		inline std::string to_string(const Event& type)
		{
			switch (type) {
			case Event::Sprite: return "Sprite";
			case Event::Animation: return "Animation";
			case Event::Sample: return "Sample";
			}
			return {};
		}
		inline std::ostream& operator<<(std::ostream& os, const Event& type)
		{
			return os << to_string(type);
		}

		namespace Object
		{
			namespace Layer
			{
				enum class ImageLayer : std::uint8_t
				{
					Background = 0, Fail = 1, Pass = 2, Foreground = 3
				};

				inline ImageLayer from_string(std::string str) {
					std::ranges::transform(str, str.begin(),
						[](const unsigned char c) { return std::tolower(c); });
					if (str == "background")
						return ImageLayer::Background;
					if (str == "fail")
						return ImageLayer::Fail;
					if (str == "pass")
						return ImageLayer::Pass;
					return ImageLayer::Foreground;
					// if (str == "foreground")
				}

				inline std::string to_string(const ImageLayer& layer) {
					switch (layer) {
					case ImageLayer::Background: return "Background";
					case ImageLayer::Fail:       return "Fail";
					case ImageLayer::Pass:       return "Pass";
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

				inline Origin from_string(std::string str) {
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

				inline std::string to_string(const Origin& origin) {
					switch (origin) {
					case Origin::TopLeft:      return "TopLeft";
					case Origin::Centre:       return "Centre";
					case Origin::CentreLeft:   return "CentreLeft";
					case Origin::TopRight:     return "TopRight";
					case Origin::BottomCentre: return "BottomCentre";
					case Origin::TopCentre:    return "TopCentre";
					case Origin::Custom:       return "Custom";
					case Origin::CentreRight:  return "CentreRight";
					case Origin::BottomLeft:   return "BottomLeft";
					case Origin::BottomRight:  return "BottomRight";
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
					LoopForever = 0, LoopOnce = 1
				};

				inline std::ostream& operator<<(std::ostream& os, const Loop& loop)
				{
					return os << static_cast<bool>(loop);
				}

				inline Loop from_string(std::string str) {
					std::ranges::transform(str, str.begin(),
						[](const unsigned char c) { return std::tolower(c); });
					if (str == "looponce")
						return Loop::LoopOnce;
					return Loop::LoopForever; // if (str == "loopforever")
				}
				inline std::string to_string(const Loop& loop) {
					switch (loop) {
					case Loop::LoopForever: return "LoopForever";
					case Loop::LoopOnce:    return "LoopOnce";
					}
					return {};
				}
			}
		}
		namespace Command
		{
			struct FadeCommand;
			struct MoveCommand;
			struct MoveXCommand;
			struct MoveYCommand;
			struct ScaleCommand;
			struct VectorScaleCommand;
			struct RotateCommand;
			struct ColorCommand;
			struct ParameterCommand;
			struct LoopCommand;
			struct TriggerCommand;
			struct Command : std::variant<
				FadeCommand, MoveCommand, MoveXCommand, MoveYCommand,
				ScaleCommand, VectorScaleCommand, RotateCommand,
				ColorCommand, ParameterCommand, LoopCommand, TriggerCommand>
			{
				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					return std::visit(
						[depth](auto&& cmd) -> std::string
						{
							return cmd.to_string(depth);
						}, *this);
				}
			};
			struct Commands : std::vector<Command>
			{
				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const
				{
					std::ostringstream oss;
					for (const auto& cmd : *this) {
						oss << '\n';
						oss << cmd.to_string(depth);
					}
					return oss.str();
				}
				friend std::ostream& operator<<(std::ostream& os, const Commands& commands)
				{
					return os << commands.to_string();
				}
			};

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

			struct FadeCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				float startOpacity, endOpacity;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "F," << easing << ',' << startTime << ',' << endTime << ','
						<< startOpacity << ',' << endOpacity;
					return oss.str();
				}
			};
			struct MoveCommand {
				Easing::Easing easing;
				int32_t startTime, endTime, startX, startY, endX, endY;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "M," << easing << ',' << startTime << ',' << endTime << ','
						<< startX << ',' << startY << ',' << endX << ',' << endY;
					return oss.str();
				}
			};
			struct MoveXCommand {
				Easing::Easing easing;
				int32_t startTime, endTime, startX, endX;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "MX," << easing << ',' << startTime << ',' << endTime << ','
						<< startX << ',' << endX;
					return oss.str();
				}
			};
			struct MoveYCommand {
				Easing::Easing easing;
				int32_t startTime, endTime, startY, endY;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "MY," << easing << ',' << startTime << ',' << endTime << ','
						<< startY << ',' << endY;
					return oss.str();
				}
			};
			struct ScaleCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				float startScale, endScale;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "S," << easing << ',' << startTime << ',' << endTime << ','
						<< startScale << ',' << endScale;
					return oss.str();
				}
			};
			struct VectorScaleCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				float startXScale, startYScale, endXScale, endYScale;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "V," << easing << ',' << startTime << ',' << endTime << ','
						<< startXScale << ',' << startYScale << ',' << endXScale << ',' << endYScale;
					return oss.str();
				}
			};
			struct RotateCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				float startRotate, EndRotate;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "R," << easing << ',' << startTime << ',' << endTime << ','
						<< startRotate << ',' << EndRotate;
					return oss.str();
				}
			};
			struct ColorCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				int32_t startR, startG, startB, endR, endG, endB;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "C," << easing << ',' << startTime << ',' << endTime << ','
						<< startR << ',' << startG << ',' << startB << ','
						<< endR << ',' << endG << ',' << endB;
					return oss.str();
				}
			};
			struct ParameterCommand {
				Easing::Easing easing;
				int32_t startTime, endTime;
				Parameter::Parameter parameter;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "P," << easing << ',' << startTime << ',' << endTime << ",\"" << parameter << "\"";
					return oss.str();
				}
			};
			struct LoopCommand {
				int32_t startTime, loopCount;
				Commands commands;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const
				{
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "L," << startTime << ',' << loopCount;

					const auto new_depth = depth + 1;
					for (const auto& cmd : commands) {
						oss << '\n';
						oss << cmd.to_string(new_depth);
					}
					return oss.str();
				}
			};
			struct TriggerCommand {
				int32_t startTime, endTime;
				std::string triggerType;
				Commands commands;

				[[nodiscard]] std::string to_string(const uint32_t& depth = 1) const {
					std::ostringstream oss;
					for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
					oss << "T," << triggerType << ',' << startTime << ',' << endTime;

					const auto new_depth = depth + 1;
					for (const auto& cmd : commands) {
						oss << '\n';
						oss << cmd.to_string(new_depth);
					}
					return oss.str();
				}
			};
		}
	}
	namespace Object
	{
		struct SpriteObject
		{
			const Type::Event event = Type::Event::Sprite;
			Type::Object::Layer::ImageLayer layer;
			Type::Object::Origin::Origin origin;
			std::string filepath;
			int32_t x = 0, y = 0;
			Type::Command::Commands commands;

			[[nodiscard]] std::string to_string() const
			{
				std::ostringstream oss;
				oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
				oss << commands;

				return oss.str();
			}
			friend std::ostream& operator<<(std::ostream& os, const SpriteObject& obj)
			{
				return os << obj.to_string();
			}
		};
		struct AnimationObject
		{
			const Type::Event event = Type::Event::Animation;
			Type::Object::Layer::ImageLayer layer;
			Type::Object::Origin::Origin origin;
			std::string filepath;
			int32_t x = 0, y = 0;
			Type::Command::Commands commands;
			int32_t frameCount, frameDelay;
			Type::Object::Loop::Loop looptype = Type::Object::Loop::Loop::LoopForever;

			[[nodiscard]] std::string to_string() const
			{
				std::ostringstream oss;
				oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
				oss << ',' << frameCount << ',' << frameDelay << ',' << looptype;
				oss << commands;

				return oss.str();
			}
			friend std::ostream& operator<<(std::ostream& os, const AnimationObject& obj)
			{
				return os << obj.to_string();
			}
		};
		struct SampleObject
		{
			const Type::Event event = Type::Event::Sample;
			int32_t time;
			Type::Object::Layer::AudioLayer layer_num;
			std::string filepath;
			uint8_t volume = 100;

			[[nodiscard]] std::string to_string() const
			{
				std::ostringstream oss;
				oss << event << ',' << time << ',' << layer_num << ',' << '"' << filepath << '"' << ',' << static_cast<uint32_t>(volume);
				return oss.str();
			}
			friend std::ostream& operator<<(std::ostream& os, const SampleObject& obj)
			{
				return os << obj.to_string();
			}
		};

		struct EventObject : std::variant<SpriteObject, AnimationObject, SampleObject>
		{
			[[nodiscard]] std::string to_string() const {
				return std::visit(
					[](auto&& obj) -> std::string
					{
						return obj.to_string();
					}, *this);
			}
		};
	}
}