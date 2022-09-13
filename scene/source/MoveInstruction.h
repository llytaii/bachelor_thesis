#pragma once

#include "UnigineInstruction.h"

#include <queue>

using namespace Unigine;
using namespace Math;

// moves npc using pathfinding or just plain kinematic arrive, if no path found

class MoveInstruction
	: public UnigineInstruction
{
public:
	virtual ~MoveInstruction() = default;
	MoveInstruction(uint8_t sequence_position, NodePtr destination = nullptr, float arrive_radius = 0.1f, bool interruptable = true)
	{
		m_destination_node = destination;
		m_arrive_radius = arrive_radius;
		m_interruptable = interruptable;
		m_sequence_position = sequence_position;

		m_route = PathRoute::create();
		m_route->setRadius(1.0f);
		m_route->setMaxAngle(1.0f);
	};

	void set_text(const std::string& text)
	{
		m_text = text;
	}

	void set_speed(float speed)
	{
		m_movement_speed = speed;
	}

	virtual float get_duration() const override
	{
		auto distance = m_destination_node->getWorldPosition() - m_node->getWorldPosition();
		distance.z = 0.0f;
		return (distance.lengthFast() - m_arrive_radius) / m_movement_speed;
	}

	virtual GOB::ExecuteSignal execute() override {
		if (m_destination_node == nullptr) return GOB::ExecuteSignal::DONE;
		auto destination = m_destination_node->getWorldPosition();

		if (arrived_at(destination)) return GOB::ExecuteSignal::DONE;

		// set npc text
		if(m_text.size() != 0)
			m_component->set_text(m_text);
		else
			m_component->set_text(std::string{ "geht zu " } + m_destination_node->getName());

		return GOB::ExecuteSignal(approach(destination));
	};

	virtual bool is_ignorable() override
	{
		return m_interruptable;
	}

	virtual std::unique_ptr<GOB::Instruction> clone() const override {
		return std::make_unique<MoveInstruction>(*this);
	};

	bool approach(Vec3 destination)
	{
		Vec3 npc_pos = m_node->getWorldPosition();
		Vec3 dest = destination;
		m_route->create2D(npc_pos, dest);

		if (m_route->isReady() && m_route->isReached())
		{
			m_route->renderVisualizer(vec4(1.0f));

			for (int i = 0; i < m_route->getNumPoints(); ++i)
			{
				Vec3 vec = m_route->getPoint(i);
			}

			Vec3 next_waypoint = m_route->getPoint(1) - m_node->getWorldPosition();
			next_waypoint.z = 0.0f;
			m_direction = next_waypoint.normalizeFast();

			update_orientation(m_direction);

			Vec3 delta_movement = m_direction * m_movement_speed * Game::getIFps();
			m_node->setWorldPosition(m_node->getWorldPosition() + delta_movement);
		}
		else
		{
			m_direction = dest - m_node->getWorldPosition();
			m_direction.normalizeFast();

			update_orientation(m_direction);

			Vec3 delta_movement = m_direction * m_movement_speed * Game::getIFps();
			m_node->setWorldPosition(m_node->getWorldPosition() + delta_movement);
		}

		return arrived_at(destination);
	}
	bool arrived_at(Vec3 destination)
	{
		Vec3 dest = destination;
		dest.z = 0.0f;
		Vec3 npc_position = m_node->getWorldPosition();
		npc_position.z = 0.0f;
		return (dest - npc_position).lengthFast() < m_arrive_radius;
	}

	void update_orientation(Vec3 direction)
	{
		quat target_rotation{ Math::setTo(vec3_zero, direction, vec3_up, MathLib::AXIS::AXIS_Y) };

		quat current_rotation = m_node->getWorldRotation();
		current_rotation = Math::slerp(current_rotation, target_rotation, Game::getIFps() * m_rotation_stiffness / 2);
		m_node->setWorldRotation(current_rotation);
		
		current_rotation = m_node->getChild(0)->getChild(0)->getWorldRotation();
		current_rotation = Math::slerp(current_rotation, target_rotation, Game::getIFps() * m_rotation_stiffness);
		m_node->getChild(0)->getChild(0)->setWorldRotation(current_rotation);
	}

protected:
	Vec3 m_direction;

	std::string m_text;

	NodePtr m_destination_node;
	float m_movement_speed = 4.0f;
	bool m_interruptable = false;
	float m_arrive_radius = 0.1f;
	float m_rotation_stiffness = 4.0f;

	PathRoutePtr m_route;

};
