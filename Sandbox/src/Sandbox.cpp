#include "Prism.h"

struct Sandbox : public Prism::Application {

	Prism::Entity entity;

	Sandbox() : Prism::Application({})
	{
	}

	~Sandbox()
	{
	}

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