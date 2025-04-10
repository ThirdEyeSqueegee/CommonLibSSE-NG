#include "RE/U/UI.h"

#include "RE/I/IMenu.h"

namespace RE
{
	UI* UI::GetSingleton()
	{
		static REL::Relocation<UI**> singleton{ RELOCATION_ID(514178, 400327) };
		return *singleton;
	}

	bool UI::GameIsPaused()
	{
		return numPausesGame > 0;
	}

	GPtr<IMenu> UI::GetMenu(const std::string_view& a_menuName)
	{
		auto it = menuMap.find(a_menuName);
		return it != menuMap.end() ? it->second.menu : nullptr;
	}

	GPtr<GFxMovieView> UI::GetMovieView(const std::string_view& a_menuName)
	{
		auto menu = GetMenu(a_menuName);
		return menu ? menu->uiMovie : nullptr;
	}

	bool UI::IsApplicationMenuOpen() const
	{
		return numApplicationMenus > 0;
	}

	bool UI::IsCursorHiddenWhenTopmost() const
	{
		return numDontHideCursorWhenTopmost == 0;
	}

	bool UI::IsItemMenuOpen() const
	{
		return numItemMenus > 0;
	}

	bool UI::IsMenuOpen(const std::string_view& a_menuName)
	{
		auto menu = GetMenu(a_menuName);
		return menu && menu->OnStack();
	}

	bool UI::IsModalMenuOpen() const
	{
		return modal;
	}

	bool UI::IsPauseMenuDisabled() const
	{
		return numDisablePauseMenu > 0;
	}

	bool UI::IsSavingAllowed() const
	{
		return numAllowSaving > 0;
	}

	bool UI::IsShowingMenus() const
	{
		return menuSystemVisible;
	}

	bool UI::IsUsingCustomRendering() const
	{
		return numCustomRendering > 0;
	}

	void UI::Register(std::string_view a_menuName, Create_t* a_creator)
	{
		menuMap.insert({ a_menuName, { nullptr, a_creator } });
	}

	void UI::ShowMenus(bool a_show)
	{
		menuSystemVisible = a_show;
	}

	void UI::GetTopMostMenu(RE::IMenu** a_result, std::uint32_t a_depthLimit)
	{
		using func_t = decltype(&UI::GetTopMostMenu);
		static REL::Relocation<func_t> func{ RELOCATION_ID(79944, 82081) };
		return func(this, a_result, a_depthLimit);
	}
}
