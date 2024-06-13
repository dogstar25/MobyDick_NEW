#include "Action.h"

Action::~Action()
{

}

// Serialization and Deserialization
namespace Json {
	template<>
	void serialize<Action>(Json::Value& value, Action& o) {
		value["label"] = o.m_label;
	}

	template<>
	void deserialize<Action>(Json::Value& value, Action& o) {
		o.setLabel(value["label"].asString());
	}
}
