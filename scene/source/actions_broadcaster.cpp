#include "actions_broadcaster.h"

#include "gob_engine30/attributes.h"
#include "gob_engine30/broadcaster.h"

#include "MoveInstruction.h"
#include "DoInstruction.h"
#include "TalkInstruction.h"
#include "GhostInstruction.h"

REGISTER_COMPONENT(ActionBroadcaster)

void ActionBroadcaster::init()
{
	// game object attributes
	GOB::Attributes default_attributes;
	for (int i = 0; i < m_default_attributes.size(); ++i)
	{
		auto attribute = std::string{ m_default_attributes[i] };
		default_attributes.add(attribute);
	}

	// add actions
	for (int i = 0; i < prop_actions.size(); ++i)
	{
		auto& prop_action = prop_actions[i];
		auto name = std::string{ prop_action->name };
		auto action = GOB::Action{ name };


		for (int j = 0; j < prop_action->affected_goals.size(); ++j)
		{
			auto& affected_goal = prop_action->affected_goals[j];

			auto goal_name = std::string{ affected_goal->goal_name };
			auto change = float(affected_goal->change);
			float duration = affected_goal->duration;
			action.set_goal_change(goal_name, change, duration);
		}

		for (int j = 0; j < prop_action->move_instructions.size(); ++j)
		{
			auto& prop_instruction = prop_action->move_instructions[j];

			auto sequence_position = uint8_t(prop_instruction->sequence_position);
			auto destination = NodePtr{ prop_instruction->destination };
			auto arrive_radius = float(prop_instruction->arrive_radius);
			auto interruptable = bool(prop_instruction->interruptable);
			std::string text = prop_instruction->text;
			float speed = prop_instruction->speed;

			auto instruction = MoveInstruction{sequence_position, destination, arrive_radius,interruptable};
			instruction.set_speed(speed);
			instruction.set_text(text);
			action.add_instruction(instruction.clone());
		}

		for (int j = 0; j < prop_action->ghost_instructions.size(); ++j)
		{
			auto& prop_instruction = prop_action->ghost_instructions[j];

			auto sequence_position = uint8_t(prop_instruction->sequence_position);
			auto destination = NodePtr{ prop_instruction->destination };
			auto arrive_radius = float(prop_instruction->arrive_radius);
			auto interruptable = bool(prop_instruction->interruptable);
			std::string text = prop_instruction->text;
			float speed = prop_instruction->speed;
			auto persons = prop_instruction->scared_person;

			auto instruction = GhostInstruction{sequence_position, destination, arrive_radius,interruptable};
			for(int i = 0; i < persons.size(); ++i)
				instruction.add_scared_person(persons[i]);
			action.add_instruction(instruction.clone());
		}

		for (int j = 0; j < prop_action->do_instructions.size(); ++j)
		{
			auto& prop_instruction = prop_action->do_instructions[j];
			auto destination = NodePtr{ prop_instruction->destination };
			auto sequence_position = uint8_t(prop_instruction->sequence_position);
			auto task = std::string{ prop_instruction->task };
			auto duration = float(prop_instruction->duration) * 60.0f;
			auto interrupt_progress = float(prop_instruction->interrupt_progress);
			std::string text = prop_instruction->text;
			float speed = prop_instruction->speed;

			auto instruction = DoInstruction{ task, destination, sequence_position, duration, interrupt_progress};
			instruction.set_speed(speed);
			instruction.set_text(text);
			action.add_instruction(instruction.clone());
		}

		for (int j = 0; j < prop_action->talk_instructions.size(); ++j)
		{
			auto& prop_instruction = prop_action->talk_instructions[j];
			auto complice = NodePtr{ prop_instruction->complice };
			auto duration = float(prop_instruction->duration) * 60.0f;
			auto interrupt_progress = float(prop_instruction->interrupt_progress);
			auto instruction = TalkInstruction{ complice, duration, interrupt_progress};
			action.add_instruction(instruction.clone());
		}

		auto action_attributes = GOB::Attributes{};
		for (int j = 0; j < prop_action->attributes.size(); ++j)
		{
			auto attribute = std::string{ prop_action->attributes[j] };
			action_attributes.add(attribute);
		}

		auto& attributes = (action_attributes.empty()) ? default_attributes : action_attributes;
		GOB::Broadcaster::get_instance().add_action(action, attributes);
	}
}
