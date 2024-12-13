#include "framework/framework.h"
#include "framework/logger.h"
#include "game/state/gamestate.h"
#include "fmt/format.h"
#include <entt/entt.hpp>
#include <cstdint>

struct ExampleComponentPosition
{
	OpenApoc::Vec2<float> position;
};

struct ExampleComponentName
{
	std::string name;
};

enum class uint64_entity_type : uint64_t;

using registry_type = entt::basic_registry<uint64_entity_type>;
//using registry_type = entt::registry;

struct TestArchive
{
	void operator()(registry_type::entity_type entity)
	{
		fmt::print("snapshot Entity: {}\n", entity);
	}
	void operator()(std::underlying_type_t<registry_type::entity_type> count)
	{
		fmt::print("snapshot Entity count: {}\n", count);
	}
	void operator()(const ExampleComponentPosition& position)
	{
		fmt::print("snapshot Entity Position: {}\n", position.position);
	}
	void operator()(const ExampleComponentName& name)
	{
		fmt::print("snapshot Entity name: {}\n", name.name);
	}
};

int main(int argc, char **argv)
{

	registry_type registry;
	std::vector<registry_type::entity_type> entities;

	for (int i = 0; i < 10; i++)
	{
		const auto entity = registry.create();
		fmt::print("Creating entity {}\n", entity);
		if (i % 2 == 0) registry.emplace<ExampleComponentName>(entity, fmt::format("Entity {}", i));
		if (i % 2 == 1) registry.emplace<ExampleComponentPosition>(entity, OpenApoc::Vec2<float>{i, i});
		entities.push_back(entity);
	}

	auto view = registry.view<const ExampleComponentPosition>();
	auto view2 = registry.view<const ExampleComponentName>();
	view.each([](const auto& position)
	{
		fmt::print("Entity position {}\n",  position.position);
	});
	view2.each([](const auto& name)
	{
		fmt::print("Entity name {}\n", name.name);
	});

	fmt::print("Destroying entity [1]\n");
	registry.destroy(entities[1]);

	view.each([](const auto& position)
	{
		fmt::print("Entity position {}\n",  position.position);
	});
	view2.each([](const auto& name)
	{
		fmt::print("Entity name {}\n", name.name);
	});

	for (const auto entity : entities)
	{
		auto valid = registry.valid(entity);
		fmt::print("Entity {} valid: {}\n", entity, valid);
	}

	auto snapshot = entt::basic_snapshot<registry_type>{registry};




	TestArchive archive;

	snapshot.get<ExampleComponentPosition>(archive)
	.get<ExampleComponentName>(archive);


	for (int i = 10; i < 20; i++)
	{
		const auto entity = registry.create();
		fmt::print("Creating entity {}\n", entity);
		registry.emplace<ExampleComponentName>(entity, fmt::format("Entity {}", i));
		registry.emplace<ExampleComponentPosition>(entity, OpenApoc::Vec2<float>{i, i});
		entities.push_back(entity);
	}

	 view = registry.view<const ExampleComponentPosition>();
	 view2 = registry.view<const ExampleComponentName>();

	view.each([](const auto& position)
{
	fmt::print("Entity position {}\n",  position.position);
});
	view2.each([](const auto& name)
	{
		fmt::print("Entity name {}\n", name.name);
	});

	for (const auto entity : entities)
	{
		auto valid = registry.valid(entity);
		auto version = entt::to_version(entity);
		fmt::print("Entity {} version: {} valid: {}\n", entity, version, valid);
	}

	LogInfo("test_entity success");




	TestArchive archive2;

	snapshot.get<ExampleComponentPosition>(archive2)
	.get<ExampleComponentName>(archive2);

	return EXIT_SUCCESS;
}
