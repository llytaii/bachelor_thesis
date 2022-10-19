#pragma once

#include "common.h"

#include "action.h"
#include "attributes.h"
#include "reasoner.h"

#include <memory>
#include <tuple>
#include <unordered_set>

namespace GOB
{
	class Broadcaster
	{
	public:
		// getter
		static Broadcaster &get_instance();

		// etc
		ACTION add_action(const Action &action, const Attributes &attributes = {});
		ACTION add_action(ACTION action, const Attributes &attributes = {});
		void remove_action(ACTION action, const Attributes &attributes = {});

		void add_reasoner(Reasoner *reasoner, const Attributes &attributes = {});
		void remove_reasoner(Reasoner *reasoner);

	private:
		struct ActionAttributes
		{
			ACTION action;
			Attributes attributes;
		};
		std::vector<ActionAttributes> m_actions;

		struct ReasonerAttributes
		{
			Reasoner *reasoner;
			Attributes attributes;
		};
		std::vector<ReasonerAttributes> m_reasoners;
	};
}
