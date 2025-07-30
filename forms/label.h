#pragma once

#include "forms/control.h"
#include "forms/forms_enums.h"
#include "library/sp.h"
#include "library/strings.h"

namespace OpenApoc
{

class BitmapFont;
class ScrollBar;

class Label : public Control
{

  private:
	UString text;
	sp<BitmapFont> font;
	int scrollOffset;

  protected:
	void onRender() override;

  public:
	sp<ScrollBar> scroller;
	HorizontalAlignment TextHAlign;
	VerticalAlignment TextVAlign;
	bool WordWrap;
	Colour Tint{255, 255, 255, 255};
	bool wordWrapped;

	Label(const UString &Text = "", sp<BitmapFont> font = nullptr);
	~Label() override;

	void eventOccured(Event *e) override;
	void update() override;
	void unloadResources() override;

	UString getText() const;
	void setText(const UString &Text);

	sp<BitmapFont> getFont() const;
	void setFont(sp<BitmapFont> NewFont);

	sp<Control> copyTo(sp<Control> CopyParent) override;
	void configureSelfFromXml(pugi::xml_node *node) override;
};

}; // namespace OpenApoc
