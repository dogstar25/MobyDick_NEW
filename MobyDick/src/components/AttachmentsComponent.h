#ifndef ATTACHMENTS_COMPONENT_H
#define ATTACHMENTS_COMPONENT_H

#include <vector>
#include <memory>
#include <optional>

#include <json/json.h>
#include "Component.h"
//#include "../GameObject.h"

class Scene;
class GameObject;

struct Attachment
{
	std::string id{};
	std::shared_ptr<GameObject> gameObject{};
	bool addToInventory{ false };
	b2JointType attachB2JointType{ b2JointType::e_weldJoint };
	b2Vec2 attachLocation{};
	

	Attachment(std::string id, bool addToInventory, b2JointType attachB2JointType, b2Vec2 attachLocation, std::shared_ptr<GameObject> gameObject) :
		id(id),
		addToInventory(addToInventory),
		attachB2JointType(attachB2JointType),
		attachLocation(attachLocation),
		gameObject(gameObject) {}
};

class AttachmentsComponent :   public Component
{
public:
	AttachmentsComponent()=default;
	AttachmentsComponent(Json::Value componentJSON, GameObject* parent, std::string parentName, Scene* parentScene);
	~AttachmentsComponent();

	void update() override;
	void render();
	void postInit() override;
	void removeAllAttachments();
	void removeAttachment(std::string id);

	std::vector<Attachment>& attachments() {return m_attachments;};
	const std::optional<Attachment> getAttachment(std::string id);


private:
	std::vector<Attachment> m_attachments;

	std::string _buildAttachmentName(std::string parentName, int attachmentCount);
	void _removeFromWorldPass();

};

#endif

