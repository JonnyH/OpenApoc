#include "game/ui/city/infiltrationscreen.h"
#include "dependencies/spline/src/spline.h"
#include "forms/form.h"
#include "forms/graphic.h"
#include "forms/label.h"
#include "forms/ui.h"
#include "framework/event.h"
#include "framework/framework.h"
#include "framework/keycodes.h"
#include "game/state/gamestate.h"
#include "game/state/shared/organisation.h"
#include "library/line.h"
#include <array>

namespace OpenApoc
{

// The image seems to have 6 segments, each split into 7, so it's meant to be 6 weeks of values?
constexpr int num_steps = 6 * 7;

static void drawOrgLine(sp<RGBImage> image, const Organisation &org, const Colour &colour,
                        int steps)
{
	const double step_width = static_cast<double>(image->size.x - 1) / static_cast<double>(steps);
	constexpr int max_infiltration_value = 100;
	double infiltration_y_scale =
	    static_cast<double>(image->size.y - 1) / static_cast<double>(max_infiltration_value);

	// Initialise all steps to zero, in case there's not enough history (we assume anything
	// pre-game-start has 0 infilation anyway)
	auto step_values = std::vector<double>(steps, 0.0f);
	// First is always current infiltration value
	step_values[0] = static_cast<double>(org.infiltrationValue);
	int step = 1;
	for (const auto step_value : org.infiltrationHistory)
	{
		if (step > steps)
			return;
		step_values[step] = static_cast<double>(std::min(max_infiltration_value, step_value));
		step++;
	}

	auto image_lock = RGBImageLock(image);

	std::vector<double> points_x(steps, 0);
	std::vector<double> points_y(steps, 0);

	for (step = 0; step < steps; step++)
	{
		int dst_idx = steps - step - 1;
		// Reverse the steps, so they're in increasing X
		points_x[step] = static_cast<double>(step) * step_width;
		points_y[dst_idx] =
		    static_cast<double>(image->size.y) - step_values[step] * infiltration_y_scale;
	}

	tk::spline spline;
	//spline.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, true);
	spline.set_points(points_x, points_y);

	for (int x = 0; x < image->size.x - 1; x++)
	{
		int x1 = x;
		int x2 = x + 1;
		int y1 = static_cast<int>(spline(static_cast<double>(x1)));
		int y2 = static_cast<int>(spline(static_cast<double>(x2)));
		LineSegment<int, false> line(Vec3<int>(x1, y1, 0), Vec3<int>(x2, y2, 0));
		for (const auto &point : line)
		{
			image_lock.set(Vec2<int>(clamp<int>(lrint(point.x), 0, image->size.x - 1),
			                         clamp<int>(lrint(point.y), 0, image->size.y - 1)),
			               colour);
		}
	}
}

constexpr std::array<Colour, 10> line_colors = {
    Colour{195, 47, 47},  Colour{235, 79, 27}, Colour{243, 171, 87}, Colour{232, 247, 139},
    Colour{235, 213, 25}, Colour{24, 231, 24}, Colour{55, 145, 72},  Colour{5, 101, 255},
    Colour{54, 74, 200},  Colour{143, 15, 178}};

InfiltrationScreen::InfiltrationScreen(sp<GameState> state)
    : Stage(), menuform(ui().getForm("city/infiltration")), state(state)
{
	for (int i = 0; i < 10; i++)
	{
		shown_org_names[i] = menuform->findControlTyped<Label>(format("ORG_NAME_%d", i)).get();
		shown_orgs[i] = nullptr;
	}
	graph = menuform->findControlTyped<Graphic>("GRAPH");
}

InfiltrationScreen::~InfiltrationScreen() = default;

void InfiltrationScreen::begin()
{
	menuform->findControlTyped<Label>("TEXT_FUNDS")->setText(state->getPlayerBalance());
	this->update_view();
}

void InfiltrationScreen::pause() {}

void InfiltrationScreen::resume() {}

void InfiltrationScreen::finish() {}

void InfiltrationScreen::eventOccurred(Event *e)
{
	menuform->eventOccured(e);

	if (e->type() == EVENT_KEY_DOWN)
	{
		if (e->keyboard().KeyCode == SDLK_ESCAPE)
		{
			menuform->findControl("BUTTON_QUIT")->click();
			return;
		}
	}

	if (e->type() == EVENT_FORM_INTERACTION && e->forms().EventFlag == FormEventType::ButtonClick)
	{
		if (e->forms().RaisedBy->Name == "BUTTON_QUIT")
		{
			fw().stageQueueCommand({StageCmd::Command::POP});
			return;
		}
		if (e->forms().RaisedBy->Name == "BUTTON_TOPTEN")
		{
			this->reset_shown_orgs();
			return;
		}
	}
}

void InfiltrationScreen::reset_shown_orgs()
{
	std::vector<const Organisation *> orgs;

	for (const auto &org : state->organisations)
	{
		if (org.second->id == "ORG_ALIEN" || org.second->id == "ORG_X-COM")
			continue;
		if (org.second->infiltrationValue == 0)
			continue;
		orgs.push_back(org.second.get());
	}

	auto infiltration_comparer = [](const Organisation *org1, const Organisation *org2) {
		return org1->infiltrationValue > org2->infiltrationValue;
	};
	std::sort(orgs.begin(), orgs.end(), infiltration_comparer);

	for (int i = 0; i < 10; i++)
	{
		if (i >= orgs.size())
		{
			shown_orgs[i] = nullptr;
			continue;
		}
		shown_orgs[i] = orgs[i];
	}
	this->update_view();
}

void InfiltrationScreen::update_view()
{
	auto newGraphImage = mksp<RGBImage>(graph->Size);

	for (int i = 0; i < 10; i++)
	{
		if (shown_orgs[i])
		{
			shown_org_names[i]->setText(shown_orgs[i]->name);
			drawOrgLine(newGraphImage, *shown_orgs[i], line_colors[i], num_steps);
		}
		else
		{
			shown_org_names[i]->setText("");
		}
	}

	graph->setImage(newGraphImage);
}

void InfiltrationScreen::update() { menuform->update(); }

void InfiltrationScreen::render()
{
	fw().stageGetPrevious(this->shared_from_this())->render();
	menuform->render();
}

bool InfiltrationScreen::isTransition() { return false; }

}; // namespace OpenApoc
