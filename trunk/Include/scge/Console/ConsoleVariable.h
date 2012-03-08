#ifndef __ConsoleVariable_h__
#define __ConsoleVariable_h__

#include "scge\Console\ConsoleCommand.h"
#include "scge\Utility.h"

#include <functional>
#include <type_traits>
#include <boost/format.hpp>

template <typename VariableType, bool useMinMax>
class ConsoleVariableMinMax
{
public:
	inline bool checkValue(const VariableType &newValue) { return true; }
};

template <typename VariableType>
class ConsoleVariableMinMax<VariableType, true>
{
public:
	static_assert(std::is_arithmetic<VariableType>::value, "Min and Max can only be used with arithmetic types");
	static_assert(!std::is_same<VariableType, bool>::value, "Using bool with Min and Max is pointless");
	ConsoleVariableMinMax()
		: mMinValue()
		, mMaxValue()
	{
	}
	
	ConsoleVariableMinMax(VariableType minValue, VariableType maxValue)
		: mMinValue(minValue)
		, mMaxValue(maxValue)
	{
		checkValues();
	}

	inline void setMinMaxValue(VariableType minValue, VariableType maxValue)
	{
		mMinValue = minValue;
		mMaxValue = maxValue;
		checkValues();
	}

	inline void setMinValue(VariableType minValue)
	{
		mMinValue = minValue;
		checkValues();
	}

	inline void setMaxValue(VariableType maxValue)
	{
		mMaxValue = maxValue;
		checkValues();
	}

	inline bool checkValue(VariableType newValue)
	{
		return newValue >= mMinValue && newValue <= mMaxValue;
	}

	inline VariableType getMinValue() const { return mMinValue; }
	inline VariableType getMaxValue() const { return mMaxValue; }

	inline void clampValue(VariableType &testValue) const
	{
		if(testValue > mMaxValue)
			testValue = mMaxValue;
		else if(testValue < mMinValue)
			testValue = mMinValue;
	}

private:
	VariableType mMinValue;
	VariableType mMaxValue;

	inline void checkValues()
	{
		if(mMinValue > mMaxValue)
			std::swap(mMinValue, mMaxValue);
	}
};

template <typename VariableType, bool useOnChange>
class ConsoleVariableOnChange
{
public:
	inline void operator()() {}
};

template <typename VariableType>
class ConsoleVariableOnChange<VariableType, true>
{
public:
	ConsoleVariableOnChange() {}
	ConsoleVariableOnChange(std::function<void()> onChange) : mOnChange(onChange) {}

	inline void setOnChange(std::function<void()> onChange)
	{
		mOnChange = onChange;
	}

	inline void operator()() { mOnChange(); }

private:
	std::function<void()> mOnChange;
};

template <typename VariableType, bool useMinMax = false, bool useOnChange = false>
class ConsoleVariable: public ConsoleCommand
{
public:
	ConsoleVariable(Console &console, std::string name, std::string comment, const VariableType &defaultValue)
		: ConsoleCommand(std::move(name), std::move(comment))
		, mCurrentValue(defaultValue)
		, mNewValue(defaultValue)
		, mDefaultValue(defaultValue)
		, mMinMax()
		, mOnChange()
	{
		static_assert(!useOnChange, "When using on change, pass in a function to call");
		static_assert(!useMinMax, "When using min and max, pass in their values");
		ConsoleCommand::initialise(console);
		updateValue();
	}

	ConsoleVariable(Console &console, std::string name, std::string comment, const VariableType &minValue, const VariableType &maxValue, const VariableType &defaultValue)
		: ConsoleCommand(std::move(name), std::move(comment))
		, mCurrentValue(defaultValue)
		, mNewValue(defaultValue)
		, mDefaultValue(defaultValue)
		, mMinMax(minValue, maxValue)
		, mOnChange()
	{
		static_assert(!useOnChange, "When using on change, pass in a function to call");
		static_assert(useMinMax, "When not using min and max, do not pass in their values");
		clampValues();

		ConsoleCommand::initialise(console);
		updateValue();
	}

	ConsoleVariable(Console &console, std::string name, std::string comment, const VariableType &defaultValue, std::function<void()> onChange)
		: ConsoleCommand(std::move(name), std::move(comment))
		, mCurrentValue(defaultValue)
		, mNewValue(defaultValue)
		, mDefaultValue(defaultValue)
		, mMinMax()
		, mOnChange(onChange)
	{
		static_assert(useOnChange, "When not using on change, don't pass in a function to call");
		static_assert(!useMinMax, "When using min and max, pass in their values");
		ConsoleCommand::initialise(console);
		updateValue();
	}

	ConsoleVariable(Console &console, std::string name, std::string comment, const VariableType &minValue, const VariableType &maxValue, const VariableType &defaultValue, std::function<void()> onChange)
		: ConsoleCommand(std::move(name), std::move(comment))
		, mCurrentValue(defaultValue)
		, mNewValue(defaultValue)
		, mDefaultValue(defaultValue)
		, mMinMax(minValue, maxValue)
		, mOnChange(onChange)
	{
		static_assert(useOnChange, "When not using on change, don't pass in a function to call");
		static_assert(useMinMax, "When not using min and max, do not pass in their values");
		clampValues();

		ConsoleCommand::initialise(console);
		updateValue();
	}

	ConsoleVariable(std::string name, std::string comment)
		: ConsoleCommand(std::move(name), std::move(comment))
		, mCurrentValue()
		, mNewValue()
		, mDefaultValue()
		, mOnChange()
		, mMinMax()
	{
	}

	virtual ~ConsoleVariable()
	{
		if(mNewValue != mDefaultValue)
		{
			const std::string &stringValue = StringUtility::toString(mNewValue);

			if(stringValue.find_first_of(" =\"'") == std::string::npos)
				storeCommand(mName + " = " + stringValue);
			else if(stringValue.find_first_of('\'') == std::string::npos)
				storeCommand(mName + " = '" + stringValue + "'");
			else
				storeCommand(mName + " = \"" + stringValue + "\"");
		}
	}

	bool hasValueChanged() const { return mNewValue != mCurrentValue; }

	operator VariableType() const { return mCurrentValue; }
	VariableType getValue() const { return mCurrentValue; }

	void updateValue() { mCurrentValue = mNewValue; }

	VariableType getMinValue() const { static_assert(useMinMax, "getMinValue() only available when using Min and Max"); return mMinMax.getMinValue(); }
	VariableType getMaxValue() const { static_assert(useMinMax, "getMaxValue() only available when using Min and Max"); return mMinMax.getMaxValue(); }

	void setMinValue(const VariableType &minValue) { static_assert(useMinMax, "setMinValue() only available when using Min and Max"); mMinMax.setMinValue(minValue); clampValues(); }
	void setMaxValue(const VariableType &maxValue) { static_assert(useMinMax, "setMinValue() only available when using Min and Max"); mMinMax.setMaxValue(maxValue); clampValues(); }
	void setMinMaxValue(const VariableType &minValue, const VariableType &maxValue) { static_assert(useMinMax, "setMinMaxValue() only available when using Min and Max"); mMinMax.setMinMaxValue(minValue, maxValue); clampValues(); }

	void setDefaultValue(const VariableType &defaultValue)
	{
		if(mNewValue == mDefaultValue)
		{
			mNewValue = defaultValue;
			if(!isInitialised())
				mCurrentValue = defaultValue;
		}
		mDefaultValue = defaultValue;
		
		clampValues();
	}

	bool setValue(VariableType newValue)
	{
		if(newValue == mNewValue)
			return true;

		if(!mMinMax.checkValue(newValue))
			return false;

		mNewValue = newValue;
		mOnChange();

		return true;
	}

	virtual void parse(const std::vector<std::string> &arguments, bool silent) final
	{
		if(arguments.size() == 2)
		{
			try
			{
				parseValue(arguments[1], silent);
			}
			catch(boost::bad_lexical_cast &)
			{
				if(!silent)
					mConsole->printError(StringUtility::format("Cannot convert % to correct type", arguments[1]));
			}
		}
		else if(!silent)
		{
			if(arguments.size() == 1)
				printCommand();
			else
				mConsole->printError("Too Many Arguments");
		}
	}

	bool initialise(Console &console, const VariableType &defaultValue)
	{
		static_assert(!useOnChange, "When using on change, pass in a function to call");
		static_assert(!useMinMax, "When using min and max, pass in their values");

		mCurrentValue = defaultValue;
		mNewValue = defaultValue;
		mDefaultValue = defaultValue;

		return ConsoleCommand::initialise(console);
		updateValue();
	}

	bool initialise(Console &console, const VariableType &minValue, const VariableType &maxValue, const VariableType &defaultValue)
	{
		static_assert(!useOnChange, "When using on change, pass in a function to call");
		static_assert(useMinMax, "When not using min and max, do not pass in their values");

		mCurrentValue = defaultValue;
		mNewValue = defaultValue;
		mDefaultValue = defaultValue;
		mMinMax.setMinMaxValue(minValue, maxValue);
		clampValues();

		return ConsoleCommand::initialise(console);
		updateValue();
	}

	bool initialise(Console &console, const VariableType &defaultValue, std::function<void()> onChange)
	{
		static_assert(useOnChange, "When not using on change, don't pass in a function to call");
		static_assert(!useMinMax, "When using min and max, pass in their values");

		mCurrentValue = defaultValue;
		mNewValue = defaultValue;
		mDefaultValue = defaultValue;
		mOnChange.setOnChange(onChange);

		return ConsoleCommand::initialise(console);
		updateValue();
	}

	bool initialise(Console &console, const VariableType &minValue, const VariableType &maxValue, const VariableType &defaultValue, std::function<void()> onChange)
	{
		static_assert(useOnChange, "When not using on change, don't pass in a function to call");
		static_assert(useMinMax, "When not using min and max, do not pass in their values");

		mCurrentValue = defaultValue;
		mNewValue = defaultValue;
		mDefaultValue = defaultValue;
		mMinMax.setMinMaxValue(minValue, maxValue);
		clampValues();
		mOnChange.setOnChange(onChange);

		return ConsoleCommand::initialise(console);
		updateValue();
	}

private:
	VariableType mCurrentValue;
	VariableType mNewValue;

	VariableType mDefaultValue;

	ConsoleVariableMinMax<VariableType, useMinMax> mMinMax;
	ConsoleVariableOnChange<VariableType, useOnChange> mOnChange;

	inline void parseValue(const std::string &stringValue, bool silent)
	{
		parseValue(stringValue, silent, std::integral_constant<bool, useMinMax>());
	}

	inline void parseValue(const std::string &stringValue, bool silent, std::false_type)
	{
		VariableType temp = stringValue == "default" ? mDefaultValue : StringUtility::toVariable<VariableType>(stringValue);

		if(!silent) mConsole->print(StringUtility::format("% = %", mName, temp));

		setValue(temp);
	}

	inline void parseValue(const std::string &stringValue, bool silent, std::true_type)
	{
		VariableType temp;
		if(stringValue == "default")
			temp = mDefaultValue;
		else if(stringValue == "min")
			temp = getMinValue();
		else if(stringValue == "max")
			temp = getMaxValue();
		else
			temp = StringUtility::toVariable<VariableType>(stringValue);

		if(!silent)
			mConsole->print(StringUtility::format("% = %", mName, temp));

		if(!setValue(temp) && !silent)
			mConsole->printError(StringUtility::format("The value % is outside the bounds % to %", temp, getMinValue(), getMaxValue()));
	}

	inline void printCommand()
	{
		printCommand(std::integral_constant<bool, useMinMax>());
	}

	inline void printCommand(std::false_type)
	{
		mConsole->print(StringUtility::format("% = %, Default = %", mName, mNewValue, mDefaultValue));
		if(!mComment.empty())
			mConsole->print(mComment);
	}

	inline void printCommand(std::true_type)
	{
		mConsole->print(StringUtility::format("% = %, Default = %, Min = %, Max = %", mName, mNewValue, mDefaultValue, getMinValue(), getMaxValue()));
		if(!mComment.empty())
			mConsole->print(mComment);
	}

	inline void clampValues()
	{
		clampValues(std::integral_constant<bool, useMinMax>());
	}

	inline void clampValues(std::false_type) {}

	inline void clampValues(std::true_type)
	{
		if(!isInitialised())
			mMinMax.clampValue(mCurrentValue);
		mMinMax.clampValue(mNewValue);
		mMinMax.clampValue(mDefaultValue);
	}
};

#endif // __ConsoleVariable_h__