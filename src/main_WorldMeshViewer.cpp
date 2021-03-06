#include <memory>

#include <BsFPSCamera.h>
#include <Components/BsCCamera.h>
#include <Scene/BsSceneObject.h>

#include <core.hpp>
#include <world/internals/ConstructFromZEN.hpp>

class REGothWorldMeshViewer : public REGoth::EmptyGame
{
public:
  using REGoth::EmptyGame::EmptyGame;

  void setupMainCamera() override
  {
    REGoth::Engine::setupMainCamera();

    mFPSCamera = mMainCamera->SO()->addComponent<bs::FPSCamera>();
  }

  void setupScene() override
  {
    REGoth::Internals::loadWorldMeshFromZEN("ADDONWORLD.ZEN");
  }

protected:
  bs::HFPSCamera mFPSCamera;
};

int main(int argc, char** argv)
{
  auto config = REGoth::parseArguments<REGoth::EngineConfig>(argc, argv);
  REGothWorldMeshViewer engine{std::move(config)};

  return REGoth::runEngine(engine);
}
