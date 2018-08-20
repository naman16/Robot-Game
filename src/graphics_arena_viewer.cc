/**
 * @file graphics_arena_viewer.cc
 *
 * @copyright 2017 3081 Staff, All rights reserved.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <vector>
#include <iostream>
#include <string>
#include "src/graphics_arena_viewer.h"
#include "src/arena_params.h"
#include "src/rgb_color.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NAMESPACE_BEGIN(csci3081);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
GraphicsArenaViewer::GraphicsArenaViewer(
    const struct arena_params *const params,
    Arena * arena, Controller * controller) :
    GraphicsApp(
        params->x_dim + GUI_MENU_WIDTH + GUI_MENU_GAP * 2,
        params->y_dim,
        "Robot Simulation"),
    controller_(controller),
    arena_(arena) {
  auto *gui = new nanogui::FormHelper(screen());
  nanogui::ref<nanogui::Window> window =
      gui->addWindow(
          Eigen::Vector2i(10 + GUI_MENU_GAP, 10),
          "Menu");

  gui->addGroup("Simulation Control");
  playing_button_ =
    gui->addButton(
      "Play",
      std::bind(&GraphicsArenaViewer::OnPlayingBtnPressed, this));
  // adding a new button in the window
  newgame_result_button_ =
    gui->addButton(
      "Welcome",
      std::bind(&GraphicsArenaViewer::OnResultBtnPressed, this));
  screen()->performLayout();
}

/*******************************************************************************
 * Member Functions
 ******************************************************************************/

// This is the primary driver for state change in the arena.
// It will be called at each iteration of nanogui::mainloop()
void GraphicsArenaViewer::UpdateSimulation(double dt) {
  if (!paused_) {
    controller_->AdvanceTime(dt);
  }
}

// Function to deal with the situation once the game is over
void GraphicsArenaViewer::GameOver() {
  game_result_ = true;  // setting the flag true because we have a result
  paused_ = true;  // game should be paused because game over
  playing_button_->setCaption("New Game");
  if (arena_->get_game_status() == WON) {
    newgame_result_button_->setCaption("You Won");  // displaying the result
  } else {
    newgame_result_button_->setCaption("You Lost");  // displaying the result
    }
}

/*******************************************************************************
 * Handlers for User Keyboard and Mouse Events
 ******************************************************************************/
//  Reset the arena when the New Game button is pressed
void GraphicsArenaViewer::OnResultBtnPressed() {
  if (!game_result_) {
    game_result_ = false;
    game_start_ = false;
    paused_ = true;
    playing_button_->setCaption("Play");
    controller_ -> AcceptCommunication(kNewGame);
  }
}

// Function to deal with the button presses during in the viewer
// during different stages of the game.
void GraphicsArenaViewer::OnPlayingBtnPressed() {
  // When button pressed before the start of the game
  if (paused_ && !game_start_) {
    paused_ = !paused_;
    game_start_ = true;
    playing_button_->setCaption("Pause");
    newgame_result_button_->setCaption("New Game");
    controller_ -> AcceptCommunication(kPlay);
    return;
  } else if (game_result_) {  // buttons are pressed when game over
    game_result_ = false;
    game_start_ = false;
    paused_ = true;
    playing_button_->setCaption("Play");
    newgame_result_button_->setCaption("New Game");
    controller_ -> AcceptCommunication(kNewGame);
  } else if (!paused_) {  // when "Pause" is pressed during the game
    paused_ = true;
    playing_button_->setCaption("Play");
    controller_ -> AcceptCommunication(kPlay);
  } else {  // when "Play" is pressed to resume the game
    paused_ = false;
    playing_button_->setCaption("Pause");
    controller_ -> AcceptCommunication(kPause);
  }
}

/** OnSpecialKeyDown is called when the user presses down on one of the
  * special keys (e.g. the arrow keys).
  */
/**
 * @TODO: Check for arrow key presses using GLFW macros, then
 * convert to appropriate enum Communication and relay to controller
 */
void GraphicsArenaViewer::OnSpecialKeyDown(int key,
  __unused int scancode, __unused int modifiers) {
    Communication key_value = kNone;
    switch (key) {
      case GLFW_KEY_LEFT: key_value = kKeyLeft;
        break;
      case GLFW_KEY_RIGHT: key_value = kKeyRight;
        break;
      case GLFW_KEY_UP: key_value = kKeyUp;
        break;
      case GLFW_KEY_DOWN: key_value = kKeyDown;
        break;
      default: {}
    }
  controller_->AcceptCommunication(key_value);
}




/*******************************************************************************
 * Drawing of Entities in Arena
 ******************************************************************************/
void GraphicsArenaViewer::DrawRobot(NVGcontext *ctx,
                                     const Robot *const robot) {
  // translate and rotate all graphics calls that follow so that they are
  // centered, at the position and heading of this robot
  nvgSave(ctx);
  nvgTranslate(ctx,
               static_cast<float>(robot->get_pose().x),
               static_cast<float>(robot->get_pose().y));
  nvgRotate(ctx,
            static_cast<float>(robot->get_pose().theta * M_PI / 180.0));

  // robot's circle
  nvgBeginPath(ctx);
  nvgCircle(ctx, 0.0, 0.0, static_cast<float>(robot->get_radius()));
  nvgFillColor(ctx,
               nvgRGBA(robot->get_color().r, robot->get_color().g,
                       robot->get_color().b, 255));
  nvgFill(ctx);
  nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
  nvgStroke(ctx);

  // robot id text label
  nvgSave(ctx);
  nvgRotate(ctx, static_cast<float>(M_PI / 2.0));
  nvgFillColor(ctx, nvgRGBA(0, 0, 0, 255));
  std::string r_ = "Lives:" + std::to_string(robot->get_lives());
  nvgText(ctx, 0.0, 4.0, r_.c_str(), nullptr);
  // nvgText(ctx, 0.0, 10.0, robot->get_lives(), nullptr);
  nvgRestore(ctx);
  nvgRestore(ctx);
}
void GraphicsArenaViewer::DrawArena(NVGcontext *ctx) {
  nvgBeginPath(ctx);
  // Creates new rectangle shaped sub-path.
  nvgRect(ctx, 0, 0, arena_->get_x_dim(), arena_->get_y_dim());
  nvgStrokeColor(ctx, nvgRGBA(255, 255, 255, 255));
  nvgStroke(ctx);
}

void GraphicsArenaViewer::DrawEntity(NVGcontext *ctx,
                                       const ArenaEntity *const entity) {
  // obstacle's circle
  nvgBeginPath(ctx);
  nvgCircle(ctx,
            static_cast<float>(entity->get_pose().x),
            static_cast<float>(entity->get_pose().y),
            static_cast<float>(entity->get_radius()));
  nvgFillColor(ctx,
               nvgRGBA(entity->get_color().r, entity->get_color().g,
                       entity->get_color().b, 255));
  nvgFill(ctx);
  nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, 255));
  nvgStroke(ctx);

  // obstacle id text label
  nvgFillColor(ctx, nvgRGBA(0, 0, 0, 255));
  nvgText(ctx,
          static_cast<float>(entity->get_pose().x),
          static_cast<float>(entity->get_pose().y),
          entity->get_name().c_str(), nullptr);
}

void GraphicsArenaViewer::DrawUsingNanoVG(NVGcontext *ctx) {
  // initialize text rendering settings
  nvgFontSize(ctx, 18.0f);
  nvgFontFace(ctx, "sans-bold");
  nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  DrawArena(ctx);
  std::vector<ArenaEntity *> entities = arena_->get_entities();
  for (auto &entity : entities) {
    DrawEntity(ctx, entity);
  } /* for(i..) */
  DrawRobot(ctx, arena_->robot());
}

NAMESPACE_END(csci3081);
