#pragma once

#include <string>
#include <sstream>

#include "window.h"
#include "camera.h"
#include "text.h"
#include "bounds.h"
#include "assets.h"

struct StatsTracker {

  static void Restart() {
      
    tomatoes_collected = 0;
    tomatoes_delivered = 0;
    doggos_in_game = 0;
    doggos_fed = 0;
    plants_died = 0;
    plants_purchased = 0;
    plant_watered = 0;
    plant_pickedup = 0;
    basked_pickedup = 0;
    jumps_done = 0;
    walljumps_done = 0;
    time_played = 0;
    delete statsText;
  }
  
  static void Draw() {
    const float margin = 10.0f;
    const float textScale = 0.4f;

    vec center = Camera::Center();

    vec textSize = statsText->Size() * textScale;
    BoxBounds b(vec(center.x - textSize.x / 2 - margin, center.y - margin), vec(textSize.x + 2 * margin, textSize.y + 2 * margin));

    Window::DrawPrimitive::Rectangle(b, -1, 255, 255, 200, 128); //fill
    Window::DrawPrimitive::Rectangle(b, 4, 0, 0, 0, 255); //border

    Window::Draw(*statsText, center)
        .withScale(textScale)
        .withOrigin(statsText->Size().x / 2, 0);
  }

  static void Update(float dt) {
      time_played += dt;
  }

  static void Stop() {

    int min= time_played / 60;
    int sec = int(time_played)%60;

    std::stringstream ss;
    ss << "Stats:" << "\n\n"
      << "Time alive: " << std::to_string(min) << "m " << std::to_string(sec) << "s\n"
      << "Tomatoes sold: " << std::to_string(tomatoes_delivered) << "\n"
      << "Tomatoes collected: " << std::to_string(tomatoes_collected) << "\n"
      << "Doggos in game: " << std::to_string(doggos_in_game) << "\n"
      << "Tomatoes fed to Doggo: " << std::to_string(doggos_fed) << "\n"
      << "Plants purchased: " << std::to_string(plants_purchased) << "\n"
      << "Plants watered: " << std::to_string(plant_watered) << "\n"
      << "Plants picked up: " << std::to_string(plant_pickedup) << "\n"
      << "Baskets picked up: " << std::to_string(basked_pickedup) << "\n"
      << "Jumps done: " << std::to_string(jumps_done) << "\n"
      << "Walljumps done: " << std::to_string(walljumps_done) << "\n"
      << "\nPress ESC to play again!\n";
    statsText = new Text(Assets::font_30);
    statsText->SetString(ss.str());
    statsText->SetFillColor(0, 0, 0);
    statsText->SetEmptyLinesMultilineSpacing(25);

    time_played = 0;
  }
  
  static float time_played;
  static int tomatoes_delivered;
  static int tomatoes_collected;
  static int doggos_in_game;
  static int doggos_fed;
  static int plants_purchased;
  static int plant_watered;
  static int plants_died;
  static int plant_pickedup;
  static int basked_pickedup;
  static int jumps_done;
  static int walljumps_done;
  static Text* statsText;
};
