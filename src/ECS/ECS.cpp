#include "../Logger/Logger.h"
#include "ECS.h"

int Entity::GetId() const {
    return id;
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
    }), entities.end());
}

const Signature& System::GetComponentSignature() const {
    return componentSignature;
}

std::vector<Entity> System::GetSystemEntities() const {
    return  entities;
}

Entity Registry::CreateEntity() {
    int entityId = numEntities++;
    Entity entity(entityId);
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id = " + std::to_string(entityId));

    return entity;
}

void Registry::Update() {
    // TODO: Add entities that are waiting to be created to the active Systems.
    // TOOD: Remove entities that are waiting to be removed from the active Systems.
}
