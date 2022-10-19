#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

namespace GOB
{
	class Goal;
	class Action;
	class Instruction;

	using GOAL = std::shared_ptr<Goal>;
	using ACTION = std::shared_ptr<Action>;

	using GOALS = std::vector<std::shared_ptr<Goal>>;
	using ACTIONS = std::vector<std::shared_ptr<Action>>;

	using INSTRUCTION = std::unique_ptr<Instruction>;
	using INSTRUCTIONS = std::vector<std::unique_ptr<Instruction>>;

	struct CHANGE
	{
		float change = 0.0f;
		float duration = 0.0f;
	};

	using GOAL_CHANGE_MAP = std::unordered_map<std::string, CHANGE>;
}
