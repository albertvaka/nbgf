#include "plant.h"
#include "assets.h"
#include "debug.h"
#include "input.h"
#include "stats_tracker.h"

#ifdef _IMGUI
#include "imgui.h"
#endif

const float kSecondsToGrow = 10.0f;
const int kMaxHeight = 4;
const vec kCarryPositionOffset = { 8.0f, -8.0f };

const float kMaxStats = 50.0f;
const float kInitialStats = 35.f;

const float kLightIncreasePerSecond = 4.5f;
const float kWaterIncreasePerWatering = 30.0f;

const float kAdditionalWaterLostWhenGetsLight = 1.0f;

const float kLightLostPerSecond = 0.25f;
const float kWaterLostPerSecond = 0.5f;

const float kWaterAndLighThresholdToGrow = 40.0f;

const float kWaterBubbleDuration = 3.0f;

extern float mainClock;

static int ids = 0;
Plant::Plant(vec pos) : BoxEntity(pos, vec(16.0f, 16.0f)) {
    water = kInitialStats;
    light = kInitialStats;
    id = ids++;
    mirror = Rand::OnceEvery(2);
}

void Plant::Update(float dt) {

    pos.DebugDraw();

    if (carrier != nullptr) {
        pos = carrier->pos;
        pos.x += (carrier->lookingLeft ? -1 : 1) * kCarryPositionOffset.x;
        pos.y += kCarryPositionOffset.y - 8.0f * height;
    }

    if (!alive) return;

    water -= kWaterLostPerSecond * dt;
    light -= kLightLostPerSecond * dt;

    growClock += dt;

    if (water >= kWaterAndLighThresholdToGrow && light >= kWaterAndLighThresholdToGrow && growClock > kSecondsToGrow) {
        growClock = 0;
        if (height >= kMaxHeight) {
            if (!has_tomato) {
                has_tomato = true;
                // Random offset.
                for (int i = 0; i < kNumTomatoes; ++i) {
                    tomato_offset[i] = vec(Rand::rollf(-8.0f, 8.0f), Rand::rollf(-16.0f, 16.0f));
                }
            }
        }
        else {
            ++height;
            pos.y -= 8.0f;
            size.y += 16.0f;
        }
    }
  
    if (gets_light) {
        light = std::min(kMaxStats, light + dt * kLightIncreasePerSecond);
        water -= kAdditionalWaterLostWhenGetsLight * dt;
    }
    if (gets_water) {
        water = std::min(kMaxStats, water + kWaterIncreasePerWatering);
        gets_water = false;
        time_left_water_bubble = kWaterBubbleDuration;
    } else {
      time_left_water_bubble -= dt;
    }

      if (light < 0 || water < 0) {
          alive = false;
          has_tomato = false;
          ++StatsTracker::plants_died;
          Assets::soundDeath.Play();
          if (height >= kMaxHeight) {
              height = kMaxHeight - 1;
              pos.y += 8.0f;
              size.y -= 16.0f;
          }
      }
          
#ifdef _IMGUI
      ImGui::Begin((std::string("plant") + ::std::to_string(id)).c_str());
      ImGui::SliderFloat("water", &water, 0.f, kMaxStats);
      ImGui::SliderFloat("light", &light, 0.f, kMaxStats);
      ImGui::Checkbox("alive", &alive);
      ImGui::End();
#endif
}

bool Plant::HasTomato() const {
  return has_tomato;
}

void Plant::PickTomato() {
  has_tomato = false;
  growClock = 0;
  ++StatsTracker::tomatoes_collected;
}

void Plant::Draw() const {
  // Draw tomato
  if (has_tomato) {
    for (int i = 0; i < kNumTomatoes; ++i) {
      Window::Draw(Assets::plantTexture, pos + tomato_offset[i])
          .withOrigin(5, 5)
          .withRect(22, 54, 10, 10);
    }
  }

  uint8_t color = alive? 255 : 128;
  Window::Draw(Assets::plantTexture, pos)
      .withRect(0, 64 - 16 * height, 16, 16 * height)
      .withColor(color, color, color, 255)
      .withScale(mirror? -1.f : 1.f, 1.f)
      .withOrigin(size / 2.0f);

  // Draw stats if they are changing
  bool draw_light_bubble = (int(mainClock) % 2 == 0);
  bool draw_water_bubble = !draw_light_bubble;
  if (alive && gets_light) {
    if (draw_light_bubble) {
      Window::Draw(Assets::plantTexture, vec(pos) + vec(10.0f, -23.f))
          .withRect(34, 0, 18, 23);
    }
  }
  if (alive && time_left_water_bubble > 0.0f) {
    if (draw_water_bubble) {
      Window::Draw(Assets::plantTexture, vec(pos) + vec(10.0f, -23.0f))
          .withRect(34, 23, 18, 23);
    }
  }
  // Draw faces on the plant underneath.
    // Background
  Window::Draw(Assets::plantTexture, vec(pos.x, getBottomY()) + vec(0.0f, 2.0f))
      .withOrigin(32/2,0)
      .withRect(52, alive?0:16,30,16);

  if (alive) {

    // Sun face
    int sun_happiness = std::min(int(light/10), 4);
    Window::Draw(Assets::plantTexture, vec(pos.x, getBottomY()) + vec(-8.0f, 5.0f))
        .withOrigin(6, 0)
        .withRect(94, 52 - 12 * sun_happiness, 12, 12);

    // Water face
    int water_happiness = std::min(int(water/10), 4);
    Window::Draw(Assets::plantTexture, vec(pos.x, getBottomY()) + vec(6.0f, 5.0f))
        .withOrigin(6, 0)
        .withRect(82, 52 - 12 * water_happiness, 12, 12);
  }

}

float Plant::getBottomY() const
{
  return pos.y + 8.0f * height;
}

bool Plant::IsBeingCarried() const
{
  return carrier != nullptr;
}

bool Plant::IsCarriedBy(const JumpMan* carrier) const
{
  return carrier == this->carrier;
}

void Plant::PickUpBy(const JumpMan* player)
{
  carrier = player;
}

void Plant::Drop()
{
  pos.y = carrier->pos.y - 8.0f * height;
  carrier = nullptr;
}

void Plant::SetHitByWater()
{
  gets_water = true;
  ++StatsTracker::plant_watered;
}

void Plant::SetHitByLight(bool hit)
{
  gets_light = hit;
}

