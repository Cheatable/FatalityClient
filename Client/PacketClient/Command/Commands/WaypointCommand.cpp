#include "WaypointCommand.h"

WaypointCommand::WaypointCommand() : IMCCommand("waypoint", "Manage Waypoints", "<add|remove|teleport|removeall> <name> [x y z]") {
	registerAlias("wp");
}

bool WaypointCommand::execute(std::vector<std::string>* args) {
	C_LocalPlayer* player = g_Data.getLocalPlayer();
	assertTrue(player != nullptr);
	assertTrue(args->size() >= 2);

	static auto mod = moduleMgr->getModule<Waypoints>();

	std::string opt = args->at(1);

	if (opt == "removeall") {
		auto num = mod->getWaypoints()->size();
		mod->getWaypoints()->clear();
		clientMessageF("[F] %sRemoved %i waypoints!", RED, num);
		return true;
	}

	assertTrue(args->size() > 2);

	std::string name = args->at(2);
	name = Utils::sanitize(name);
	if (name.size() <= 1 || name.size() > 30) {
		clientMessageF("[F] %sInvalid name! Must be less than 30 characters!", RED);
		return true;
	}

	if (opt == "add") {
		vec3_t pos = player->currentPos.floor().add(0.5, 0, 0.5);
		if (args->size() == 6) {
			pos.x = assertFloat(args->at(3));
			pos.y = assertFloat(args->at(4));
			assertTrue(pos.y >= 0);
			pos.z = assertFloat(args->at(5));
		} else if (args->size() != 3) {
			return false;
		}
		int dimension;
		player->getDimensionId(&dimension);

		if (mod->add(name, pos, dimension)) {
			clientMessageF("[F] %sSuccessfully added waypoint \"%s\"", RED, name.c_str());
			if (!mod->isEnabled())
				clientMessageF("%sEnable the waypoints module to see it ingame!", YELLOW);
		} else {
			clientMessageF("[F] %sWaypoint \"%s\" already exists", RED, name.c_str());
		}
	} else if (opt == "remove") {
		if (mod->remove(name)) {
			clientMessageF("[F] %sRemoved waypoint \"%s\"", RED, name.c_str());
		} else {
			clientMessageF("[F] %sUnknown waypoint \"%s\"", RED, name.c_str());
		}
	}else if (opt == "tp" || opt == "teleport") {
		if (auto wp = mod->getWaypoint(name)) {
			auto wpV = wp.value();
			auto pos = wpV.pos;
			player->setPos(pos);
			clientMessageF("[F] %sTeleported to waypoint \"%s\" (%.02f, %.02f, %.02f)", RED, name.c_str(), pos.x, pos.y, pos.z);
		} else {
			clientMessageF("[F] %sUnknown waypoint \"%s\"", RED, name.c_str());
		}
	} else {
		return false;
	}

	return true;
}
