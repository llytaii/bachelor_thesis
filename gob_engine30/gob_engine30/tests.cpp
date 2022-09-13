#define CATCH_CONFIG_MAIN
#include <catch.hpp>

// float compare function
bool float_cmp(const float f1, const float f2)
{
	return (std::abs(f1 - f2) < 0.0000001);
}

#include "instruction.h"

using namespace GOB;

// some dummy instruction classes
class GreetingsInstruction
	: public Instruction
{
public:
	GreetingsInstruction() = default;
	GreetingsInstruction(uint8_t _position, float duration)
		: Instruction{ _position }
	{
		m_duration = duration;
	}
	virtual ~GreetingsInstruction() = default;
	GreetingsInstruction& operator=(const GreetingsInstruction& other) = default;

	virtual float get_duration() const override {
		return m_duration;
	}
	virtual void init() override {};
	virtual GOB::ExecuteSignal execute() override {
		return ExecuteSignal::DONE;
	}
	virtual void done() override {};
	virtual bool is_ignorable() const override {
		return true;
	}
	virtual INSTRUCTION clone() const override {
		return std::make_unique<GreetingsInstruction>(*this);
	}
	void* get_npc() const {
		return m_npc;
	}
	Reasoner* get_reasoner() const {
		return m_reasoner;
	}
private:
	float m_duration{ 0 };
};

TEST_CASE("INSTRUCTION")
{
	auto position = uint8_t(3);
	auto duration = float(5.5f);

	auto i = new int;
	delete i;
	REQUIRE(i != nullptr);

	auto v = static_cast<void*>(i);
	auto r = static_cast<Reasoner*>(v);

	auto greetings = GreetingsInstruction{ position, duration };
	greetings.set_npc_reasoner(v, r);

	REQUIRE(greetings.get_npc() == v);
	REQUIRE(greetings.get_reasoner() == r);


	auto ptr = greetings.clone();

	REQUIRE(float_cmp(ptr->get_duration(), duration) == true);
	REQUIRE(ptr->get_sequence_position() == position);


	greetings = GreetingsInstruction{ uint8_t(duration), float(position) };
	REQUIRE(ptr->get_sequence_position() != greetings.get_sequence_position());
}

#include "attributes.h"


TEST_CASE("ATTRIBUTES")
{
	auto npc_attributes = Attributes{};
	auto go_attributes = Attributes{};

	auto kenny = std::string{ "kenny" };
	auto hugo = std::string{ "hugo" };
	auto smiths = std::string{ "smiths" };

	npc_attributes.add(kenny);
	npc_attributes.add(hugo);

	REQUIRE(npc_attributes.contains(kenny) == true);
	REQUIRE(npc_attributes.contains(hugo) == true);

	npc_attributes.remove(hugo);

	REQUIRE(npc_attributes.contains(hugo) == false);

	go_attributes.add(kenny);
	go_attributes.add(hugo);
	go_attributes.add(smiths);

	REQUIRE(npc_attributes.contains(go_attributes) == false);
	REQUIRE(go_attributes.contains(npc_attributes) == true);

	go_attributes.remove(npc_attributes);

	REQUIRE(go_attributes.contains(kenny) == false);
	REQUIRE(go_attributes.contains(hugo) == true);
	REQUIRE(go_attributes.contains(smiths) == true);

	REQUIRE(go_attributes.contains(npc_attributes) == false);
	REQUIRE(npc_attributes.contains(go_attributes) == false);

	npc_attributes.add(go_attributes);

	REQUIRE(npc_attributes.contains(kenny) == true);
	REQUIRE(npc_attributes.contains(hugo) == true);
	REQUIRE(npc_attributes.contains(smiths) == true);

	REQUIRE(npc_attributes.contains(go_attributes) == true);
	REQUIRE(go_attributes.contains(npc_attributes) == false);
}

#include "action.h"

TEST_CASE("ACTION")
{
	SECTION("default constructor")
	{
		auto tmp = Action{};

		REQUIRE(tmp.get_name() == std::string{ "" });
		REQUIRE(tmp.get_full_cooldown() == 0.0f);
		REQUIRE(tmp.get_cooldown() == 0.0f);
		REQUIRE(tmp.get_score() == 0.0f);
	}

	SECTION("overloaded constructor")
	{
		auto name = std::string{ "test" };
		auto tmp = Action{ name };

		REQUIRE(tmp.get_name() == name);
	}

	SECTION("copy constructor")
	{
		auto ptr = new int;
		delete ptr;
		Reasoner* r_ptr = static_cast<Reasoner*>(static_cast<void*>(ptr));
		Action original;
		original.set_full_cooldown(10.0f);
		original.set_on_cooldown();
		original.set_name("hi");
		original.set_npc_reasoner(ptr, r_ptr);
		original.set_goal_change("eat", 0.4f);
		original.add_instruction(GreetingsInstruction{ 0, 1.5f }.clone());

		REQUIRE(original.get_name() == "hi");
		REQUIRE(original.get_full_cooldown() == 10.0f);
		REQUIRE(original.get_cooldown() == 10.0f);
		REQUIRE(original.get_npc() == ptr);
		REQUIRE(original.get_reasoner() == r_ptr);
		REQUIRE(float_cmp(original.get_change("eat"), 0.4f));

		auto copy = Action{ original };

		REQUIRE(copy.get_cooldown() == original.get_cooldown());
		REQUIRE(copy.get_full_cooldown() == original.get_full_cooldown());
		REQUIRE(copy.get_name() == original.get_name());
		REQUIRE(copy.get_npc() == original.get_npc());
		REQUIRE(copy.get_reasoner() == original.get_reasoner());
		REQUIRE(copy.get_instructions().size() == original.get_instructions().size());
	}

	SECTION("move constructor")
	{
		auto ptr = new int;
		delete ptr;
		Reasoner* r_ptr = static_cast<Reasoner*>(static_cast<void*>(ptr));
		Action original;
		original.set_full_cooldown(10.0f);
		original.set_on_cooldown();
		original.set_name("hi");
		original.set_npc_reasoner(ptr, r_ptr);
		original.set_goal_change("eat", 0.4f);
		original.add_instruction(GreetingsInstruction{ 0, 1.5f }.clone());

		REQUIRE(original.get_name() == "hi");
		REQUIRE(original.get_full_cooldown() == 10.0f);
		REQUIRE(original.get_cooldown() == 10.0f);
		REQUIRE(original.get_npc() == ptr);
		REQUIRE(original.get_reasoner() == r_ptr);
		REQUIRE(float_cmp(original.get_change("eat"), 0.4f));

		auto cooldown = original.get_cooldown();
		auto full_cooldown = original.get_full_cooldown();
		auto name = original.get_name();
		auto npc = original.get_npc();
		auto reasoner = original.get_reasoner();
		auto instr = original.get_instructions().size();


		auto copy = Action{ std::move(original) };

		REQUIRE(copy.get_cooldown() == cooldown);
		REQUIRE(copy.get_full_cooldown() == full_cooldown);
		REQUIRE(copy.get_name() == name);
		REQUIRE(copy.get_npc() == npc);
		REQUIRE(copy.get_reasoner() == reasoner);
		REQUIRE(copy.get_instructions().size() == instr);
	}

	SECTION("effects")
	{
		auto eat = std::string{ "eat" };
		auto sleep = std::string{ "sleep" };
		auto drink = std::string{ "drink" };

		auto tmp = Action{};
		tmp.set_goal_change(eat, 0.1f);
		tmp.set_goal_change(sleep, -0.95f, 10.0f);
		tmp.set_goal_change(drink, 0.999f);



		REQUIRE(float_cmp(tmp.get_change(eat), 0.1f));
		REQUIRE(float_cmp(tmp.get_change(sleep), -0.95f));
		REQUIRE(float_cmp(tmp.get_change(drink), 0.999f));
		REQUIRE(float_cmp(tmp.get_change("none"), 0.0f));


		REQUIRE(float_cmp(tmp.get_goal_change_duration(sleep).duration, 10.0f));
		REQUIRE(float_cmp(tmp.get_goal_change_duration(eat).duration, 0.0f));

		REQUIRE(tmp.get_affected_goals().size() == 3);
	}

	SECTION("instructions")
	{
		auto tmp = Action{};

		auto greetings2 = GreetingsInstruction{ 2, 1.5f };
		auto greetings0 = GreetingsInstruction{ 0, 1.5f };
		auto greetings1 = GreetingsInstruction{ 1, 1.5f };

		tmp.add_instruction(greetings1.clone());
		tmp.add_instruction(greetings2.clone());
		tmp.add_instruction(greetings0.clone());

		for (int j = 0; j < tmp.get_instructions().size(); ++j)
			REQUIRE(tmp.get_instructions()[j]->get_sequence_position() == j);

		REQUIRE(tmp.get_full_duration() == tmp.get_duration());
		REQUIRE(tmp.advance_instruction() == true);
		REQUIRE(tmp.get_duration() == tmp.get_full_duration() - 1.5f);
		REQUIRE(tmp.advance_instruction() == true);
		REQUIRE(tmp.get_duration() == tmp.get_full_duration() - 3.0f);
		REQUIRE(tmp.advance_instruction() == false);
	}
}

#include "growth.h"

TEST_CASE("GROWTH")
{
	SECTION("TEMPORARY_GROWTH")
	{
		auto CHANGE = float(10.0f);
		auto duration = float(10.0f);
		auto tmp_growth = TemporaryGrowth{ CHANGE, duration };

		REQUIRE(tmp_growth.get_change(duration / 2) == CHANGE / 2);
		tmp_growth.decrease_duration(duration / 2);
		REQUIRE(tmp_growth.is_done() == false);
		tmp_growth.decrease_duration(duration * 2);
		REQUIRE(tmp_growth.is_done() == true);
	}

	SECTION("RANDOM_GROWTH")
	{
		float start = 0.0f;
		float end = 1.0f;

		float change_start = 0.0f;
		float change_end = 1.0f;

		auto rand_growth = RandomGrowth{ change_start, change_end, start, end };

		float last = rand_growth.get_random(change_start, change_end);
		for (int i = 0; i < 100; ++i)
		{
			float current = rand_growth.get_random(change_start, change_end);
			REQUIRE(current != last);
			REQUIRE(current >= 0.0f);
			REQUIRE(current < 1.0f);
			float last = current;
		}
	}

	SECTION("PERMANENT_GROWTH")
	{
		float change = 10.0f;
		float start = 2.0f;
		float end = 3.0f;

		auto perm_growth = PermanentGrowth{ change, start, end };

		REQUIRE(perm_growth.get_change((end - start) / 2, start / 2) == 0.0f);
		REQUIRE(perm_growth.get_change((end - start) / 2, start + (end - start) / 2) == change / 2);
	}
}

#include "goal.h"

TEST_CASE("GOAL")
{
	SECTION("default constructor")
	{
		auto tmp = Goal{};

		REQUIRE(tmp.get_name() == std::string{ "NULL" });
		REQUIRE(float_cmp(tmp.get_threshold(), 0.0f));
		REQUIRE(float_cmp(tmp.get_value(), 0.0f));
		REQUIRE(tmp.get_rank() == 1);
	}

	SECTION("overloaded constructor")
	{
		auto tmp = Goal{ "test" };

		REQUIRE(tmp.get_name() == std::string{ "test" });
		REQUIRE(float_cmp(tmp.get_threshold(), 0.0f));
		REQUIRE(float_cmp(tmp.get_value(), 0.0f));
		REQUIRE(tmp.get_rank() == 1);
	}

	SECTION("setters")
	{

		Goal tmp;

		REQUIRE(float_cmp(tmp.get_threshold(), 0.0f));
		REQUIRE(float_cmp(tmp.get_value(), 0.0f));
		REQUIRE(tmp.get_rank() == 1);

		tmp.set_threshold(0.5f);
		tmp.set_value(0.5f);
		tmp.set_rank(3);

		REQUIRE(float_cmp(tmp.get_threshold(), 0.5f));
		REQUIRE(float_cmp(tmp.get_value(), 0.5f));
		REQUIRE(tmp.get_rank() == 3);

		tmp.change_value(0.4f);
		REQUIRE(float_cmp(tmp.get_value(), 0.9f));

		REQUIRE(float_cmp(tmp.get_discontentment(0.8f), 0.8f * 0.8f));

		SECTION("clamping setters")
		{
			tmp.set_threshold(2.0f);
			tmp.set_value(2.0f);

			REQUIRE(float_cmp(tmp.get_threshold(), 1.0f));
			REQUIRE(float_cmp(tmp.get_value(), 1.0f));

			tmp.change_value(1.2f);
			REQUIRE(float_cmp(tmp.get_value(), 1.0f));

			tmp.set_threshold(-2.0f);
			tmp.set_value(-2.0f);

			REQUIRE(float_cmp(tmp.get_threshold(), 0.0f));
			REQUIRE(float_cmp(tmp.get_value(), 0.0f));

			tmp.change_value(-1.2f);
			REQUIRE(float_cmp(tmp.get_value(), 0.0f));
		}

		SECTION("GROWTH")
		{
			auto tmp_growth = TemporaryGrowth{ 0.1f, 1.0f };
			auto perm_growth = PermanentGrowth{ 0.1f, 0.0f, 1.0f };
			auto goal = Goal{};

			goal.add_growth(tmp_growth);
			goal.add_growth(perm_growth);
			REQUIRE(goal.get_value() == 0.0f);

			goal.update(0.5f, 2.0f); // only tmp_growth active
			REQUIRE(goal.get_value() == 0.05f);

			goal.update(1.0f, 1.0f); // both active
			REQUIRE(goal.get_value() == 0.2f);

			goal.cleanup_temporary_growths();
			goal.update(0.5f, 2.0f); // only tmp_growth active
			REQUIRE(goal.get_value() == 0.2f);

			// get change for future:
			goal.set_value(0.0f);
			goal.add_growth(tmp_growth);

			REQUIRE(goal.get_change(2.0f, 0.0f, 1.0f) == 0.3f);
			REQUIRE(goal.get_change(1.0f, 0.5f, 2.0f) == 0.15f);
			REQUIRE(goal.get_change(2.0f, 0.0f, 2.0f) == 0.2f);
		}
	}
}

#include "logger.h"

TEST_CASE("LOGGER")
{
	Goal eat_g{ "eat" };
	eat_g.set_rank(4);
	eat_g.set_threshold(0.4f);
	eat_g.set_value(0.4f);

	Goal sleep_g{ "eat" };
	sleep_g.set_rank(3);
	sleep_g.set_threshold(0.3f);
	sleep_g.set_value(0.3f);

	Goal support_g{ "eat" };
	support_g.set_rank(3);
	support_g.set_threshold(0.3f);
	support_g.set_value(0.3f);

	Action eat_a{ "eat" };
	eat_a.set_score(0.444f);

	Action sleep_a{ "eat" };
	sleep_a.set_score(0.333f);

	Action support_a{ "eat" };
	support_a.set_score(0.555f);

	GOALS goals;
	goals.push_back(std::make_shared<Goal>(eat_g));
	goals.push_back(std::make_shared<Goal>(sleep_g));


	ACTIONS actions;
	actions.push_back(std::make_shared<Action>(eat_a));
	actions.push_back(std::make_shared<Action>(sleep_a));

	ACTION current = std::make_shared<Action>(eat_a);
	ACTION candidate = std::make_shared<Action>(sleep_a);

	// defaults test

	float dummy_timestamp = 0.5f;

	Logger logger;
	logger.log(dummy_timestamp, goals, actions, current);
	logger.log(dummy_timestamp, goals, actions, current, "test source");
	logger.log(dummy_timestamp, goals, actions, current);
	logger.log(dummy_timestamp, goals, actions, current, "test source");
	logger.log(dummy_timestamp, goals, actions, current);

	// mutate vectors
	goals.erase(++goals.begin());
	actions.erase(actions.begin());

	logger.log(dummy_timestamp, goals, actions, current, "test source");
	logger.log(dummy_timestamp, goals, actions, current);
	logger.log(dummy_timestamp, goals, actions, current, "test source");

	goals.push_back(std::make_shared<Goal>(support_g));
	actions.push_back(std::make_shared<Action>(support_a));

	logger.log(dummy_timestamp, goals, actions, current);
	logger.log(dummy_timestamp, goals, actions, current, "test source");
	logger.log(dummy_timestamp, goals, actions, current);

	goals.push_back(std::make_shared<Goal>(sleep_g));
	actions.push_back(std::make_shared<Action>(eat_a));

	logger.log(dummy_timestamp, goals, actions, current);
	logger.log(dummy_timestamp, goals, actions, current, "test source");
	logger.log(dummy_timestamp, goals, actions, current);

	//logger.save_to_file("tests");
}

#include "reasoner.h"

TEST_CASE("REASONER")
{
	// INIT

	Reasoner r1, r2;

	auto i = new int;
	delete i;

	r1.set_day_duration(10.0f);
	r1.set_npc(i);
	r1.set_decision_strategy(DECISION::HIERARCHICAL_BEST_DISSATISFACION);
	r1.set_evaluate_strategy(EVALUATE::MINIMAL_SCORE_TIME);
	r1.set_redeem_strategy(REDEEM::DELAYED);

	REQUIRE(r1.get_day_duration() == 10.0f);
	REQUIRE(r2.get_day_duration() == 60.0f * 24.0f);

	REQUIRE(r1.get_npc() == i);
	REQUIRE(r2.get_npc() == nullptr);

	REQUIRE(r1.get_decision_strategy() == DECISION::HIERARCHICAL_BEST_DISSATISFACION);
	REQUIRE(r2.get_decision_strategy() == DECISION::BEST_DISSATISFACTION);

	REQUIRE(r1.get_evaluate_strategy() == EVALUATE::MINIMAL_SCORE_TIME);
	REQUIRE(r2.get_evaluate_strategy() == EVALUATE::MINIMAL_SCORE);

	REQUIRE(r1.get_redeem_strategy() == REDEEM::DELAYED);
	REQUIRE(r2.get_redeem_strategy() == REDEEM::INSTANT);

#ifdef GOB_ENABLE_LOGGING
	REQUIRE(r1.get_id() == 0);
	REQUIRE(r2.get_id() == 1);
#endif // GOB_ENABLE_LOGGING

	r1.update_state(5.0f);

	REQUIRE(r1.get_current_time() == 5.0f);
	REQUIRE(r2.get_current_time() == 0.0f);

	r1.update_state(10.0f);
	REQUIRE(r1.get_current_time() == 5.0f);


	// add goals

	Goal eat("eat");
	Goal sleep("sleep");
	Goal drink("drink");

	GOAL eat_ptr;
	std::shared_ptr<Goal> sleep_ptr;
	std::shared_ptr<Goal> drink_ptr;
	std::shared_ptr<Goal> null_ptr;
	REQUIRE(null_ptr == nullptr);

	// add_goal
	eat_ptr = r1.add_goal(eat);
	null_ptr = r1.add_goal(eat);
	sleep_ptr = r1.add_goal(sleep);
	drink_ptr = r1.add_goal(drink);

	REQUIRE(eat_ptr->get_name() == eat.get_name());
	REQUIRE(sleep_ptr->get_name() == sleep.get_name());
	REQUIRE(drink_ptr->get_name() == drink.get_name());
	REQUIRE(null_ptr == nullptr);

	// get_goal
	eat_ptr = r1.get_goal(eat.get_name());
	sleep_ptr = r1.get_goal(sleep.get_name());
	drink_ptr = r1.get_goal(drink.get_name());
	null_ptr = r1.get_goal("null");

	REQUIRE(eat_ptr->get_name() == eat.get_name());
	REQUIRE(sleep_ptr->get_name() == sleep.get_name());
	REQUIRE(drink_ptr->get_name() == drink.get_name());
	REQUIRE(null_ptr == nullptr);

	// remove_goal
	r1.remove_goal(eat.get_name());
	null_ptr = r1.get_goal(eat.get_name());
	REQUIRE(null_ptr == nullptr);

	// add actions
	REQUIRE(r1.get_actions().size() == 0);

	Action test{ "test" };
	test.set_full_cooldown(10.0f);

	Action test2{ "test2" };
	test2.set_full_cooldown(10.0f);

	r1.add_action(test);
	REQUIRE(r1.get_actions().size() == 1);

	r1.add_action(test2);
	REQUIRE(r1.get_actions().size() == 2);

	r1.remove_action(test.get_id());
	REQUIRE(r1.get_actions().size() == 1);

	r1.remove_action(test2.get_id());
	REQUIRE(r1.get_actions().size() == 0);

	auto a_test = r1.add_action(test);
	auto a_test2 = r1.add_action(test2);


	// UPDATE
	// update goals
	auto tmp_growth = TemporaryGrowth{ 0.5f, 0.8f };
	auto perm_growth = PermanentGrowth{ 0.5f, 0.0f, 0.8f };
	sleep_ptr->add_growth(perm_growth);

	REQUIRE(sleep_ptr->get_value() == 0.0f);

	r1.set_day_duration(0.8f);
	r1.set_current_time(0.4f);

	REQUIRE(r1.get_day_duration() == 0.8f);
	REQUIRE(float_cmp(r1.get_current_time(), 0.4f));

	r1.update_state(0.8f);

	REQUIRE(float_cmp(r1.get_current_time(), 0.4f));
	REQUIRE(sleep_ptr->get_value() == 0.5f);

	sleep_ptr->add_growth(tmp_growth);
	sleep_ptr->set_value(0.0f);

	REQUIRE(float_cmp(r1.get_current_time(), 0.4f));
	REQUIRE(sleep_ptr->get_value() == 0.0f);

	r1.update_state(0.8f);
	REQUIRE(sleep_ptr->get_value() == 1.0f);

	// update actions

	a_test->set_on_cooldown();
	REQUIRE(a_test->get_cooldown() == 10.0f);
	REQUIRE(a_test2->get_cooldown() == 0.0f);

	r1.update_actions(5.0f);

	REQUIRE(a_test->get_cooldown() == 5.0f);
	REQUIRE(a_test2->get_cooldown() == 0.0f);

	// choose action

	eat_ptr = r1.add_goal(eat);

	REQUIRE(r1.get_goals().size() == 3);

	eat_ptr->set_value(0.0f);
	sleep_ptr->set_value(0.5f);
	drink_ptr->set_value(0.7f);

	auto greetings = GreetingsInstruction{ 0, 10 };
	Action eat_a{ "eat" };
	eat_a.add_instruction(greetings.clone());
	eat_a.set_goal_change(eat_ptr->get_name(), -1.0f);

	Action sleep_a{ "sleep" };
	sleep_a.add_instruction(greetings.clone());
	sleep_a.set_goal_change(sleep_ptr->get_name(), -1.0f);

	Action drink_a{ "drink" };
	drink_a.add_instruction(greetings.clone());
	drink_a.set_goal_change(drink_ptr->get_name(), -1.0f);

	auto a_eat_ptr = r1.add_action(eat_a);
	auto a_sleep_ptr = r1.add_action(sleep_a);
	auto a_drink_ptr = r1.add_action(drink_a);

	// most pressing
	r1.set_decision_strategy(DECISION::MOST_PRESSING);
	REQUIRE(r1.get_decision_strategy() == DECISION::MOST_PRESSING);
	r1.decide();

	REQUIRE(r1.get_current_action() == a_drink_ptr);

	// best dissatisfaction
	r1.set_current_action(nullptr);
	r1.set_decision_strategy(DECISION::BEST_DISSATISFACTION);
	r1.decide();

	REQUIRE(r1.get_current_action() == a_drink_ptr);

	// TODO: STRATEGIEN?

	SECTION("EVALUATION")
	{
		Reasoner evaluater;
		evaluater.set_decision_strategy(DECISION::BEST_DISSATISFACTION);

		// übergrundbedürfnisse
		auto survive_g = evaluater.add_goal({ "survive" });
		auto breathe_g = evaluater.add_goal({ "breathe" });

		survive_g->set_rank(4);
		survive_g->set_value(0.0f);

		breathe_g->set_rank(4);
		breathe_g->set_value(0.0f);

		auto survive_a = evaluater.add_action(Action{ "survive" });
		auto breathe_a = evaluater.add_action(Action{ "breathe" });

		survive_a->set_goal_change(survive_g->get_name(), -1.0f);
		breathe_a->set_goal_change(breathe_g->get_name(), -1.0f);

		// grundbedürfnisse
		auto eat_g = evaluater.add_goal({ "eat" });
		auto sleep_g = evaluater.add_goal({ "sleep" });
		auto drink_g = evaluater.add_goal({ "drink" });

		eat_g->set_rank(3);
		eat_g->set_value(0.2f);

		sleep_g->set_rank(3);
		sleep_g->set_value(0.2f);

		drink_g->set_rank(3);
		drink_g->set_value(0.2f);
	}
}

#include "broadcaster.h"

TEST_CASE("BROADCASTER")
{
	auto& broadcaster = Broadcaster::get_instance();
	Reasoner r1;
	auto attributes_r1 = Attributes{ "hugo", "smith" };
	REQUIRE(attributes_r1.contains({ "hugo", "smith" }) == true);

	Reasoner r2;
	auto attributes_r2 = Attributes{ "clemens", "smith" };
	REQUIRE(attributes_r2.contains({ "clemens", "smith" }) == true);

	Reasoner r3;

	auto attributes_hugo = Attributes{ "hugo" };
	auto attributes_clemens = Attributes{ "clemens" };
	auto attributes_smith = Attributes{ "smith" };

	broadcaster.add_reasoner(&r1, attributes_r1);
	broadcaster.add_reasoner(&r3);

	broadcaster.add_action({ "hugo" }, attributes_hugo);
	broadcaster.add_action({ "clemens" }, attributes_clemens);
	auto smith = broadcaster.add_action({ "smith" }, attributes_smith);

	broadcaster.add_reasoner(&r2, attributes_r2);

	REQUIRE(r1.get_actions().size() == 2);
	REQUIRE(r2.get_actions().size() == 2);
	REQUIRE(r3.get_actions().size() == 3);


	broadcaster.remove_action(smith);

	REQUIRE(r1.get_actions().size() == 1);
	REQUIRE(r2.get_actions().size() == 1);
	REQUIRE(r3.get_actions().size() == 2);
}


