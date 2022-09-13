#pragma once
#include "Unigine.h"

#include "gob_engine30/reasoner.h"

using namespace Unigine;

class NPC
	: public Unigine::ComponentBase 
{
public:
	COMPONENT_DEFINE(NPC, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// inspector
	PROP_ARRAY(String, m_attributes, "Attributes")
	PROP_PARAM(Int, m_decision_strat, 0, "Action Choose Strategy", "0: MP\n1: BD\n2: BDT\n3: HMP\n4: HBD\n5: HBDT\n6: hmptv\n7: hbdte", "", "my_type=2;max=7;min=0")
	PROP_PARAM(Int, m_evaluate_strat, 0, "Evaluate Strategy", "0: MINIMAL_SCORE\n1: MINIMAL_SCORE_TIME\n", "", "my_typee=2;max=1;min=0")
	PROP_PARAM(Int, m_redeem_strat, 0, "Redeem Strategy", "0: DELAYED\n1: INSTANT\n", "", "my_typeee=2;max=1;min=0")
	PROP_PARAM(Float, m_day_duration, 24 ,"Day Duration in Minutes")
	PROP_PARAM(Float, m_current_time, 0 ,"Current Time in Minutes")

	// goals
	struct Goal : public Unigine::ComponentStruct 
	{
		PROP_PARAM(String, name, "NULL")
		PROP_PARAM(Float, threshold, 0.0f)
		PROP_PARAM(Float, value, 0.0f)
		PROP_PARAM(Int, rank, 1,"Rank", "", "", "my_type=2;max=255;min=1")

		// growths
		struct PermanentGrowth : public Unigine::ComponentStruct 
		{
			PROP_PARAM(Float, start, 0.0f, "Start in Minutes", "0: 6 am, 12: 18 pm", "", "my_type=2;max=24,min=0");
			PROP_PARAM(Float, end, 24.0f, "End in Minutes", "0: 6 am, 12: 18 pm", "", "my_type=2;max=24,min=0");
			PROP_PARAM(Float, change, 0.0f, "Change", "Change during one Interval")
		};
		PROP_ARRAY_STRUCT(PermanentGrowth, m_permanent_growths, "Permanent Growths")

		struct TemporaryGrowth: public Unigine::ComponentStruct
		{
			PROP_PARAM(Float, change, 0.0f, "Change")
			PROP_PARAM(Float, duration, 0.0f, "Duration in seconds")
		};
		PROP_ARRAY_STRUCT(TemporaryGrowth, m_temporary_growths, "Temporary Growths")

		struct RandomGrowth : public Unigine::ComponentStruct 
		{
			PROP_PARAM(Float, start, 0.0f, "Start in Minutes", "0: 6 am, 12: 18 pm", "", "my_type=2;max=24,min=0");
			PROP_PARAM(Float, end, 24.0f, "End in Minutes", "0: 6 am, 12: 18 pm", "", "my_type=2;max=24,min=0");
			PROP_PARAM(Float, change_start, 0.0f, "Random Interval Start")
			PROP_PARAM(Float, change_end, 1.0f, "Random Interval End")
		};
		PROP_ARRAY_STRUCT(RandomGrowth, m_random_growths, "Random Growths")

	};
	PROP_ARRAY_STRUCT(Goal, m_goals, "Goals")

	// members
	ObjectTextPtr m_text;
	void set_text(const std::string& activity);
	GOB::Reasoner m_reasoner;

private:
	void init();
	void update();
	void shutdown();

	bool m_active_geometry = false;
};

