#pragma once

#include "common.h"

#include <string>
#include <unordered_set>

namespace GOB
{
	// a set of strings
	class Attributes
	{
	public:
		Attributes() = default;
		~Attributes() = default;

		Attributes(std::initializer_list<std::string> attributes);

		Attributes(const Attributes &other);
		Attributes(Attributes &&other) noexcept;

		Attributes &operator=(const Attributes &other);
		Attributes &operator=(Attributes &&other) noexcept;

		Attributes &add(const std::string &attribute);
		Attributes &add(const Attributes &attributes);
		Attributes &add(std::initializer_list<std::string> attributes);

		Attributes &remove(const std::string &attribute);
		Attributes &remove(const Attributes &attributes);
		Attributes &remove(std::initializer_list<std::string> attributes);

		// true if other is part of this
		bool contains(const std::string &other) const;
		bool contains(const Attributes &other) const;
		bool contains(std::initializer_list<std::string> other) const;

		bool empty() const;

	private:
		std::unordered_set<std::string> m_attributes;
	};
}
