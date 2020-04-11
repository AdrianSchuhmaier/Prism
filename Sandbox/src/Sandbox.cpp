#include "Prism.h"

#include <iostream>

struct test {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	std::string name;
};

struct Sandbox : public Prism::Application {

	Sandbox() : Prism::Application({}) {
	}

	~Sandbox() { std::cin.get(); }

	void OnUpdate(float dt) override
	{
	}

	void OnEvent(Prism::Event& e) override
	{
	}
};

APPLICATION_ENTRY_POINT{
	return new Sandbox();
}