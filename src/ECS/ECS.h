#ifndef ECS_H
#define ECS_H

#include <bitset>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

// Signature
// We use a bitset (1s and 0s) to keep track of which components an entity has.
// This also helps keep track of which entities a system is interested in
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

// Used to assign a unique id to a component type.
template <typename TComponent>
class Component: public IComponent {
    // Returns the unique id of Component<T>
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default;
        int GetId() const;
        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const { return id == other.GetId(); }
        bool operator !=(const Entity& other) const { return id != other.GetId(); }
        bool operator <(const Entity& other) const { return id < other.GetId(); }
        bool operator >(const Entity& other) const { return id > other.GetId(); }
};

// System
// The system process entities that contain a specific signature.
class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        const Signature& GetComponentSignature() const;
        std::vector<Entity> GetSystemEntities() const;

        // Defines a component type that entities must have
        // to be considered by the system.
        template <typename TComponent> void RequireComponent();
};

class IPool {
    public:
        virtual ~IPool() {}
};

// Pool
// A pool is just a vector (contiguous data) of objects of type T.
template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;

    public:
        Pool(int size = 100) { data.resize(size); }
        virtual ~Pool() = default;
        bool IsEmpty() const { return data.empty(); }
        int GetSize() const { return data.size(); }
        void Resize(int n) { data.resize(n); }
        void Clear() { data.clear(); }
        void Add(T object) { data.push_back(object); }
        void Set(int index, T object) { data[index] = object; }
        T& Get(int index) const { return static_cast<T&>(data[index]); }
        T& operator [](unsigned int index) { return data[index]; }
};

// Registry
// The registry manages the creation and destruction of entities, systems, and components.
class Registry {
    private:
        int numEntities = 0;
        std::set<Entity> entitiesToBeAdded; // Entities awaiting creation in the next Registry Update()
        std::set<Entity> entitiesToBeKilled; // Entities awaiting destruction in the next Registry Update()

        // Vector of component pools, each pool contains
        // all of the data for a specific component type.
        // Vector index = component type id
        // Pool index = entity id
        std::vector<IPool*> componentPools;

        // Vector of component signatures.
        // The signature lets us know which components are turned "on" for an entity.
        // Vector index = entity id
        std::vector<Signature> entityComponentSignatures;

        // Map of active systems [index = system type id]
        std::unordered_map<std::type_index, System*> systems;

    public:
        Registry() = default;

        void Update();
        Entity CreateEntity();
        void AddEntityToSystem(Entity entity);

        template <typename T, typename ...TArgs>
        void AddComponent(Entity entity, TArgs&& ...args);

        template <typename T>
        void RemoveComponent(Entity entity);

        template <typename T>
        bool HasComponent(Entity entity) const;

        template <typename T>
        T& GetComponent(Entity) const;

        // AddSystem()
};


template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    // If the component id is greater than the current size of the componentPools, then resize the vector.
    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    // If we still don't have a Pool for that compontent type, create one.
    if (!componentPools[componentId]) {
        Pool<T>* newComponentPool = new Pool<T>();
        componentPools[componentId] = newComponentPool;
    }

    // Get the pool of component values for that component type.
    Pool<T>* componentPool = Pool<T>(componentPools[componentId]);

    // If the entity id is greater than the current size of the component pool, then resize the pool.
    if (entityId >= componentPool->GetSize()) {
        componentPool->Resize(numEntities);
    }

    // Create a new Component object of the type T
    // and forward the various parameters to the constructor.
    T newComponent(std::forward<TArgs>(args)...);

    // Add the new component to the component pool list, using the entity id as index.
    componentPool->Set(entityId, newComponent);

    // Finally, change the component signature of the entity and set the component id on the bitset to 1.
    entityComponentSignatures[entityId].set(componentId);
}

#endif
