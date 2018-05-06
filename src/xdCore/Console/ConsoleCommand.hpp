#pragma once

namespace XDay::Console
{
#pragma region ICommand
class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual void Execute() = 0;
    virtual void Execute(stringc&& args) = 0;

    virtual stringc Name() const = 0;

    virtual bool Enabled() const = 0;
    virtual void Enabled(const bool) = 0;

    virtual bool LowerCaseArguments() const = 0;
    virtual void LowerCaseArguments(const bool) = 0;

    virtual bool EmptyArgumentsHandled() const = 0;
    virtual void EmptyArgumentsHandled(const bool) = 0;

    virtual bool SaveAllowed() const = 0;
    virtual void SaveAllowed(const bool) = 0;


    virtual stringc Status() const = 0;
    virtual stringc Syntax() const = 0;
    virtual void InvalidSyntax(stringc&& args) const = 0;
    virtual stringc Save() const = 0;
    virtual stringc Help() const = 0;
    virtual stringc Type() = 0;
};
#pragma endregion ICommand

#pragma region CommandBase
template <typename type>
class CommandBase : public ICommand
{
protected:
    string name;
    string description;
    string localized_description;

    bool enabled;
    bool lowerCaseArguments;
    bool emptyArgumentsHandled;
    bool saveAllowed;

    type& value;

public:
    CommandBase() = delete;
    CommandBase(stringc&& name, stringc&& description, type& value)
        : name(std::move(name)),
          description(std::move(description)),
          enabled(true),
          lowerCaseArguments(false),
          emptyArgumentsHandled(false),
          saveAllowed(true),
          value(value) {}

    stringc Name() const override { return name; }

    bool Enabled() const override { return enabled; }
    void Enabled(const bool newValue) override { enabled = newValue; }

    bool LowerCaseArguments() const override { return lowerCaseArguments; }
    void LowerCaseArguments(const bool newValue) override { lowerCaseArguments = newValue; }

    bool EmptyArgumentsHandled() const override { return emptyArgumentsHandled; }
    void EmptyArgumentsHandled(const bool newValue) override { emptyArgumentsHandled = newValue; }

    bool SaveAllowed() const override { return saveAllowed; }
    void SaveAllowed(const bool newValue) override { saveAllowed = newValue; }

    type Value() const { return value; }
    void Value(const type newValue) { value = newValue; }

    void InvalidSyntax(stringc&& args) const override
    {
        Log::Warning("Invalid syntax in call [{} {}]", name, args);
        Log::Warning("Valid arguments: {}", Syntax());
    }

    stringc Save() const override { return name + " " + Status(); }

    constexpr stringc Type() override { return typeid(type).name(); }

    stringc Help() const override { return localized_description.empty() ? description : localized_description; }
};
#pragma endregion CommandBase

#pragma region Command<>
// You should instantiate template
// with the type you want to use before using it
template <typename type>
class Command : public CommandBase<type> {};
#pragma endregion Command<>

#pragma region Command<bool>
template <>
class Command<bool> : public CommandBase<bool>
{
public:
    Command(stringc&& name, stringc&& description, bool& value)
        : CommandBase(std::move(name), std::move(description), value)
    {
        CommandBase<bool>::LowerCaseArguments(true);
    }

    void Execute() override { Log::Warning("{} called, which shouldn't happen.", __FUNCTION__); }

    void Execute(stringc&& args) override
    {
        bool v;
        if (args == "on" || args == "true" || args == "1")
            v = true;
        else if (args == "off" || args == "false" || args == "0")
            v = false;
        else
        {
            InvalidSyntax(std::move(args));
            return;
        }
        value = v;
    }

    void RevertValue() { value = !value; }

    stringc Status() const override { return value ? "on" : "off"; }
    stringc Syntax() const override { return "on/off, true/false, 1/0"; }
};
#pragma endregion Command<bool>

#pragma region Command<int>
template <>
class Command<int> : public CommandBase<int>
{
    int min, max;

public:
    Command(stringc&& name, stringc&& description, int& value, const int min, const int max)
        : CommandBase(std::move(name), std::move(description), value), min(min), max(max) {}

    void Execute() override { Log::Warning("{} called, which shouldn't happen.", __FUNCTION__); }

    void Execute(stringc&& args) override
    {
        const auto v = std::stoi(args);

        if (v < min || v > max)
            InvalidSyntax(std::move(args));
        else
            value = v;
    }

    auto Min() const { return min; }
    void Min(const int newValue) { min = newValue; }

    auto Max() const { return max; }
    void Max(const int newValue) { max = newValue; }

    stringc Status() const override
    {
        return std::to_string(value);
    }

    stringc Syntax() const override
    {
        return fmt::format("range [{}, {}]", min, max);
    }
};
#pragma endregion Command<int>

#pragma region Command<float>
template <>
class Command<float> : public CommandBase<float>
{
    float min, max;

public:
    Command(stringc&& name, stringc&& description, float& value, const float min, const float max)
        : CommandBase(std::move(name), std::move(description), value), min(min), max(max) {}

    void Execute() override { Log::Warning("{} called, which shouldn't happen.", __FUNCTION__); }

    void Execute(stringc&& args) override
    {
        const auto v = stof(args);

        if (v < min || v > max)
            InvalidSyntax(std::move(args));
        else
            value = v;
    }

    auto Min() const { return min; }
    void Min(const float newValue) { min = newValue; }

    auto Max() const { return max; }
    void Max(const float newValue) { max = newValue; }

    stringc Status() const override
    {
        return std::to_string(value);
    }

    stringc Syntax() const override
    {
        return fmt::format("range [{}, {}]", min, max);
    }
};
#pragma endregion Command<float>

#pragma region Command<double>
template <>
class Command<double> : public CommandBase<double>
{
    double min, max;

public:
    Command(stringc&& name, stringc&& description, double& value, const double min, const double max)
        : CommandBase(std::move(name), std::move(description), value), min(min), max(max) {}

    void Execute() override { Log::Warning("{} called, which shouldn't happen.", __FUNCTION__); }

    void Execute(stringc&& args) override
    {
        const auto v = stof(args);

        if (v < min || v > max)
            InvalidSyntax(std::move(args));
        else
            value = v;
    }

    auto Min() const { return min; }
    void Min(const double newValue) { min = newValue; }

    auto Max() const { return max; }
    void Max(const double newValue) { max = newValue; }

    stringc Status() const override
    {
        return std::to_string(value);
    }

    stringc Syntax() const override
    {
        return fmt::format("range [{}, {}]", min, max);
    }
};
#pragma endregion Command<double>

#pragma region Command<string>
template <>
class Command<string> : public CommandBase<string>
{
    size_t maxSize;

public:
    Command(stringc&& name, stringc&& description, string& value, const size_t maxSize)
        : CommandBase(std::move(name), std::move(description), value), maxSize(maxSize) {}

    void Execute() override { Log::Warning("{} called, which shouldn't happen.", __FUNCTION__); }

    void Execute(stringc&& args) override
    {
        if (maxSize != 0 && args.length() > maxSize)
            InvalidSyntax(std::move(args));
        else
            value = std::move(args);
    }

    stringc Status() const override { return value; }
    stringc Syntax() const override
    {
        return maxSize == 0 ? "max size is not defined" : fmt::format("max size is {}", maxSize);
    }
};
#pragma endregion Command<string>

#pragma region Command<Call>
class Call
{
    using funcNoArgs = void(*)();
    using funcWithArgs = void(*)(stringc&& args);

    funcNoArgs noArgs;
    funcWithArgs withArgs;

public:
    Call(const funcNoArgs noArgs, const funcWithArgs withArgs): noArgs(noArgs), withArgs(withArgs) {}

    bool NoArgs() const { return noArgs != nullptr; }
    void NoArgs(nullptr_t) const { noArgs(); }

    bool WithArgs() const { return withArgs != nullptr; }
    void WithArgs(stringc&& args) const { withArgs(std::move(args)); }
};

template <>
class Command<Call> : public CommandBase<Call>
{
    stringc syntax;

public:
    Command(stringc&& name, stringc&& description, Call call,
        const bool lowerCaseArguments = true, stringc&& syntax = "no syntax")
        : CommandBase(std::move(name), std::move(description), call), syntax(syntax)
    {
        CommandBase<Call>::EmptyArgumentsHandled(call.NoArgs());
        CommandBase<Call>::LowerCaseArguments(lowerCaseArguments);
    }

    void Execute() override
    {
        if (value.NoArgs())
            value.NoArgs(nullptr);
    }

    void Execute(stringc&& args) override
    {
        if (value.WithArgs())
            value.WithArgs(std::move(args));
    }

    stringc Status() const override { return fmt::format("Can call: NoArgs({}), WithArgs({})", value.NoArgs(), value.WithArgs()); }
    stringc Syntax() const override { return syntax; }
};
#pragma endregion Command<Call>
} //namespace XDay::Console
