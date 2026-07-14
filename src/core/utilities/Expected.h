#pragma once

#include <variant>
#include <utility>

template<typename TValue, typename TError>
class Expected
{
public:
	Expected(const TValue& value);
	Expected(TValue&& value);

	Expected(const TError& error);
	Expected(TError&& error);


	bool HasValue() const;
	explicit operator bool() const;

	TValue& Value();
	const TValue& Value() const;

	TError& Error();
	const TError& Error() const;

private:
	std::variant<TValue, TError> m_data;
};

template<typename TValue, typename TError>
inline Expected<TValue, TError>::Expected(const TValue& value)
	: m_data(value)
{
}

template<typename TValue, typename TError>
inline Expected<TValue, TError>::Expected(TValue&& value)
	: m_data(std::move(value))
{
}

template<typename TValue, typename TError>
inline Expected<TValue, TError>::Expected(const TError& error)
	:m_data(error)
{
}

template<typename TValue, typename TError>
inline Expected<TValue, TError>::Expected(TError&& error)
	: m_data(std::move(error))
{
}

template<typename TValue, typename TError>
inline bool Expected<TValue, TError>::HasValue() const
{
	return std::holds_alternative<TValue>(m_data);
}

template<typename TValue, typename TError>
inline Expected<TValue, TError>::operator bool() const
{
	return HasValue();
}

template<typename TValue, typename TError>
inline TValue& Expected<TValue, TError>::Value()
{
	return std::get<TValue>(m_data);
}

template<typename TValue, typename TError>
inline const TValue& Expected<TValue, TError>::Value() const
{
	return std::get<TValue>(m_data);
}

template<typename TValue, typename TError>
inline TError& Expected<TValue, TError>::Error()
{
	return std::get<TError>(m_data);
}

template<typename TValue, typename TError>
inline const TError& Expected<TValue, TError>::Error() const
{
	return std::get<TError>(m_data);
}
