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
#include "osu!parser/Parser/Utilities.hpp"

namespace OsuParser::Beatmap::Objects::Event
{
    namespace Type
    {
        enum class Event : uint8_t
        {
            Background = 0,
            Video = 1,
            Break = 2,
            Sprite, Animation, Sample
        };

        inline Event get_event_type_from_string(std::string str)
        {
            std::ranges::transform(str, str.begin(),
                                   [](const unsigned char c) { return std::tolower(c); });
            if (str == "animation")
                return Event::Animation;
            if (str == "sprite")
                return Event::Sprite;
            if (str == "sample")
                return Event::Sample;
            if (str == "break" || str == "2")
                return Event::Break;
            if (str == "background" || str == "0")
                return Event::Background;
            //if (str == "video" || str == "1")
            return Event::Video;
        }

        inline std::string to_string(const Event& type)
        {
            switch (type)
            {
            case Event::Background: return "0";
            case Event::Video: return "Video"; // or "1"
            case Event::Break: return "Break"; // or "2"
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

        namespace ObjectArgs
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

        namespace Commands
        {
            inline size_t get_line_depth(const std::string& raw_event_arg)
            {
                if (raw_event_arg.front() != ' ' || raw_event_arg.front() != '_') return 0;
                return std::min(
                    raw_event_arg.find_first_not_of(' '),
                    raw_event_arg.find_first_not_of('_')
                );
            }

            //! DO NOT USE THIS CLASS, ONLY FOR INHERITANCE
            struct BaseCommand
            {
                int32_t startTime = 0; // not default value
                [[nodiscard]] virtual std::string to_string(const uint32_t& depth) const = 0;

                virtual ~BaseCommand() = default;
            };

            using Command = std::unique_ptr<BaseCommand>;

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

            // TODO: Make shorthand Same Event, Same Duration, Sequentially:
            // https://osu.ppy.sh/wiki/en/Storyboard/Scripting/Shorthand#:~:text=storyboard%20commands.-,Same%20Event%2C%20Same%20Duration%2C%20Sequentially,-If%20you%20have

            struct FadeCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                float startOpacity, endOpacity;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "F," << easing << ',' << startTime << ',' << endTime << ','
                        << startOpacity << ',' << endOpacity;
                    return oss.str();
                }

                explicit FadeCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 5) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startOpacity = std::stof(line[4]);
                    endOpacity = std::stof(line[5]);
                }
            };

            struct MoveCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime, startX, startY, endX, endY;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "M," << easing << ',' << startTime << ',' << endTime << ','
                        << startX << ',' << startY << ',' << endX << ',' << endY;
                    return oss.str();
                }

                explicit MoveCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 6) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startX = std::stoi(line[4]);
                    startY = std::stoi(line[5]);
                    endX = std::stoi(line[6]);
                    endY = std::stoi(line[7]);
                }
            };

            struct MoveXCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime, startX, endX;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "MX," << easing << ',' << startTime << ',' << endTime << ','
                        << startX << ',' << endX;
                    return oss.str();
                }

                explicit MoveXCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 5) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startX = std::stoi(line[4]);
                    endX = std::stoi(line[5]);
                }
            };

            struct MoveYCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime, startY, endY;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "MY," << easing << ',' << startTime << ',' << endTime << ','
                        << startY << ',' << endY;
                    return oss.str();
                }

                explicit MoveYCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 5) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startY = std::stoi(line[4]);
                    endY = std::stoi(line[5]);
                }
            };

            struct ScaleCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                float startScale, endScale;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "S," << easing << ',' << startTime << ',' << endTime << ','
                        << startScale << ',' << endScale;
                    return oss.str();
                }

                explicit ScaleCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 5) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startScale = std::stof(line[4]);
                    endScale = std::stof(line[5]);
                }
            };

            struct VectorScaleCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                float startXScale, startYScale, endXScale, endYScale;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "V," << easing << ',' << startTime << ',' << endTime << ','
                        << startXScale << ',' << startYScale << ',' << endXScale << ',' << endYScale;
                    return oss.str();
                }

                explicit VectorScaleCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 7) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startXScale = std::stof(line[4]);
                    startYScale = std::stof(line[5]);
                    endXScale = std::stof(line[6]);
                    endYScale = std::stof(line[7]);
                }
            };

            struct RotateCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                float startRotate, EndRotate;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "R," << easing << ',' << startTime << ',' << endTime << ','
                        << startRotate << ',' << EndRotate;
                    return oss.str();
                }

                explicit RotateCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 5) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startRotate = std::stof(line[4]);
                    EndRotate = std::stof(line[5]);
                }
            };

            struct ColorCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                int32_t startR, startG, startB, endR, endG, endB;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "C," << easing << ',' << startTime << ',' << endTime << ','
                        << startR << ',' << startG << ',' << startB << ','
                        << endR << ',' << endG << ',' << endB;
                    return oss.str();
                }

                explicit ColorCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 9) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    startR = std::stoi(line[4]);
                    startG = std::stoi(line[5]);
                    startB = std::stoi(line[6]);
                    endR = std::stoi(line[7]);
                    endG = std::stoi(line[8]);
                    endB = std::stoi(line[9]);
                }
            };

            struct ParameterCommand final : BaseCommand
            {
                Easing::Easing easing;
                int32_t endTime;
                Parameter::Parameter parameter;

                [[nodiscard]] std::string to_string(const uint32_t& depth) const override
                {
                    std::ostringstream oss;
                    for (uint32_t current_depth = 1; current_depth <= depth; ++current_depth) oss << ' ';
                    oss << "P," << easing << ',' << startTime << ',' << endTime << ",\"" << parameter << "\"";
                    return oss.str();
                }

                explicit ParameterCommand(const std::vector<std::string>& line)
                {
                    easing = static_cast<Easing::Easing>(std::stoi(line[1]));
                    if (line.size() <= 4) return;
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                    parameter = static_cast<Parameter::Parameter>(line[4].front());
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

                explicit LoopCommand(const std::vector<std::string>& line)
                {
                    if (line.size() <= 2) return;
                    startTime = std::stoi(line[1]);
                    loopCount = std::stoi(line[2]);
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

                explicit TriggerCommand(const std::vector<std::string>& line)
                {
                    if (line.size() <= 3) return;
                    triggerType = line[1];
                    endTime = startTime = std::stoi(line[2]);
                    if (!line[3].empty()) endTime = std::stoi(line[3]);
                }
            };
        }
    }

    namespace Object
    {
        struct BaseEventObject
        {
            const Type::Event event;

            [[nodiscard]] virtual std::string to_string() const = 0;

            explicit BaseEventObject(const Type::Event& event) : event(event)
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
                oss << event << ',' << startTime << ',' << filename << ',' << x_offset << ',' << y_offset;
                return oss.str();
            }

            explicit BackgroundObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Background)
            {
                if (line.size() < 3) return;
                filename = Utilities::Trim(line[2], false, '"');
                if (line.size() > 3 && !line[3].empty()) x_offset = std::stoi(line[3]);
                if (line.size() > 4 && !line[4].empty()) y_offset = std::stoi(line[4]);
            }
        };

        struct VideoObject final : BaseEventObject
        {
            const Type::Event event = Type::Event::Video;
            int32_t startTime = 0;
            std::string filename;
            int32_t x_offset = 0, y_offset = 0;

            [[nodiscard]] std::string to_string() const override
            {
                std::ostringstream oss;
                oss << event << ',' << startTime << ',' << filename << ',' << x_offset << ',' << y_offset;
                return oss.str();
            }

            explicit VideoObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Video)
            {
                if (line.size() < 3) return;
                filename = Utilities::Trim(line[2], false, '"');
                if (line.size() > 3 && !line[3].empty()) x_offset = std::stoi(line[3]);
                if (line.size() > 4 && !line[4].empty()) y_offset = std::stoi(line[4]);
            }
        };

        struct BreakObject final : BaseEventObject
        {
            const Type::Event event = Type::Event::Break;
            int32_t startTime = 0, endTime = 0;

            [[nodiscard]] std::string to_string() const override
            {
                std::ostringstream oss;
                oss << event << ',' << startTime << ',' << endTime;
                return oss.str();
            }

            explicit BreakObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Break)
            {
                if (line.size() < 3) return;
                startTime = std::stoi(line[1]);
                endTime = std::stoi(line[2]);
            }
        };

        struct SpriteObject final : BaseEventObject
        {
            const Type::Event event = Type::Event::Sprite;
            Type::ObjectArgs::Layer::ImageLayer layer;
            Type::ObjectArgs::Origin::Origin origin;
            std::string filepath;
            int32_t x, y;
            Type::Commands::Commands commands;

            [[nodiscard]] std::string to_string() const override
            {
                std::ostringstream oss;
                oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
                oss << commands;

                return oss.str();
            }

            explicit SpriteObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Sprite)
            {
                if (line.size() <= 5) return;
                layer = Type::ObjectArgs::Layer::from_string(line[1]);
                origin = Type::ObjectArgs::Origin::from_string(line[2]);
                filepath = Utilities::Trim(line[3], false, '"');
                if (line.size() > 4 && !line[4].empty()) x = std::stoi(line[4]);
                if (line.size() > 5 && !line[5].empty()) y = std::stoi(line[5]);
            }
        };

        struct AnimationObject final : BaseEventObject
        {
            const Type::Event event = Type::Event::Animation;
            Type::ObjectArgs::Layer::ImageLayer layer;
            Type::ObjectArgs::Origin::Origin origin;
            std::string filepath;
            int32_t x, y;
            int32_t frameCount, frameDelay;
            Type::ObjectArgs::Loop::Loop looptype = Type::ObjectArgs::Loop::Loop::LoopForever;
            Type::Commands::Commands commands;

            [[nodiscard]] std::string to_string() const override
            {
                std::ostringstream oss;
                oss << event << ',' << layer << ',' << origin << ',' << '"' << filepath << '"' << ',' << x << ',' << y;
                oss << ',' << frameCount << ',' << frameDelay << ',' << looptype;
                oss << commands;

                return oss.str();
            }

            explicit AnimationObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Animation)
            {
                if (line.size() <= 7) return;
                layer = Type::ObjectArgs::Layer::from_string(line[1]);
                origin = Type::ObjectArgs::Origin::from_string(line[2]);
                filepath = Utilities::Trim(line[3], false, '"');
                x = std::stoi(line[4]);
                y = std::stoi(line[5]);
                frameCount = std::stoi(line[6]);
                frameDelay = std::stoi(line[7]);
                if (line.size() > 8 && !line[8].empty()) looptype = Type::ObjectArgs::Loop::from_string(line[8]);
            }
        };

        struct SampleObject final : BaseEventObject
        {
            const Type::Event event = Type::Event::Sample;
            int32_t time;
            Type::ObjectArgs::Layer::AudioLayer layer_num;
            std::string filepath;
            uint8_t volume = 100;

            [[nodiscard]] std::string to_string() const override
            {
                std::ostringstream oss;
                oss << event << ',' << time << ',' << layer_num << ',' << '"' << filepath << '"' << ',' << static_cast<
                    uint32_t>(volume);
                return oss.str();
            }

            explicit SampleObject(const std::vector<std::string>& line) : BaseEventObject(Type::Event::Sample)
            {
                if (line.size() < 4) return;
                time = std::stoi(line[1]);
                layer_num = Type::ObjectArgs::Layer::from_string(line[2]);
                filepath = Utilities::Trim(line[3], false, '"');
                if (line.size() > 4 && !line[4].empty()) volume = static_cast<uint8_t>(std::stoi(line[4]));
            }
        };

        struct EventObject
        {
            std::unique_ptr<BaseEventObject> object;

            [[nodiscard]] int32_t get_first_command_start_time(const bool is_commands_greater_sorted = false) const
            {
                switch (object->event)
                {
                case Type::Event::Sample:
                    return dynamic_cast<const SampleObject*>(object.get())->time;

                case Type::Event::Sprite:
                    {
                        const auto& sprite = dynamic_cast<const SpriteObject*>(object.get());
                        if (is_commands_greater_sorted)
                            return sprite->commands.commands.front()->startTime;
                        return std::ranges::min_element(sprite->commands.commands,
                                                        [](const Type::Commands::Command& a,
                                                           const Type::Commands::Command& b)
                                                        {
                                                            return a->startTime < b->startTime;
                                                        })->get()->startTime;
                    }
                case Type::Event::Animation:
                    {
                        const auto& sprite = dynamic_cast<const AnimationObject*>(object.get());
                        if (is_commands_greater_sorted)
                            return sprite->commands.commands.front()->startTime;
                        return std::ranges::min_element(sprite->commands.commands,
                                                        [](const Type::Commands::Command& a,
                                                           const Type::Commands::Command& b)
                                                        {
                                                            return a->startTime < b->startTime;
                                                        })->get()->startTime;
                    }

                case Type::Event::Background:
                    return dynamic_cast<const BackgroundObject*>(object.get())->startTime;
                case Type::Event::Video:
                    return dynamic_cast<const VideoObject*>(object.get())->startTime;
                case Type::Event::Break:
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
        std::vector<Object::EventObject> objects;

        void Parse(const std::vector<std::string>& lines, const bool sort = true)
        {
            std::stack<Type::Commands::Commands*> levels;
            for (auto ObjectString = lines.begin(); ObjectString != lines.end(); ++ObjectString)
            {
                std::vector<std::string> SplitObject = Utilities::Split(*ObjectString, ',');

                const auto depth = Type::Commands::get_line_depth(SplitObject.front());
                SplitObject.front().erase(0, depth); // remove all leading spaces
                while (depth < levels.size()) levels.pop();

                if (!levels.empty()) // => Command line
                {
                    auto& [commands] = *levels.top();
                    if (SplitObject[0] == "F") // Move
                        commands.emplace_back(std::make_unique<Type::Commands::FadeCommand>(SplitObject));
                    else if (SplitObject[0] == "M") // Move
                        commands.emplace_back(std::make_unique<Type::Commands::MoveCommand>(SplitObject));
                    else if (SplitObject[0] == "MX") // MoveX
                        commands.emplace_back(std::make_unique<Type::Commands::MoveXCommand>(SplitObject));
                    else if (SplitObject[0] == "MY") // MoveY
                        commands.emplace_back(std::make_unique<Type::Commands::MoveYCommand>(SplitObject));
                    else if (SplitObject[0] == "S") // Scale
                        commands.emplace_back(std::make_unique<Type::Commands::ScaleCommand>(SplitObject));
                    else if (SplitObject[0] == "V") // VectorScale
                        commands.emplace_back(std::make_unique<Type::Commands::VectorScaleCommand>(SplitObject));
                    else if (SplitObject[0] == "R") // Rotate
                        commands.emplace_back(std::make_unique<Type::Commands::RotateCommand>(SplitObject));
                    else if (SplitObject[0] == "C") // Color
                        commands.emplace_back(std::make_unique<Type::Commands::ColorCommand>(SplitObject));
                    else if (SplitObject[0] == "P") // Parameter
                        commands.emplace_back(std::make_unique<Type::Commands::ParameterCommand>(SplitObject));
                    else if (SplitObject[0] == "L") // Loop
                    {
                        commands.emplace_back(std::make_unique<Type::Commands::LoopCommand>(SplitObject));
                        levels.push(
                            &static_cast<Type::Commands::LoopCommand*>(commands.back().get())->commands);
                    }
                    else if (SplitObject[0] == "T") // Trigger
                    {
                        commands.emplace_back(std::make_unique<Type::Commands::TriggerCommand>(SplitObject));
                        levels.push(
                            &static_cast<Type::Commands::TriggerCommand*>(commands.back().get())->commands);
                    }
                }
                else // => Object line
                {
                    switch (Type::get_event_type_from_string(SplitObject[0])) // event
                    {
                    case Type::Event::Background:
                        objects.emplace_back(std::make_unique<Object::BackgroundObject>(SplitObject));
                        break;
                    case Type::Event::Video:
                        objects.emplace_back(std::make_unique<Object::VideoObject>(SplitObject));
                        break;
                    case Type::Event::Break:
                        objects.emplace_back(std::make_unique<Object::BreakObject>(SplitObject));
                        break;
                    case Type::Event::Sample:
                        objects.emplace_back(std::make_unique<Object::SampleObject>(SplitObject));
                        break;
                    case Type::Event::Sprite:
                        {
                            objects.emplace_back(std::make_unique<Object::SpriteObject>(SplitObject));
                            levels.push(&dynamic_cast<Object::SpriteObject*>(objects.back().object.get())->commands);
                        }
                        break;
                    case Type::Event::Animation:
                        {
                            objects.emplace_back(std::make_unique<Object::AnimationObject>(SplitObject));
                            levels.push(&dynamic_cast<Object::SpriteObject*>(objects.back().object.get())->commands);
                        }
                        break;
                    }
                }
            }

            if (sort)
            {
                // sort commands inside
                for (auto& object : objects)
                {
                    switch (object.object->event)
                    {
                    case Type::Event::Animation:
                        {
                            const auto animation = dynamic_cast<Object::AnimationObject*>(object.object.get());
                            std::ranges::sort(animation->commands.commands,
                                              [](const Type::Commands::Command& a, const Type::Commands::Command& b)
                                              {
                                                  return a->startTime < b->startTime;
                                              });
                        }
                        continue;

                    case Type::Event::Sprite:
                        {
                            const auto sprite = dynamic_cast<Object::SpriteObject*>(object.object.get());
                            std::ranges::sort(sprite->commands.commands,
                                              [](const Type::Commands::Command& a, const Type::Commands::Command& b)
                                              {
                                                  return a->startTime < b->startTime;
                                              });
                        }
                        continue;

                    default:
                        continue;
                    }
                }

                // sort object
                std::ranges::sort(objects,
                                  [](const Object::EventObject& a, const Object::EventObject& b)
                                  {
                                      if (a.object->event == b.object->event)
                                          return a.get_first_command_start_time(true) < b.get_first_command_start_time(true);
                                      return a.object->event < b.object->event;
                                  });
            }
        }
    };
}
