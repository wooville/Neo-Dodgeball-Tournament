https://github.com/wooville/UTN2023/assets/47064476/86101d48-2fa1-47d7-8480-10a3f08617db

## ECS Implementation
This engine uses a basic ECS implementation of Entity, Component, and System classes. Components and Systems use template types so that their functions can be used generically for each specific Component/System. A Registry class organizes the E/C/S in use. Template types are used for scalability of components and systems by allowing generic functionality for those components and systems.

Each Entity has a unique ID generated for it upon creation. If an Entity is killed, its ID will be
reused as the next Entity is created. Entities can be tagged, grouped, compared by ID (using
overloaded operators), or have Components added to them. To enable access to the tag, group,
and Component management functionality of the Registry from an instance of the Entity class, a
reference to the Entity's Registry is stored as a class variable.

The Registry manages Systems and entities as well as their Components, tags, and groups. The Registry will synchronize the addition and removal of entities in its Update() function by keeping track of a vector of entities to be added & removed.

The Registry tracks Components using Pools. The Pool class is a wrapper on top of a vector which is used to identify which Components each Entity has enabled. In the spirit of data-oriented design, each Pool is a vector representing one Component's list of entities contiguously in memory (as opposed to randomly dispersed), using hash maps to allow the user to index the data vector by the Entity ID as well as retrieve the ID of an Entity at a given index.

### Events & Event Bus
Each Event type has its own list of arguments. The EventBus handles the emitting of Events and subscription of Systems to different types of Events. When an Event is emitted through the Event Bus, any subscribed Systems will execute the callback function that is pointed to for that Event by that System in its SubscribeToEvents method.

## Components
The a list of Components is shown below with their respective parameters:
#### Animation Component
int numFrames; int currentFrame; int frameSpeedRate; bool isLoop; int startTime;
#### BoxCollider Component
int width; int height; int offsetX; int offsetY;
#### Health Component
int health_val;
#### Projectile Component
bool isFriendly; int hitDamage; int duration; int startTime;
#### ProjectileEmitter Component
float projectileVelocityX; float projectileVelocityY; int repeatFreq; int projectileDuration; int hitDamage; bool isFriendly; int lastEmissionTime;
Can be used for repeating emission or event-based emission
#### RigidBody Component
float velocityX, float velocityY, float acceleration;
#### ScriptedBehaviour Component
std::shared_ptr <IScriptedBehaviour> script;
Used to attach scripts to individual entities such as player, enemies, game manager
#### Sprite Component
CSimpleSprite *simpleSprite; std::string assetId; float width; float height; int zIndex;
#### TextLabel Component
std::vector<std::pair<std::pair<float, float>, std::string>> textToRender;
Vector containing pairs of coordinates and strings to render at those coordinates
#### Transform Component
float x; float y; float scale; float rotation;

## Systems
### Animation System
#### Required Components: AnimationComponent, SpriteComponent
not used, but calls CSimpleSprite’s Update()

### Collision System
#### Required Components: BoxColliderComponent, TransformComponent
This System checks for AABB collision on update and emits a CollisionEvent if it finds one.

### Damage System
#### Required Components: BoxColliderComponent
This System is subscribed to CollisionEvents and handles projectile collisions with players and enemies, reducing their HealthComponent health values.

### Movement System
#### Required Components: TransformComponent, RigidBodyComponent
This System handles the movement of relevant entities based on their velocity (scaled by deltaTime) and current position.

### ProjectileEmit System
#### Required Components: ProjectileEmitterComponent, TransformComponent
This System emits projectiles from relevant entities to the specification of their ProjectileEmitterComponent values.

### ProjectileLifecycle System
#### Required Components: ProjectileComponent
This System kills projectile entities after their duration has expired and converts them into pickups.

### Render System
#### Required Components: TransformComponent, SpriteComponent
This System sorts and then renders relevant entities in Z-Index order using the provided API. Entities outside of camera bounds are excluded from rendering. 

### RenderText System
#### Required Components: TextLabelComponent
This System renders each of the TextLabelComponent’s strings at their proper coordinates using the API's App::Print().

### ScriptedBehaviour System
#### Required Components: ScriptedBehaviourComponent
This System loops through each of the ScriptedBehaviours, updates them and subscribes them to events based on their definitions.
