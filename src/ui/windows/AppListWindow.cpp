#include "AppListWindow.hpp"
#include "LoadingOverlay.hpp"
#include "AppButton.hpp"
#include "StreamWindow.hpp"

using namespace nanogui;

AppListWindow::AppListWindow(Widget *parent, const std::string &address): ContentWindow(parent, "Applications"), m_address(address) {
    set_left_pop_button();
    
    container()->set_layout(new GridLayout(Orientation::Horizontal, 5, Alignment::Minimum, 30, 18));
}

void AppListWindow::window_appear() {
    for (auto button: m_app_buttons) {
        if (container()->child_index(button) != -1) {
            container()->remove_child(button);
        }
    }
    m_app_buttons.clear();
    
    auto loader = add<LoadingOverlay>();
    
    GameStreamClient::client()->applist(m_address, [this, loader](auto result) {
        loader->dispose();
        
        if (result.isSuccess()) {
            int currentGame = GameStreamClient::client()->server_data(m_address).currentGame;
            PAPP_LIST app = result.value();
            
            while (app != NULL) {
                auto button = container()->add<AppButton>(m_address, *app, currentGame);
                m_app_buttons.push_back(button);
                button->set_callback([this, app] {
                    run_game(app->id);
                });
                app = app->next;
            }
            perform_layout();
        } else {
            screen()->add<MessageDialog>(MessageDialog::Type::Information, "Error", result.error());
        }
    });
}

void AppListWindow::run_game(int app_id) {
    push<StreamWindow>(m_address, app_id);
}
