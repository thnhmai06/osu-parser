#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <variant>
#include <sstream>

namespace Parser::Beatmap::Event
{
	// type
	struct Event {
		enum class Type { BasicImage, MovingImage, Audio };
		Type type;

		std::string to_string() const {
			switch (type) {
			case Type::MovingImage: return "Animation";
			case Type::BasicImage:  return "Sprite";
			case Type::Audio:       return "Sample";
			}
			return {};
		}
		friend std::ostream& operator<<(std::ostream& os, const Event& event)
		{
			return os << event.to_string();
		}

		Event(std::string str) 
		{
			std::transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c) { return std::tolower(c); });
			if (str == "sprite")
				type = Type::BasicImage;
			else if (str == "sample")
				type = Type::Audio;
			else
				type = Type::MovingImage; // if (str == "animation")
		}
		Event(Type type) : type(type) {}
		Event() = default;
	};

	// object
	struct BasicImageObject 
	{
		//! on object
		struct Layer {
			enum class Type : std::uint8_t
			{
				Background = 0, Fail = 1, Pass = 2, Foreground = 3
			};
			Type type;

			std::string to_string() const {
				switch (type) {
				case Type::Background: return "Background";
				case Type::Fail:       return "Fail";
				case Type::Pass:       return "Pass";
				case Type::Foreground: return "Foreground";
				}
				return {};
			}
			friend std::ostream& operator<<(std::ostream& os, const Layer& layer)
			{
				return os << layer.to_string();
			}
			Layer(std::string str) {
				std::transform(str.begin(), str.end(), str.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (str == "background")
					type = Type::Background;
				else if (str == "fail")
					type = Type::Fail;
				else if (str == "pass")
					type = Type::Pass;
				else
					type = Type::Foreground; // if (str == "foreground")
			}
			Layer(Type type) : type(type) {}
			Layer() = default;
		};
		struct Origin {
			enum class Type : std::uint8_t
			{
				TopLeft = 0, Centre = 1, CentreLeft = 2, TopRight = 3,
				BottomCentre = 4, TopCentre = 5, Custom = 6,
				CentreRight = 7, BottomLeft = 8, BottomRight = 9
			};

			Type type;
			std::string to_string() const {
				switch (type) {
				case Type::TopLeft:      return "TopLeft";
				case Type::Centre:       return "Centre";
				case Type::CentreLeft:   return "CentreLeft";
				case Type::TopRight:     return "TopRight";
				case Type::BottomCentre: return "BottomCentre";
				case Type::TopCentre:    return "TopCentre";
				case Type::Custom:       return "Custom";
				case Type::CentreRight:  return "CentreRight";
				case Type::BottomLeft:   return "BottomLeft";
				case Type::BottomRight:  return "BottomRight";
				}
				return {};
			}
			friend std::ostream& operator<<(std::ostream& os, const Origin& origin)
			{
				return os << origin.to_string();
			}
			Origin(std::string str) {
				std::transform(str.begin(), str.end(), str.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (str == "topleft")
					type = Type::TopLeft;
				else if (str == "centreleft")
					type = Type::CentreLeft;
				else if (str == "topright")
					type = Type::TopRight;
				else if (str == "bottomcentre")
					type = Type::BottomCentre;
				else if (str == "topcentre")
					type = Type::TopCentre;
				else if (str == "custom")
					type = Type::Custom;
				else if (str == "centreright")
					type = Type::CentreRight;
				else if (str == "bottomleft")
					type = Type::BottomLeft;
				else if (str == "bottomright")
					type = Type::BottomRight;
				else
					type = Type::Centre; // if (str == "centre")
			}
			Origin(Type type) : type(type) {}
			Origin() = default;
		} ;
		//! on Command
		struct Command;
		struct Commands : std::vector<Command>
		{
			std::string to_string(const uint32_t& depth = 1) const
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
		struct Easing
		{
			enum class Type : std::uint8_t
			{
				Linear = 0,
				EasingOut = 1,
				EasingIn = 2,
				QuadIn = 3,
				QuadOut = 4,
				QuadInOut = 5,
				CubicIn = 6,
				CubicOut = 7,
				CubicInOut = 8,
				QuartIn = 9,
				QuartOut = 10,
				QuartInOut = 11,
				QuintIn = 12,
				QuintOut = 13,
				QuintInOut = 14,
				SineIn = 15,
				SineOut = 16,
				SineInOut = 17,
				ExpoIn = 18,
				ExpoOut = 19,
				ExpoInOut = 20,
				CircIn = 21,
				CircOut = 22,
				CircInOut = 23,
				ElasticIn = 24,
				ElasticOut = 25,
				ElasticHalfOut = 26,
				ElasticQuarterOut = 27,
				ElasticInOut = 28,
				BackIn = 29,
				BackOut = 30,
				BackInOut = 31,
				BounceIn = 32,
				BounceOut = 33,
				BounceInOut = 34
			};

			Type type;
			friend std::ostream& operator<<(std::ostream& os, const Easing& easing)
			{
				return os << static_cast<uint32_t>(easing.type);
			}
		};
		struct Parameter 
		{
			enum class Type : char
			{
				Horizontal = 'H',
				Vertical = 'V',
				AdditiveColour = 'A'
			};
			Type type;
			friend std::ostream& operator<<(std::ostream& os, const Parameter& parameter)
			{
				return os << static_cast<char>(parameter.type);
			}
		};
		//? Normal
		struct FadeCommand {
			Easing easing;
			int32_t startTime, endTime;
			float startOpacity, endOpacity;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "F," << easing << ',' << startTime << ',' << endTime << ','
					<< startOpacity << ',' << endOpacity;
				return oss.str();
			}
		};
		struct MoveCommand {
			Easing easing;
			int32_t startTime, endTime, startX, startY, endX, endY;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "M," << easing << ',' << startTime << ',' << endTime << ','
					<< startX << ',' << startY << ',' << endX << ',' << endY;
				return oss.str();
			}
		};
		struct MoveXCommand {
			Easing easing;
			int32_t startTime, endTime, startX, endX;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "MX," << easing << ',' << startTime << ',' << endTime << ','
					<< startX << ',' << endX;
				return oss.str();
			}
		};
		struct MoveYCommand {
			Easing easing;
			int32_t startTime, endTime, startY, endY;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "MY," << easing << ',' << startTime << ',' << endTime << ','
					<< startY << ',' << endY;
				return oss.str();
			}
		};
		struct ScaleCommand {
			Easing easing;
			int32_t startTime, endTime;
			float startScale, endScale;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "S," << easing << ',' << startTime << ',' << endTime << ','
					<< startScale << ',' << endScale;
				return oss.str();
			}
		};
		struct VectorScaleCommand {
			Easing easing;
			int32_t startTime, endTime;
			float startXScale, startYScale, endXScale, endYScale;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "V," << easing << ',' << startTime << ',' << endTime << ','
					<< startXScale << ',' << startYScale << ',' << endXScale << ',' << endYScale;
				return oss.str();
			}
		};
		struct RotateCommand {
			Easing easing;
			int32_t startTime, endTime;
			float startRotate, EndRotate;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "R," << easing << ',' << startTime << ',' << endTime << ','
					<< startRotate << ',' << EndRotate;
				return oss.str();
			}
		};
		struct ColorCommand {
			Easing easing;
			int32_t startTime, endTime;
			int startR, startG, startB, endR, endG, endB;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "C," << easing << ',' << startTime << ',' << endTime << ','
					<< startR << ',' << startG << ',' << startB << ','
					<< endR << ',' << endG << ',' << endB;
				return oss.str();
			}
		};
		struct ParameterCommand {
			Easing easing;
			int startTime, endTime;
			Parameter parameter;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "P," << easing << ',' << startTime << ',' << endTime << ",\"" << parameter << "\"";
				return oss.str();
			}
		};
		//? Special
		struct LoopCommand {
			int startTime, loopCount;
			Commands commands;

			std::string to_string(const uint32_t& depth = 1) const 
			{
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
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
			int startTime, endTime;
			std::string triggerType;
			Commands commands;

			std::string to_string(const uint32_t& depth = 1) const {
				std::ostringstream oss;
				for (auto current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
				oss << "T," << triggerType << ',' << startTime << ',' << endTime;

				const auto new_depth = depth + 1;
				for (const auto& cmd : commands) {
					oss << '\n';
					oss << cmd.to_string(new_depth);
				}
				return oss.str();
			}
		};
		//? Total
		struct Command : std::variant<
			FadeCommand, MoveCommand, MoveXCommand, MoveYCommand,
			ScaleCommand, VectorScaleCommand, RotateCommand,
			ColorCommand, ParameterCommand, LoopCommand, TriggerCommand>
		{
			std::string to_string(const uint32_t& depth = 1) const {
				return std::visit(
					[depth](auto&& cmd) -> std::string
					{
						return cmd.to_string(depth);
					}, *this);
			}
		};
		
		Event event{ Event::Type::BasicImage };
		Layer layer;
		Origin origin;
		std::string filepath;
		int32_t x = 0, y = 0;
		Commands commands;

		virtual std::string to_string() const
		{
			std::ostringstream oss;
			oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
			oss << commands;

			return oss.str();
		}
	};
	struct MovingImageObject final : BasicImageObject 
	{
		struct Loop
		{
			enum class Type : bool { LoopForever = false, LoopOnce = true };

			Type type = Type::LoopForever;
			std::string to_string() const {
				switch (type) {
				case Type::LoopForever: return "LoopForever";
				case Type::LoopOnce:    return "LoopOnce";
				}
				return {};
			}
			friend std::ostream& operator<<(std::ostream& os, const Loop& loop)
			{
				return os << loop.to_string();
			}

			Loop(std::string str = {}) {
				if (str.empty())
					return;
				std::transform(str.begin(), str.end(), str.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (str == "looponce")
					type = Type::LoopOnce;
				else
					type = Type::LoopForever; // if (str == "loopforever")
			}
			Loop(Type type) : type(type) {}
			Loop() = default;
		};

		int32_t frameCount;
		int32_t frameDelay;
		Loop looptype;

		std::string to_string() const override
		{
			std::ostringstream oss;
			oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
			oss << ',' << frameCount << ',' << frameDelay << ',' << looptype;
			oss << commands;

			return oss.str();
		}
		MovingImageObject() : BasicImageObject(), looptype(Loop::Type::LoopForever)
		{
			event.type = Event::Type::MovingImage;
		}
	};
	struct AudioObject 
	{
		using Layer = BasicImageObject::Layer; // similar to BasicImageObject::Layer

		Event event{ Event::Type::Audio };
		int32_t time;
		Layer layer_num;
		std::string filepath;
		uint8_t volume = 100;

		std::string to_string() const
		{
			std::ostringstream oss;
			oss << event << ',' << time << ',' << layer_num << ',' << '"' << filepath << '"' << ',' << static_cast<uint32_t>(volume);
			return oss.str();
		}
	};

	// total
	struct EventObject : std::variant<BasicImageObject, MovingImageObject, AudioObject>
	{
		std::string to_string() const {
			return std::visit(
				[](auto&& obj) -> std::string
				{
					return obj.to_string();
				}, *this);
		}
	};
}