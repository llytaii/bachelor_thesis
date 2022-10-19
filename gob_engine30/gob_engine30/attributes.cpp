#include "attributes.h"

namespace GOB
{
	Attributes::Attributes(std::initializer_list<std::string> attributes)
		: m_attributes{attributes}
	{
	}
	Attributes::Attributes(const Attributes &other)
	{
		*this = other;
	}
	Attributes::Attributes(Attributes &&other) noexcept
	{
		*this = std::move(other);
	}
	Attributes &Attributes::operator=(const Attributes &other)
	{
		if (this != &other)
		{
			m_attributes = other.m_attributes;
		}
		return *this;
	}
	Attributes &Attributes::operator=(Attributes &&other) noexcept
	{
		if (this != &other)
		{
			m_attributes = std::move(other.m_attributes);
		}
		return *this;
	}

	Attributes &Attributes::add(const std::string &attribute)
	{
		m_attributes.insert(attribute);
		return *this;
	}
	Attributes &Attributes::add(const Attributes &attributes)
	{
		for (const auto &attribute : attributes.m_attributes)
			add(attribute);
		return *this;
	}

	Attributes &Attributes::add(std::initializer_list<std::string> attributes)
	{
		for (auto &attribute : attributes)
			m_attributes.insert(attribute);
		return *this;
	}

	Attributes &Attributes::remove(const std::string &attribute)
	{
		m_attributes.erase(attribute);
		return *this;
	}
	Attributes &Attributes::remove(const Attributes &attributes)
	{
		for (const auto &attribute : attributes.m_attributes)
			remove(attribute);
		return *this;
	}

	Attributes &Attributes::remove(std::initializer_list<std::string> attributes)
	{
		for (auto &attribute : attributes)
			remove(attribute);
		return *this;
	}

	bool Attributes::contains(const std::string &attribute) const
	{
		return m_attributes.find(attribute) != m_attributes.end();
	}
	bool Attributes::contains(const Attributes &attributes) const
	{
		if (m_attributes.size() == 0)
			return true;
		for (const auto &attribute : attributes.m_attributes)
			if (m_attributes.find(attribute) == m_attributes.end())
				return false;
		return true;
	}

	bool Attributes::contains(std::initializer_list<std::string> attributes) const
	{
		if (m_attributes.size() == 0)
			return true;
		for (const auto &attribute : attributes)
			if (m_attributes.find(attribute) == m_attributes.end())
				return false;
		return true;
	}

	bool Attributes::empty() const
	{
		return m_attributes.size() == 0;
	}
}
