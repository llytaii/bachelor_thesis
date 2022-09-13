#pragma once
#include "Unigine.h"

using namespace Unigine;

class ActionBroadcaster
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ActionBroadcaster, ComponentBase)
	COMPONENT_INIT(init)

	// default attributes if none are set for an action 
	PROP_ARRAY(String, m_default_attributes, "Default Attributes")

	// actions
	struct Action : public ComponentStruct
	{
		// general
		PROP_PARAM(String, name, "Name")
		PROP_PARAM(Int, display_text, 1, "Display Text", "0 = no text", "", "my_type=2;max=1; min=0")

		// affected goals
		struct GoalChange : public ComponentStruct
		{
			PROP_PARAM(String, goal_name, "Goal Name")
			PROP_PARAM(Float, change, 0.0f, "Change", "Change for Goal", "", "my_type=2;max=1;min=-1");
			PROP_PARAM(Float, duration, 0.0f, "Duration", "Duration over which the Goal should be changed");
		};
		PROP_ARRAY_STRUCT(GoalChange, affected_goals, "Affected Goals")

		// instructions
		struct PropMoveInstruction : public ComponentStruct
		{
			PROP_PARAM(Node, destination, "Destination")
			PROP_PARAM(Float, arrive_radius, 0.1f, "Arrive Radius");
			PROP_PARAM(Int, interruptable, 1, "Interruptable", "0 = not interruptable", "", "my_type=2;max=1; min=0")
			PROP_PARAM(Int, sequence_position, 0,"Order Position", "", "", "my_type=2;max=255;min=0")
			PROP_PARAM(String, text)
			PROP_PARAM(Float, speed)
		};
		PROP_ARRAY_STRUCT(PropMoveInstruction, move_instructions, "Move Instructions")

		struct PropGhostInstruction : public ComponentStruct
		{
			PROP_PARAM(Node, destination, "Destination")
			PROP_ARRAY(Node, scared_person, "ScaredPersons")
			PROP_PARAM(Float, arrive_radius, 0.1f, "Arrive Radius");
			PROP_PARAM(Int, interruptable, 1, "Interruptable", "0 = not interruptable", "", "my_type=2;max=1; min=0")
			PROP_PARAM(Int, sequence_position, 0,"Order Position", "", "", "my_type=2;max=255;min=0")
			PROP_PARAM(String, text)
			PROP_PARAM(Float, speed)
		};
		PROP_ARRAY_STRUCT(PropGhostInstruction, ghost_instructions, "Ghost Instructions")

		struct PropDoInstruction : public ComponentStruct
		{
			PROP_PARAM(Node, destination, "Destination")
			PROP_PARAM(Int, sequence_position, 0,"Order Position", "", "", "my_type=2;max=255;min=0")
			PROP_PARAM(Float, duration, 10.0f, "Duration in Minutes", "Action Duration", "", "my_type=2;max=24;min=0");
			PROP_PARAM(Float, interrupt_progress, 1.0f, "Interrupt Progress in % left\ne.g. 0.7 = 70% of instruction has to be done", "", "", "my_type=2;max=1;min=0");
			PROP_PARAM(String, task, "Task")
			PROP_PARAM(String, text)
			PROP_PARAM(Float, speed, 4.0f)
		};
		PROP_ARRAY_STRUCT(PropDoInstruction, do_instructions, "Do Instructions")

		struct PropTalkInstruction: public ComponentStruct
		{
			PROP_PARAM(Node, complice, "Complice")
			PROP_PARAM(Float, duration, 10.0f, "Duration in Minutes", "Action Duration", "", "my_type=2;max=24;min=0");
			PROP_PARAM(Float, interrupt_progress, 0.3f, "Interrupt Progress in % left\ne.g. 0.7 = 70% of instruction has to be done", "", "", "my_type=2;max=1;min=0");
		};
		PROP_ARRAY_STRUCT(PropTalkInstruction, talk_instructions, "Talk Instructions")

		PROP_ARRAY(String, attributes, "Attributes")
	};
	PROP_ARRAY_STRUCT(Action, prop_actions)


private:
	void init();
};

