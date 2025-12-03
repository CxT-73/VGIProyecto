#pragma once
class MenuController;

class IMenuState {
public:
    virtual ~IMenuState() {};
    virtual void Render(MenuController& controller) = 0;
};